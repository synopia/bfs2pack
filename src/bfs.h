/**
file: bfs.h
you find some general usefull stuff here


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
#include <iostream>

namespace bfs{

using namespace std;

// what we are currently doing? packing, unpacking or nothing
enum ACTION{
     none, packing,unpacking, reading
};

extern ACTION action; // the current action
		  
// API... use this vars from outside
extern int totalfiles, numfiles;
extern double file_complete;
extern string current_file;
extern bool zip_all;

void setPgrNewArchive( int total  );
void setPgrNewFile( string file, int num, int size );
void setPgrBlock( int pos );
void setPgrEnd(string text);

const int BUFSIZE = 4096*4;

// structures
struct Header{
	char bfs1[4];	// "bfs1"
	short unknown;
	short unknown2;
	int data_offset;	// where the file data starts
	int no_of_files;	// number of files included in this bfs
	int entries;		// number of file-entries to read (those entries are quite useless..)
};
struct FilenameHeader{
	int length;		// length of filename buf
	int offset_table;	// where the offsettable starts
	int offset_size;	// where the sizes starts
	int offset_huffman_tree;	// where the huffman tree starts
	int offset_huffman_code;	// where the huffman code starts
};
struct FileEntry{
	int type;		// 4.. unpacked, 5..deflated
	int offset;		// fileoffset
	int unpacked;	// unpacked size
	int packed;		// packed size
	unsigned int unknown;	// ~crc32
	short dir;		// id for dir
	short file;		// id for file
};

// tool functions
void mkdirs(const char *path );
unsigned int lua_hash(const char*s, int l);


class BfsArchive{
protected:
    static int getFilenameIndex( string str );
};

};
