#include <cstdlib>
#include <iostream>
#include <vector>
#include <list>
#include "arg_parser.h"
#include "pack.h"
#include "unpack.h"
using namespace std;
using namespace bfs;

const char *invocationName = 0;
const char * const ProgramName = "Karoks bfs2pack";
const char * const programName = "bfs2pack";
const char * const programVersion = "1.1 console";
const char * const programDate = __DATE__;

void showHelp( ) throw()
{
 	 printf("%s - Flatout2 bfs archive packer/unpacker\n", ProgramName );
 	 printf("Have fun!\n");
 	 printf("\nUsage: %s <a|x|l> [dir] <foobar.bfs> [options]\n", invocationName );
 	 printf("Modes:\n");
 	 printf("   a					create a new bfs archive `foobar.bfs`\n");
 	 printf("   x					extract bfs archive `foobar.bfs`\n");
 	 printf("   l					list contents of `foobar.bfs`\n");
 	 printf("   [dir]				directory to pack int foobar.bfs\n");
 	 printf("   <foofile.bfs>			bfs archive to pack to / unpack from\n");
 	 printf("Options: \n");
 	 printf("   -h, --help				display this help and exit\n" );
 	 printf("   -V, --version			output version information and exit\n");
 	 printf("   -z, --zip				forces zipping of every file\n");
 	 printf("   -n, --nozip				do not zip any file (DEFAULT)\n");
 	 printf("   -q, --quiet				runs bfs2pack without messages\n");
 	 printf("   -v, --verbose			more messages when running\n");
 	 printf("\nReport bugs to https://www.sourceforge.net/projects/bfs2pack\n");
}

void showVersion() throw ()
{
 	 printf("%s version %s\n", ProgramName, programVersion);
 	 printf("Copyright (C) 2006 Paul Fritsche.\n");
 	 printf("Build date: %s\n\n", programDate);
 	 printf("This program is free software; you may redistribute it under the terms of\n"); 	 
 	 printf("the GNU General Public License. This program has absolutely no warranty.\n"); 	 
}

void showError(const char * msg, const int errorcode=0, const bool help=false) throw()
{
 	 if( msg && msg[0]!=0 )
 	 {
	  	 fprintf( stderr, "%s: %s", programName, msg );
	  	 if( errorcode>0 ) fprintf( stderr, ": %s", strerror( errorcode ) );
	  	 fprintf(stderr, "\n");
     }
     if( help && invocationName && invocationName[0]!=0 )
      	 fprintf(stderr, "Try `%s --help` for more information.\n", invocationName );
}

void internalError( const char *msg )throw()
{
 	 char buf[80];
 	 snprintf( buf, sizeof( buf ), "internal error: %s.\n", msg );
 	 showError( buf );
 	 exit( 3 );
}

const char * optname( const int code, const Arg_parser::Option options[] )throw()
{
 	 static char buf[2]  = "?";
 	 if( code!=0 )
 	   for( int i=0; options[i].code; ++i )
 	   {
	   		if( options[i].name ) return options[i].name; else break;
       }
     if( code>0 && code<256 ) buf[0] = code; else buf[0]='?';
     return buf;
 	   
}
int main(int argc, char *argv[])
{
    int verbose = 1;
    bool zip = false;
    invocationName = argv[0];
    
    static const Arg_parser::Option options[] =
    {
	 	   {'V', "version",			Arg_parser::no },
	 	   {'h', "help",			Arg_parser::no },
	 	   {'z', "zip",				Arg_parser::no },
	 	   {'n', "nozip",			Arg_parser::no },
	 	   {'q', "quiet",			Arg_parser::no },
	 	   {'v', "verbose",			Arg_parser::no },
	 	   {  0,         0,         Arg_parser::no }
    };

	Arg_parser parser( argc, argv, options );
	if( parser.error().size() )
	{
		showError( parser.error().c_str(), 0, true );
		return 1;
	}
	list<string> optNames;
	char action=0;
	for( int i=0; i<parser.arguments(); i++ )
	{
	 	 const int code=parser.code( i );
	 	 if( !code )
	 	 {
		  	 if( action==0 ) action = parser.argument(i).c_str()[0];
		  	 else optNames.push_back( parser.argument(i) );
		 }
		 else
		 {
		  	 switch( code )
		  	 {
	  		     case'V':showVersion();return 0;
	  		     case'h':showHelp();return 0;
	  		     case'v':verbose ++;break;
	  		     case'q':verbose = 0;break;
	  		     case'z':zip=true;break;
	  		     case'n':zip=false;break;
	  		 }
		 }
	}
	if( optNames.size()==1 && (action=='l'||action=='x'))
	{
	 	// UNPACK
		 string DIR = ".";
	     string FILE = optNames.front();
	     optNames.pop_front();
		 Unpack unpack( FILE, DIR );
		 if( !unpack.test() )
		 {
		  	 unpack.close();
		  	 // error
		 } else{
		 
			 unpack.readFilenames();
			 if( action=='x' ){
			 	 for( int i=0;i<unpack.getNumberOfFiles();i++ )
			 	 {
				  	 unpack.unpackFile( i, verbose );
			     }
			 } else
			 {
			 	 for( int i=0;i<unpack.getNumberOfFiles();i++ )
			 	 {
				  	 FileEntry *fe = unpack.getFileEntry( i );
				  	 if( verbose )
				  	 {
					 	 cout<<unpack.getFilenames()[fe->dir]<<"/"<<unpack.getFilenames()[fe->file];
				  	     cout<<" size: "<<fe->unpacked;
				  	     if( verbose>1 )
				  	     {
						  	 if( fe->type==5 ) cout<<" packed: "<<fe->packed;
						  	 else cout<<" unpacked ";
						 }
						 cout<<endl;
					 }
		  	 		 
			     }
	         }
		 }		
	 	
	} else if( optNames.size()==2 && action=='a')
	{
	   // PACK
 	   	 string DIR = optNames.front();
 	   	 optNames.pop_front();
	     string FILE = optNames.front();
 	   	 optNames.pop_front();
	     
	     Pack pack( DIR, FILE );
	     action = packing;
	     pack.readDir();
	     pack.open();
	     pack.writeHeader();
	     pack.writeFilenames();
	 	 while( pack.hasNextFile() )
	 	 {
	 		pack.packFile( zip, verbose );
		 }
		 pack.finalize();
    } else
	{
	 	showError("You must specify either one bfs archive or a directory and a bfs.",0,true);
	 	return 1;
	}
    
    return EXIT_SUCCESS;
}
