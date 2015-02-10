/**
file: pack.h
the bfs packing algorithm


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


class Pack: public BfsArchive {
public:
	   Pack( string dir, string filename );
	   virtual ~Pack();
	   
	   void setDir( string dir){mDir = dir; }
	   void setFilename(string fn ){mFilename = fn;}

	   bool open();
	   void close();

	   void readDir();
	   
	   void writeHeader();
	   void writeFilenames();
	   void finalize();
	   int packFile( bool zip, int verbose );
	   
       bool hasNextFile();
	   int getNumberOfFiles() { return mHeader.no_of_files; }
	   list<string>::iterator getCurrentFile() { return mFileIt; }
	   list<string> *getFilesToPack(){ return &mFilesToPack; }
	   list<string> *getFilenames() {return mHuffman->getStrings(); }
private:
	   static string fixDataPath(string file);
	   void listDir(string path);
	   static bool sortByHash(const string &a, const string &b );
	   	
	   string mFilename;
	   string mDir; 	
	   
	   list<string> mFilesToPack;
	   list<string>::iterator mFileIt;
	   static map<string, int> mFilenamePositions;
	   
	   ofstream	mOutput;
	   Header mHeader;

	   int mPos;
	   int mEntriesPos;
	   int mFileEntriesPos;
	   int *mEntries;
	   FileEntry *mFileEntries;
	   
	   int mNoFilenames;
	   int mLastFei;
	   
	   Huffman* mHuffman;
	   
	   char *buf;
   	   char *def;	   
		
};
		  
		  
};
