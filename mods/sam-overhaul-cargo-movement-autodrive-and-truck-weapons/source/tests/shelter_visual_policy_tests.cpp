#include <cstdio>
#include "../src/shelter_visual_policy.h"

static int tested=0,failed=0;
static void expect(bool actual,const char* label){
 ++tested;
 if(!actual)++failed;
 std::printf("%s %s\n",actual?"PASS":"FAIL",label);
}
int main(){
 using sam_shelter_visual::renderer_diameter;
 using sam_shelter_visual::accept_renderer_correction;
 const float radius8=8.0f;
 expect(renderer_diameter(radius8)==16.0f,"200-percent protection circle = 8m radius = 16m renderer diameter");
 expect(accept_renderer_correction(4.0f,radius8),"vanilla 4m source can correct");
 expect(accept_renderer_correction(8.0f,radius8),"native 8m source can correct");
 expect(accept_renderer_correction(16.0f,radius8),"correct 16m renderer remains untouched");
 expect(accept_renderer_correction(32.0f,radius8),"known native double-size 32m renderer can correct");
 expect(!accept_renderer_correction(24.0f,radius8),"unknown 24m unrelated visual denied");
 expect(!accept_renderer_correction(31.0f,radius8),"unknown near but not 32m denied");
 expect(!accept_renderer_correction(64.0f,radius8),"quadruple magnitude denied");
 expect(!accept_renderer_correction(-1.0f,radius8),"negative geometry denied");
 expect(!accept_renderer_correction(16.0f,3.0f),"unsupported low target radius denied");
 expect(!accept_renderer_correction(16.0f,17.0f),"unsupported high target radius denied");
 expect(renderer_diameter(4.0f)==8.0f,"native vanilla 4m radius converts to 8m diameter");
 expect(renderer_diameter(8.6f)>17.19f &&
        renderer_diameter(8.6f)<17.21f,
        "coating 8.6m is independent of 8m protected/circle renderer");
 std::printf("SHELTER_VISUAL_DIAMETER_POLICY %s (%d checks)\n",
             failed?"FAILED":"PASSED",tested);
 return failed?1:0;
}
