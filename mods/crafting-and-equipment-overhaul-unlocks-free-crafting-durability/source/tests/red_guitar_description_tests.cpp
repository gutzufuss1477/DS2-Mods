#include "../src/weapons/core.hpp"
#include <cstdio>
#include <cstring>
#include <initializer_list>
int main(){
 const auto& guitar=silenced::Definitions[silenced::GuitarVariant];
 unsigned passed=0,failed=0;
 auto check=[&](bool good,const char*label){if(good)++passed;else{++failed;std::printf("FAIL %s\n",label);}};
 check(std::strcmp(guitar.nameDe,"Rote Kampfgitarre (Higgs)")==0,"German name unchanged");
 check(std::strcmp(guitar.nameEn,"Red Battle Guitar (Higgs)")==0,"English name unchanged");
 check(std::strstr(guitar.descriptionDe,"Higgs' rote Kampfgitarre")!=nullptr,"German Higgs text");
 check(std::strstr(guitar.descriptionDe,"Axtmodus")!=nullptr,"German gameplay description");
 check(std::strstr(guitar.descriptionEn,"Higgs' red Battle Guitar")!=nullptr,"English Higgs text");
 check(std::strstr(guitar.descriptionEn,"axe mode")!=nullptr,"English gameplay description");
 for(const char* p:{guitar.descriptionDe,guitar.descriptionEn}){
  check(std::strstr(p,"Testversion")==nullptr,"no Testversion");
  check(std::strstr(p,"Experimental")==nullptr,"no Experimental");
  check(std::strstr(p,"Doppelhals")==nullptr,"no incorrect model description");
  check(std::strstr(p,"prototype")==nullptr,"no prototype");
 }
 check(guitar.id==305 && guitar.source==184 && silenced::craftable(silenced::GuitarVariant),"unchanged weapon identity");
 std::printf("GUITAR_DESCRIPTION_ASSERTIONS %u ERRORS %u\n",passed+failed,failed);
 return failed?1:0;
}
