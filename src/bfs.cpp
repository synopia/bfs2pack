/**
file: bfs.cpp
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
#include <direct.h>

#ifdef _MMGR
#include "mmgr.h"
#endif

#include "bfs.h"
#include "log.h"
#include "nlog.h"
namespace bfs{

// API... use this vars from outside
int totalfiles, numfiles;
double file_complete;
int fsize;
string current_file;
bool zip_all;
ACTION action = none; // the current action

void setPgrNewArchive( int total  )
{
 	 totalfiles = total;
 	 fsize = 0;
}
void setPgrNewFile( string file,int num, int size )
{
 	 current_file = file;
	 numfiles = totalfiles - num;
	 fsize = size;
	 file_complete = 0;
}
void setPgrBlock( int pos )
{
 	 file_complete = (double)(pos)/(double)fsize;
}
void setPgrEnd(string text)
{
 	 file_complete = 1.0;
 	 current_file = text;
 	 totalfiles = 1;
	 numfiles = 0;
}

int BfsArchive::getFilenameIndex( string str )
 { 
		   int res = lua_hash(str.c_str(), str.length() ) % 997;
		   LOG(endl<<"getFilenameIndex( "<<str<<" )="<<res<<endl);
		   return res; 
    }


/**
  recursive creates directories
*/
void mkdirs(const char *path )
{
	char dir[256];
	char *curr = dir;
	const char *pcurr = path;
	while( *pcurr!=0 )
	{
		if( *pcurr=='/' || *pcurr=='\\' )
		{
			mkdir( dir );
		} 
		*curr = *pcurr;
		curr++;
		*curr=0;
		pcurr++;
	}
}

// this is the lua hash function... really nice :-)
unsigned int lua_hash(const char*s, int l)
{
    unsigned int h = l;
    int step = (h>>5)+1;
    for( int i = l; i>=step; i-=step )
    {
         h = h ^ ((h<<5)+(h>>2)+(unsigned char)s[i-1]);
    }
    return h;
}


};
