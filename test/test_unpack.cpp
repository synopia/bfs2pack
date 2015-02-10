#include "../src/unpack.h"
#include "tut.h"
namespace tut
{

std::string files[] = {
	   "credits.txt",
	   "credits_xbox.txt",
	   "data/drivers/ragdoll",
	   "data/language",
	   "female.bgm",
	   "female_1.dds",
	   "female_2.dds",
	   "female_3.dds",
	   "female_4.dds",
	   "language0.dat",
	   "language1.dat",
	   "language2.dat",
	   "language3.dat",
	   "language4.dat",
	   "language5.dat",
	   "language6.dat",
	   "language7.dat",
	   "languages.bed",
	   "languages.dat",
	   "male.bgm",
	   "male_1.dds",
	   "male_2.dds",
	   "male_3.dds",
	   "male_4.dds",
	   "male_5.dds",
	   "version.ini",
	   ""
	   };
using namespace bfs;

struct unpack_data{
};
typedef test_group<unpack_data> testgroup;
typedef testgroup::object testobject;
testgroup unpack_testgroup("unpack");
template<> template<>		  
void testobject::test<1>()
{
 	 Unpack unpack( "bfs2pack_tests.exe", "." );
 	 
 	 ensure( unpack.test()==false );
 	 unpack.setFilename("fo2c.bfs.org");
 	 ensure( unpack.test()==true );
 	 
 	 unpack.readFilenames();
 	 
 	 int i=0;
 	 do
 	 {
	   	 // only valid for originial bfs
         //ensure( "should be filename "+files[i]+", is "+unpack.getFilenames()[i], unpack.getFilenames()[i]==files[i] );
         i++;
     } while( files[i]!="" );
 	 
 	 cout<<endl<<endl;
 	 for( int i=0;i<unpack.getNumberOfFiles();i++ )
 	 {
	  	 unpack.unpackFile( i );
     }
}
};
