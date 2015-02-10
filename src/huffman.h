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
#pragma once

#include <string>
#include <list>
#include <map>

namespace bfs{

using namespace std;

class HuffmanTree{
public:
       HuffmanTree *left;
       HuffmanTree *right;
       int number;
       char sign;
       bool connected;
};

class Huffman 
{
public:
     Huffman();
     virtual ~Huffman();
     
     void reset();
     
     int serialize(short *buffer);
     short* deserialize( short *buffer, int bytes, int key );
     
     void addString( string str ) { mStrings.push_back( str ); }
     list<string> *getStrings(){ return &mStrings; }
     void finalize();
     
     string decryptString( char *buf, int packsize, int unpacksize );
     int encryptString( string str, char *buf );
     
     map< int, char> *getDict(){ return &mDict; }
     map< char, int> *getDictRev(){ return &mDictRev; }
protected:
     list<string> mStrings;
     map< int, char> mDict;
     map< char, int> mDictRev;
     
     int mNoNodes;
     HuffmanTree *mTree; // full tree list
     HuffmanTree *mRoot; // the root node
     
     int mCharacters[255];
     
     void buildDictRec( HuffmanTree *node, int key );
     HuffmanTree *findSmallestNode();
     
     void countTreeRec( HuffmanTree *node, int base );
     int countNodesRec( HuffmanTree *node );
     short *walkTreeRec( HuffmanTree *node, short *buffer );
};

};
