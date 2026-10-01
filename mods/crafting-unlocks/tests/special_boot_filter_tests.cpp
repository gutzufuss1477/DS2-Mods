#include <cstdio>
#include <cstring>
#include <stdexcept>
extern "C" int TestSpecialBootFilter(const unsigned char*, unsigned int);
static unsigned checks=0;
static void check(bool ok,const char*why){++checks;if(!ok)throw std::runtime_error(why);}
int main(){try{
 unsigned char resource[0xB0]{};
 const unsigned allowed[]={0x75D99124u,0x07B21227u,0x6C3A478Bu,0x1EA3AF0Bu,0x6CC82C08u,0x1E51C488u,0x380248E3u,0x4A69CBE0u,0x38F02360u,0x14889C47u,0x4B7F7765u,0x3914F466u,0x0CE5E07Au};
 for(unsigned key:allowed){
  std::memcpy(resource+0x20,&key,4);
  check(TestSpecialBootFilter(resource,0)==1,"allowlisted Usage=None recipe must pass");
  check(TestSpecialBootFilter(resource,1)==1,"normal usage must still pass");
 }
 unsigned key=0x64CE664Eu;std::memcpy(resource+0x20,&key,4);
 check(TestSpecialBootFilter(resource,0)==0,"alternate Ghost Blade entry must remain filtered");
 key=0x66E31F44u;std::memcpy(resource+0x20,&key,4);
 check(TestSpecialBootFilter(resource,0)==0,"Heavy MG must remain filtered from final release");
 key=0x17B359C8u;std::memcpy(resource+0x20,&key,4);
 check(TestSpecialBootFilter(resource,0)==0,"alternate Heavy MG entry must remain filtered");
 key=0x12345678u;std::memcpy(resource+0x20,&key,4);
 check(TestSpecialBootFilter(resource,0)==0,"other Usage=None must remain filtered");
 check(TestSpecialBootFilter(resource,9)==1,"other normal usage must pass");
 std::printf("PASS %u targeted Usage=None UI filter cases.\n",checks);return 0;
 }catch(const std::exception&e){std::fprintf(stderr,"FAIL after %u: %s\n",checks,e.what());return 1;}}
