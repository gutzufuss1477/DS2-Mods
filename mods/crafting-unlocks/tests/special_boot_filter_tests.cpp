#include <cstdio>
#include <cstring>
#include <stdexcept>
extern "C" int TestSpecialBootFilter(const unsigned char*, unsigned int);
static unsigned checks=0;
static void check(bool ok,const char*why){++checks;if(!ok)throw std::runtime_error(why);}
int main(){try{
 unsigned char resource[0xB0]{}; unsigned key=0;
 key=0x75D99124u; std::memcpy(resource+0x20,&key,4);
 check(TestSpecialBootFilter(resource,0)==1,"special Usage=None must pass");
 check(TestSpecialBootFilter(resource,1)==1,"special normal usage must pass");
 key=0x12345678u; std::memcpy(resource+0x20,&key,4);
 check(TestSpecialBootFilter(resource,0)==0,"other Usage=None must remain filtered");
 check(TestSpecialBootFilter(resource,9)==1,"other normal usage must pass");
 std::printf("PASS %u special-boot UI filter cases.\n",checks); return 0;
 }catch(const std::exception&e){std::fprintf(stderr,"FAIL after %u: %s\n",checks,e.what());return 1;}}
