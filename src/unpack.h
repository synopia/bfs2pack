/**
file: unpack.h
the bfs unpacking algorithm


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
#include <iostream>
#include <fstream>

#include "bfs.h"
#include "huffman.h"

namespace bfs{

using namespace std;

class Unpack : public BfsArchive {
public:
	   Unpack( string filename, string dir );
	   virtual ~Unpack();

	   void setDir( string dir){mDir = dir; }
	   void setFilename(string fn ){mFilename = fn;}

	   bool test();
	   bool open();
	   void close();
	   
	   bool readHeader();
	   void readFilenames();
	   
	   int unpackFile( int no, int verbose );
	  
	   string *getFilenames(){ return mFilenames; }
	   int getNumberOfFiles() { return mHeader.no_of_files; }
	   FileEntry *getFileEntry( int no ){ return &(mFileEntries[no]); }
private:
	   string mFilename;
	   string mDir; 	
	   ifstream	mInput;
	   Header mHeader;
	   bool mHeaderRead;
	   FileEntry *mFileEntries;
	   int mNoFilenames;
	   int *mEntries;
	   string *mFilenames;
	   Huffman mHuffman;
		
  	   char *buf;
	   char *inf;
		
};
		  
		  
};
