#include "../src/pack.h"
#include "../src/unpack.h"
#include "tut.h"
namespace tut
{

std::string filesp[] = {
	   "data/language/language0.dat",
	   "data/drivers/ragdoll/male_2.dds",
	   "data/language/language5.dat",
	   "data/drivers/ragdoll/female.bgm",
	   ""
	   };
using namespace bfs;

struct pack_data{
};
typedef test_group<pack_data> testgroup;
typedef testgroup::object testobject;
testgroup pack_testgroup("pack");
template<> template<>		  
void testobject::test<1>()
{
 	 Pack pack( "data", "fo2c_new.bfs" );
 	 
 	 pack.readDir();
 	 
 	 list<string> *filenames = pack.getFilesToPack();
 	 list<string>::iterator it;
	 int i=0;
 	 for( it = filenames->begin(); it!=filenames->end()&&filesp[i]!=""; it++ )
 	 {
	  	 ensure( "should be filename "+filesp[i]+", is "+*it, *it==filesp[i] ); 
         i++;
	 }
 	 pack.open();
 	 pack.writeHeader();
 	 pack.writeFilenames();
 	 while( pack.hasNextFile() )
 	 {
 		pack.packFile( true );
	 }
	 pack.finalize();
	 pack.close();
 	 
 	 cout<<"unpacking"<<endl;
 	 Unpack unpack( "fo2c_new.bfs", "new" );
 	 unpack.test();
  	 unpack.readFilenames();
 	 for( int i=0;i<unpack.getNumberOfFiles();i++ )
 	 {
	  	 unpack.unpackFile( i );
     }

}
};
