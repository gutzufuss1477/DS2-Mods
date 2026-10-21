#include "../src/virtual_graph_core.hpp"
#include <cstdio>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <iterator>
#include <algorithm>
using namespace vgraph;
static std::vector<u8> load(const char* path){
 std::ifstream f(path,std::ios::binary);
 if(!f)throw std::runtime_error("missing file");
 return std::vector<u8>((std::istreambuf_iterator<char>(f)),std::istreambuf_iterator<char>());
}
int main(int argc,char**argv){
 if(argc!=3)return 9;
 auto original=load(argv[1]),actual=load(argv[2]);
 unsigned ok=0,fail=0;
 auto test=[&](bool pass,const char*name){++ok;if(!pass){++fail;std::printf("FAIL %s\n",name);}};
 std::vector<u8> result(PatchedSize);
 test(originalShape(original.data(),unsigned(original.size())),"native structural baseline");
 test(build(original.data(),unsigned(original.size()),result.data(),unsigned(result.size())),"additive graph transformation");
 test(actual==result,"byte-exact validated live graph");
 test(!build(original.data(),OriginalSize,result.data(),OriginalSize),"undersized output refuses");
 auto invalid=original;
 invalid[Group499+20]^=1;
 test(!build(invalid.data(),unsigned(invalid.size()),result.data(),unsigned(result.size())),"wrong parent count refuses");
 invalid=original;invalid[Subgroups]^=1;
 test(!build(invalid.data(),unsigned(invalid.size()),result.data(),unsigned(result.size())),"wrong subgroup count refuses");
 invalid=original;invalid[8]^=1;
 test(!build(invalid.data(),unsigned(invalid.size()),result.data(),unsigned(result.size())),"wrong header refuses");
 invalid=original;invalid[ChildOffset+4]=u8(ChildA);
 set32(invalid.data()+ChildOffset,ChildA);
 test(!build(invalid.data(),unsigned(invalid.size()),result.data(),unsigned(result.size())),"duplicate group refuses");
 test(!build(original.data(),OriginalSize-1u,result.data(),unsigned(result.size())),"short input refuses");
 std::printf("VIRTUAL_GRAPH_TESTS %u ERRORS %u\n",ok,fail);
 return fail?1:0;
}
