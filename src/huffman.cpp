/**
file: huffman.cpp
the implementation of a simple huffman encoder/decoder


Copyright (C) 2006 Paul Fritsche <paul.fritsche@gmx.net>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

#ifdef _MMGR
#include "mmgr.h"
#endif

#include "huffman.h"
#include "log.h"

#include "nlog.h"
namespace bfs{

Huffman::Huffman()
{
    mTree=NULL;
    reset();
}

Huffman::~Huffman()
{
    if( mTree ) delete[] mTree;
    mNoNodes = 0;
}

void Huffman::reset()
{
     if( mTree ) delete[] mTree;
 	 mTree = NULL;
 	 mDict.clear();
 	 mDictRev.clear();
 	 mStrings.clear();
 	 mNoNodes = 0;
 	 memset( mCharacters, 0, 4*255 );
 	 mStrings.clear();
}

string Huffman::decryptString(char *buf, int packsize, int unpacksize )
{
     int pattern = 1;
     char res[unpacksize+3];
     char *r = res;
     // for each char in cipher
     for( int i=0;i<packsize; i++ )
     {
	  	  unsigned char b = buf[i];
	  	  // for each bit in char
	  	  for( int j=0;j<8;j++ )
	  	  {
		   	   // append bit to pattern
		   	   char bit = b&1;
		   	   b>>=1;
		   	   pattern<<=1;
		   	   pattern|=bit;
		   	   LOG((int)bit);
		   	   if( pattern>1 )
		   	   {
			   	   // valid pattern?
			   	   map<int, char>::iterator it = mDict.find(pattern);
			   	   if( it!=mDict.end() )
			   	   {
				   	   // we have our decrypted char
				   	   *r = mDict[pattern];
				   	   LOG(*r);
				   	   r++;
				   	   pattern = 1;
			       }
			   }
		   }
		   LOGN("");
	 }
	 return string( res ).substr( 0, unpacksize );
}

int Huffman::encryptString(string str, char *buf)
{
 	char currbit = 0;
 	int bytes = 0;
 	// for each char
 	*buf = 0;
 	for( unsigned int i=0; i<str.length(); i++ )
 	{
	 	 char c = str[i];
	 	 LOG(c);
	 	 // find the key
	 	 int key = mDictRev[c];
	 	 // and add every bit to the key to the output bitstream
	 	 while( key>1 )
	 	 {
		  		char bit = key&1;
		  		key>>=1;
		  		*buf |= (bit<<currbit);
		  		LOG((int)bit);
		  		if( currbit==7 )
		  		{
				 	buf++;
				 	*buf=0;
				 	currbit=0;
				 	bytes++;
				 	LOGN(" ");
				} else
				{
				    currbit++;
				    LOG(" ");
				}	
         }
    }
    return bytes+1;
}

short* Huffman::deserialize( short *buffer, int bytes, int key )
{
 	 if( bytes<2 ) return 0;
 	 short value;
 	 value = *buffer;
 	 buffer ++;
 	 if( (value/256)==-127 )
 	 {
	  	 // we hava a leaf
	  	 mDict[key] = (char) (value%256);
	 } else
	 {
         buffer = deserialize( buffer, bytes-2, (key<<1)+1 );
         buffer = deserialize( buffer, bytes-2, (key<<1) );
     }
     return buffer;
}

int Huffman::serialize( short *buffer )
{
 	 countTreeRec( mRoot, countNodesRec( mRoot->left)+1  );
 	 short *o = buffer;
 	 short *n = walkTreeRec( mRoot, buffer );
 	 return n-o;
}

void Huffman::finalize()
{
 	 mNoNodes = 0;
 	 // count characters
 	 list<string>::iterator it;
	 for( it=mStrings.begin(); it!=mStrings.end(); it++ )
	 {
	  	  string s = *it;
	  	  for( unsigned int j=0; j<s.length(); j++ )
	  	  {
		   	   if( mCharacters[(int)s[j]]==0 ) mNoNodes++;
		   	   mCharacters[(int)s[j]]++;
		  }
     }
 	 // alloc memory
 	 mNoNodes = mNoNodes * 2 - 1; 
 	 mTree = new HuffmanTree[ mNoNodes ];
 	 memset( mTree, -1, sizeof( HuffmanTree ) * mNoNodes );
 	 int pos = 0;
 	 // initialize nodes for each character
 	 for( int i=0; i<255; i++ )
 	 {
	  	  if( mCharacters[i]>0 )
	  	  {
		   	  mTree[pos].left = mTree[pos].right = NULL;
		   	  mTree[pos].sign = (char) i;
		   	  mTree[pos].number = mCharacters[i];
		   	  mTree[pos].connected = false;
		   	  pos++;
		  }
     }
     
 	 mRoot = NULL;
 	 // build tree, taking the two smallest nodes and connect them
 	 HuffmanTree *one, *two;
 	 while( one = findSmallestNode() )
 	 {
 		 one->connected = true;
 		 two = findSmallestNode();
		  // no node found -> break immediatly, one becomes root (is last node)
 		 if( two==0 )
 		 {
		  	 mRoot = one;
		  	 break;
		 }
 		 two->connected = true;
		 mTree[pos].left = one;
		 mTree[pos].right = two;
		 mTree[pos].number = one->number+two->number;
		 mTree[pos].sign = 0;
		 mTree[pos].connected = false;
		 pos ++;
	 }
	 // if there is no root, pos is even and we take the last node
	 if( !mRoot )
	 {
	  	 mRoot = &mTree[pos-1];
	 }
	 // build hashmaps
 	 buildDictRec( mRoot, 1 );
 	 LOGN("");
}

HuffmanTree *Huffman::findSmallestNode()
{
     int min, mini;
     min = mini = -1;
     for( int i=0; i<mNoNodes; i++ )
     {
	  	  if( mTree[i].number>-1 ) // DIFF
	  	  {
		   	  if( (mTree[i].number<min || min==-1 ) && !mTree[i].connected )
		   	  {
			   	  min = mTree[i].number;
			   	  mini = i;
			  }
		  }
	 }
	 if( mini>-1 )
	 {
	  	 return &mTree[mini];
	 }
	 return NULL;
}

void Huffman::buildDictRec( HuffmanTree *node, int key )
{
 	 if( node==NULL ) return;
 	 if( node->sign>0 )
 	 {
	  	 LOG(node->sign);
	  	 int k = key;
	  	 int k2 = 1;
	  	 while( k>1 )
	  	 {
		  		k2<<=1;
		  		k2+=k&1;
		  		k>>=1;
		 }
		 mDictRev[node->sign] = k2;
		 mDict[key] = node->sign;
		 LOG("["<<key<<", "<<k2<<"]");
     } else {
   		LOG("( ");
   		 buildDictRec( node->left, (key<<1)+1 );
   		LOG(", ");
   		 buildDictRec( node->right, (key<<1) );
  		LOG(" )" );
   	 }
}

void Huffman::countTreeRec( HuffmanTree *node, int base )
{
 	 node->number = base;
 	 
 	 int left = countNodesRec( node->left );
 	 int right = countNodesRec( node->right );
 	 
 	 if( left>1 )
 	 {
	  	 int left_right = countNodesRec( node->left->right );
	  	 countTreeRec( node->left, base - left_right );
	 }
	 if( right>1 )
	 {
	  	 int right_left = countNodesRec( node->right->left );
	  	 countTreeRec( node->right, base + right_left + 1 );
	 }
}

int Huffman::countNodesRec( HuffmanTree *node )
{
 	if( node==0 ) return 0;
 	if( node->left==0 || node->right==0 )
 	{
	 	// a leaf
	 	return 1;
    }
    return countNodesRec( node->left ) + countNodesRec( node->right ) + 1;
}

short *Huffman::walkTreeRec( HuffmanTree *node, short *buffer )
{
    int value = 0;
    if( node->left==0 || node->right==0 )
	{
	 	// leaf
	 	value = -128*256 + node->sign;
	 	*buffer = value;
	 	buffer++;
    } else
    {
 	    value = node->number;
 	    *buffer = value;
 	    buffer++;
 	    buffer = walkTreeRec( node->left, buffer );
 	    buffer = walkTreeRec( node->right, buffer );
    }
    return buffer;
}

};
