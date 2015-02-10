#include"tut.h"

int add( int i, int j ) { return i+j; }

namespace tut{

struct shared_ptr_data{};
typedef test_group<shared_ptr_data> testgroup;
typedef testgroup::object testobject;
testgroup shared_ptr_testgroup("shared ptr");
template<> template<>		  
void testobject::test<1>()
{
 	ensure( "must be 4", add(2,2)==4 );
}


};

