#include"../src/huffman.h"
#include"tut.h"
namespace tut{

using namespace bfs;

struct huffman_data{
};
typedef test_group<huffman_data> testgroup;
typedef testgroup::object testobject;
testgroup huffman_testgroup("huffman");
template<> template<>		  
void testobject::test<1>()
{
	Huffman huffman;
 	ensure("no strings in", huffman.getStrings()->size()==0 );
 	string str("Hello World!");
 	huffman.addString(str);
 	ensure("added 1 string", huffman.getStrings()->size()==1 );
 	huffman.finalize();
 	ensure("dict should contain something", huffman.getDict()->size()>0 );
 	ensure("dict should contain something", huffman.getDictRev()->size()>0 );
 	char buf[300];
 	int ul = str.length();
 	int pl = huffman.encryptString(str,buf);
 	string s = huffman.decryptString(buf, pl, ul);
 	ensure("decrypt(encrypt(s))==s", s==str);
 	
 	short serializee[300];
 	int size = huffman.serialize( serializee );
 	ensure("serializee should contain something", size>0 );
 	
 	Huffman h2;
 	h2.deserialize(serializee, size, 1 );
 	s = h2.decryptString(buf, pl, ul);

 	ensure("decrypt(encrypt(s))==s", s==str);
}


};
