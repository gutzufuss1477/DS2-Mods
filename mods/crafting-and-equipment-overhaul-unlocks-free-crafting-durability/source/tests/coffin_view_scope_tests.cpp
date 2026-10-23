#include "../src/coffin_view_scope.hpp"
#include <cstdio>
int main(){
 unsigned tests=0,errors=0;
 for(int scene=0;scene<2;++scene){
  for(int story=0;story<2;++story){
   for(int armed=0;armed<2;++armed){
    ++tests;
    if(coffin_view_scope::permitStoryEntityForBaggageView(scene,story,armed))++errors;
   }
  }
 }
 std::printf("COFFIN_NATIVE_BAGGAGE_VIEW_PRESERVED_ASSERTIONS %u ERRORS %u\n",tests,errors);
 return errors?1:0;
}
