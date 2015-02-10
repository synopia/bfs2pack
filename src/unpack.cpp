/**
file: unpack.cpp
implementation of the bfs unpacking algorithm


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
#include <zlib.h>
#include "unpack.h"
#include "log.h"

#ifdef _MMGR
#include "mmgr.h"
#endif

#include "nlog.h"
namespace bfs{

using namespace std;

Unpack::Unpack( string filename, string dir )
{
    mFilename = filename;
    mDir = dir;
    
    mFileEntries = NULL;
    mEntries = NULL;
    mFilenames = NULL;
    
    mHeaderRead = false;
    
    buf = new char[BUFSIZE];
    inf = new char[BUFSIZE];
    
}

Unpack::~Unpack()
{
    close();
    
    delete buf;
    delete inf;
}

void Unpack::readFilenames()
{
 	FilenameHeader fnHead;
 	
 	mInput.read( (char*) &fnHead, sizeof( fnHead ) );
 	int noTable = (fnHead.offset_size - fnHead.offset_table ) / 4;
 	int huffmanFileOffset[noTable+1];
 	mInput.read( (char*) huffmanFileOffset, noTable * 4 );
 	huffmanFileOffset[noTable] = fnHead.length - fnHead.offset_huffman_code;
 	int noSize = (fnHead.offset_huffman_tree - fnHead.offset_size ) / 2;
 	short sizes[noSize];
 	mInput.read( (char*) sizes, sizeof( sizes ) );
 	int noTree = (fnHead.offset_huffman_code - fnHead.offset_huffman_tree );
 	short tree[noTree/2];
 	mInput.read( (char*) tree, sizeof( tree ) );
 	mHuffman.deserialize( tree, noTree, 1 );
 	
 	mNoFilenames = noTable;
 	mFilenames = new string[mNoFilenames];
 	char buf[MAX_PATH];
 	for( int i=1; i<noTable+1; i++ )
	{
	 	 int packsize = huffmanFileOffset[i] - huffmanFileOffset[i-1];
	 	 int unpacksize = sizes[i-1];
	 	 mInput.read( buf,  packsize );
		 mFilenames[i-1] = mHuffman.decryptString( buf, packsize, unpacksize );
    }
 
    mFileEntries = new FileEntry[mHeader.no_of_files];
    mInput.read( (char*) mFileEntries, sizeof( FileEntry ) * mHeader.no_of_files );
}

int Unpack::unpackFile( int no, int verbose )
{
 	int wrote = 0;
 	if( no<0 || no>= mHeader.no_of_files ) return -1;
 	FileEntry e = mFileEntries[no];
 	LOG("@"<<e.offset<<":");
 	mInput.seekg( e.offset, ios::beg );
 	
 	// prepare directory;
 	string dir=mDir+"/"+mFilenames[e.dir] + "/";
 	mkdirs( dir.c_str() );
 	
 	string filename = dir+mFilenames[e.file];
 	if( verbose ){
        cout<<"["<<(int)((double)(numfiles+1)*100/(double)totalfiles)<<"%]";
	 	cout << filename << " inflating ";}
 	setPgrNewFile( mFilenames[e.file], mHeader.no_of_files-no, e.unpacked );
 	
 	// open output file 
 	ofstream outf( filename.c_str(), ios::binary|ios::trunc );
 	
 	// init crc32
 	unsigned int chk = crc32( 0, Z_NULL, 0 );
 	z_stream strm;
 	
 	if( e.type == 5 )
 	{
	 	strm.zalloc		= Z_NULL;
		strm.zfree		= Z_NULL;
		strm.opaque		= Z_NULL;
		strm.avail_in	= 0;
		strm.next_in	= Z_NULL;
		int res = inflateInit(&strm);
		if( res!=Z_OK )
		{
		 	if( verbose )
					 	cout << "error initializing zlib!!"<<endl;
			return -2;
		}
	}
	
	int bytesToRead = e.packed;
	LOG("ps: "<<e.packed);
	int steps = bytesToRead / BUFSIZE;
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
         int currBytes = 0;
         if( bytesToRead>BUFSIZE )
         {
		  	 currBytes = BUFSIZE;
		 } else
		 {
             currBytes = bytesToRead; 
		 }
		 mInput.read( (char*) buf, currBytes );
		 LOG(" read: "<<currBytes);
		 
		 if( e.type==5 )
		 {
		  	 strm.avail_in = currBytes;
		  	 strm.next_in = (Bytef*)buf;
		  	 do{
 			     strm.avail_out = BUFSIZE;
			  	 strm.next_out = (Bytef*)inf;
			  	 
			  	 int ret = inflate( &strm, Z_NO_FLUSH );
			  	 LOG("["<<ret<<"]");
			  	 int have = BUFSIZE - strm.avail_out;
			  	 chk = crc32( chk, (Bytef*)inf, have );
			  	 LOG(" write: "<<have);
			  	 outf.write( (char*) inf, have );
			  	 wrote += have;
		     } while ( strm.avail_out==0 );
		 } else
		 {
//		   	 chk = crc32( chk, buf, currBytes );
		   	 LOG(" write: "<<currBytes);
			 outf.write( (char*)buf, currBytes );
			 wrote+=currBytes;	
		 }	  
		 setPgrBlock( wrote );
		 bytesToRead-=currBytes;
    }
    LOG("wrote: "<<wrote);
    
    if( e.type==5 )
    {
	 	inflateEnd( &strm );
    }
    if( ~chk==e.unknown )
    {
	 	if( verbose)
   	 	   cout<<"ok";
    } else
	 	if( verbose)
           cout<<"crc error";
           
    if( verbose>1 ){
       cout<<" read: "<<e.packed<<" wrote: "<<e.unpacked;
    }
    if( verbose )
       cout<<endl;	 
    outf.close();
    
    return wrote;
}

bool Unpack::readHeader()
{
 	 if( !mHeaderRead )
 	 {
	  	 mInput.read( (char*) &mHeader, sizeof( mHeader ) );
	  	 mHeader.data_offset -= 2;
	  	 mEntries = new int[mHeader.entries * 2];
	  	 mInput.read( (char*) mEntries, mHeader.entries * 2 * 4 );
	  	 mHeaderRead = true;
		 setPgrNewArchive( mHeader.no_of_files 	 );
	 }
	 return strncmp( mHeader.bfs1, "bfs1", 4 )==0;
}

bool Unpack::test()
{
 	if( mInput.is_open() )
 	{
	 	close();
    }
    if( open() )
    {
	    return readHeader();
	}
	return false;
}


bool Unpack::open()
{
 	mInput.open( mFilename.c_str(), ios::binary ); 
 	if( mInput.is_open() )
 	{
	 	return readHeader();
	}
	return false;
}

void Unpack::close()
{
 	if( mInput.is_open() ) mInput.close(); 
    if( mFileEntries ) delete[] mFileEntries;
    if( mEntries ) delete[] mEntries;
    if( mFilenames ) delete[] mFilenames;

    mFileEntries = NULL;
    mEntries = NULL;
    mFilenames = NULL;
    
    mHeaderRead = false;


}

};
