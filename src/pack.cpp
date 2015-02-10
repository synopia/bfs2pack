/**
file: pack.cpp
implementation of the bfs packing algorithm


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
#include <algorithm>
#include <dir.h>
#include <zlib.h>
#ifdef _MMGR
#include "mmgr.h"
#endif

#include "log.h"
#include "pack.h"

#include "nlog.h"
namespace bfs{

using namespace std;
map<string, int> Pack::mFilenamePositions;

Pack::Pack(string dir, string filename)
{
    mDir = dir;
    mFilename = filename;
    mEntries = NULL;
    mFileEntries = NULL;
    memset( &mHeader, 0, sizeof( mHeader ) );
    mFileIt = 0;
    
    buf = new char[BUFSIZE];
    def = new char[BUFSIZE];
    mHuffman = new Huffman();
}

Pack::~Pack()
{
    close(); 
    
    delete buf;
    delete def;    
    delete mHuffman;
}

void Pack::close()
{
 	mFilesToPack.clear();
    mHuffman->reset();
 	if( mOutput.is_open() ) mOutput.close(); 
    if( mEntries ) delete[] mEntries;
    if( mFileEntries ) delete[] mFileEntries;
}


bool Pack::hasNextFile()
{
 	return mFileIt!= mFilesToPack.end();
}

int Pack::packFile( bool zip, int verbose )
{
 	z_stream strm;
 	unsigned int chk;
 	
 	ifstream in;
 	in.open( mFileIt->c_str(), ios::binary );
 	in.seekg( 0, ios::end );
 	int length = in.tellg();
 	in.seekg( 0, ios::beg );
 	if( length==0 ) zip = false;
 	if( verbose )
 	{
        cout<<"["<<(int)((double)(numfiles+1)*100/(double)totalfiles)<<"%]";
    	cout<<*mFileIt<<" ";
    }
 	string fn = *mFileIt;

 	setPgrNewFile( fn, mFilesToPack.size()-mPos, length );

 	int pos = fn.find_last_of("/");
 	string file = fn.substr(pos+1, fn.length()-pos-1 );
 	string dir = fn.substr(0,pos);
 	dir = fixDataPath( dir );

 	if( verbose )
	 	if( zip )
	 		cout<<"deflating ";
	    else
    	cout<<"copying ";
    	
   	int bytesToRead = length;
   	int fepos = mOutput.tellp();
   	
   	if( zip )
   	{
	 	strm.zalloc 		= Z_NULL;
	 	strm.zfree			= Z_NULL;
	 	strm.opaque			= Z_NULL;
	 	deflateInit( &strm, 9 );
	}
	int flush;
	int psize = 0;
	chk = crc32( 0, Z_NULL, 0 );
	int steps = length/BUFSIZE;
	int step = 0;
	
	while( bytesToRead>0 )
	{
        if( step==0 )
        {
 		 	if( verbose )
		    	cout<<".";
		 	step = steps;
		}
		step--;
		int toread = 0;
		if( bytesToRead>BUFSIZE )
		{
		 	toread = BUFSIZE;
			flush= Z_NO_FLUSH;
		} else
		{
		  	toread = bytesToRead;
			flush = Z_FINISH;  	 	
		}
		setPgrBlock( length-bytesToRead );
		
		in.read( buf, toread );
		chk = crc32( chk, (Bytef*) buf, toread );
		
		if( zip )
		{
		 	strm.avail_in = toread;
		 	strm.next_in = (Bytef*) buf;
		 	do{
			    strm.avail_out = BUFSIZE;
			    strm.next_out  = (Bytef*)def;
			    deflate( &strm, flush );
			    int have = BUFSIZE - strm.avail_out;
			    psize+=have;
			    mOutput.write( def, have );
			} while( strm.avail_out==0 );
		} else
		{
		  	mOutput.write( buf, toread );
			psize+=toread;
		}
		bytesToRead -= toread;
	}
	in.close();
	
 	if( verbose )
		if( length>0 )
 			cout<<"done ("<<100*psize/length<<"%)";
		else
   	        cout<<"done (-)";

    if( verbose>1 )
		cout<<" read: "<<length<<" wrote: "<<psize; 
 	if( verbose )
        cout<<endl; 
	if( zip )
	  	deflateEnd( &strm );
  	
  	pos = mPos;
  	int fei = getFilenameIndex( dir+"/"+file );
  	LOG(fei);
  	if( mLastFei!=fei )
  	{
	 	mEntries[fei*2+0] = mFileEntriesPos + pos*sizeof( FileEntry );
	 	mEntries[fei*2+1] = 1;
	} else {
	  	mEntries[fei*2+1]++;
	}
	mLastFei = fei;
	
	mFileEntries[pos].offset = fepos;
	mFileEntries[pos].packed = psize;
	mFileEntries[pos].unpacked = length;
	mFileEntries[pos].type = zip?5:4;
	mFileEntries[pos].dir = mFilenamePositions[dir];
	mFileEntries[pos].file = mFilenamePositions[ file ];
	mFileEntries[pos].unknown = ~chk;
	
	mPos++;
	if( hasNextFile() )
		mFileIt++;
}

void Pack::finalize()
{
 	mOutput.seekp( mFileEntriesPos, ios::beg );
 	mOutput.write( (char*)mFileEntries, sizeof( FileEntry ) * mHeader.no_of_files );
 	mOutput.seekp( mEntriesPos, ios::beg );
 	mOutput.write( (char*)mEntries, mHeader.entries * 2 * 4 );
 	mOutput.seekp( 0, ios::beg );
 	mOutput.write( (char*)&mHeader, sizeof( mHeader ) );
 	LOG(endl<<mHeader.data_offset<<endl);
}

bool Pack::open()
{
 	mOutput.open( mFilename.c_str(), ofstream::binary | ofstream::trunc ); 
}

void Pack::writeHeader()
{
 	strncpy( mHeader.bfs1, "bfs1", 4 );
 	mHeader.no_of_files = mFilesToPack.size();
 	mHeader.entries = 997;
 	mHeader.data_offset = 0;
 	mHeader.unknown = 0;
 	
 	mOutput.write( (char*) &mHeader, sizeof( mHeader ) );
 	mEntries = new int[mHeader.entries*2];
 	memset( mEntries, 0, mHeader.entries * 2 * 4 );
 	mEntriesPos = mOutput.tellp();
 	mOutput.write( (char*) mEntries, mHeader.entries * 2 * 4 );
}

void Pack::writeFilenames()
{
 	mHuffman->finalize(); 
 	short buf[500];
 	int treeSize = mHuffman->serialize(buf);
 	char crypted[500000], *c = crypted;
 	int nofn = mHuffman->getStrings()->size();
 	short sizes[nofn];
 	int offsets[nofn+1];
 	offsets[0] = 0;
 	list<string>::iterator it;
 	int i=0;
 	mFilenamePositions.clear();
 	for( i=0,it=mHuffman->getStrings()->begin(); it!=mHuffman->getStrings()->end(); i++,it++ )
 	{
	   string fn = *it;
	   mFilenamePositions[fn] = i;
	   int cs = mHuffman->encryptString( fn , c );
	   sizes[i] = fn.length();
	   c+=cs;
	   offsets[i+1] = offsets[i]+cs;
	}
	
	FilenameHeader fnHead;
	fnHead.offset_table = sizeof( fnHead );
	fnHead.offset_size = fnHead.offset_table + nofn * 4 ;
	fnHead.offset_huffman_tree = fnHead.offset_size + nofn*2;
	fnHead.offset_huffman_code = fnHead.offset_huffman_tree + treeSize*2;
	fnHead.length = fnHead.offset_huffman_code + offsets[nofn];
	mOutput.write( (char*)&fnHead, sizeof( fnHead ) );
	mOutput.write( (char*)offsets, nofn*4 );
	mOutput.write( (char*)sizes, nofn*2 );
	mOutput.write( (char*)buf, treeSize*2 );
	mOutput.write( crypted, c-crypted );
	
	mFileEntries = new FileEntry[mHeader.no_of_files];
	mFileEntriesPos = mOutput.tellp();
	mOutput.write( (char*)mFileEntries, sizeof( FileEntry ) * mHeader.no_of_files );
	
	mHeader.data_offset = mOutput.tellp();
}

char counter = 0;
void Pack::readDir()
{
 	counter=0; 
	setPgrNewArchive( 1 );
    setPgrNewFile("reading tree", 0, 255 );
    listDir( mDir );
    
    mFilesToPack.sort( this->sortByHash );
    mFileIt = mFilesToPack.begin();
    mPos = 0;
	setPgrNewArchive( mFilesToPack.size()  );
}

bool Pack::sortByHash(const string &a, const string &b )
{
 	 string fa = fixDataPath( a );
 	 string fb = fixDataPath( b );
 	 int feia = getFilenameIndex(fa);
 	 int feib = getFilenameIndex(fb);
 	 if( feia==feib ) 
 	 {
	  	 return fa<fb;
     }
     return feia<feib;
}

string Pack::fixDataPath( string path )
{
    if( path.substr(0,5)!="data/" )
    {
	 	int pos = path.find("/",0);
	 	path = "data"+path.erase(0,pos);
    }
    return path;
}
	   
void Pack::listDir( string path )
{
    setPgrBlock( counter++ );
 	struct _finddata_t ffblk;
	int handle, done;
	done = 1;
	handle = _findfirst( (path+"\\*").c_str(), &ffblk );
	string dataDir = fixDataPath( path );
	LOG(path<<"\\*  "<<dataDir<<endl);
	while( done!=-1 )
	{
       long a;
       a = ffblk.attrib;
       string f( ffblk.name );
       if( f!="." && f!=".." )
       {
	   	   string fn = path+"/"+f;
	   	   if( (a&_A_SUBDIR)==_A_SUBDIR )
	   	   {
		   	   mHuffman->addString( dataDir+"/"+f );
		   	   listDir( fn );
           } else
           {
		   	   mHuffman->addString( f );
			   mFilesToPack.push_back( fn );
           }
       }
       done = _findnext( handle, &ffblk );
    } 
}


	   
};
