#include <ffgl/FFGL.h>
#include <dlfcn.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
void check(bool ok,const char* why) { if(!ok) throw std::runtime_error(why); }
int main(int argc,char** argv) try {
    check(argc==2,"bundle executable path required");
    void* library=dlopen(argv[1],RTLD_NOW|RTLD_LOCAL);
    if(!library) throw std::runtime_error(dlerror());
    using Entry=FFMixed(*)(FFUInt32,FFMixed,FFInstanceID);
    auto entry=reinterpret_cast<Entry>(dlsym(library,"plugMain"));
    check(entry!=nullptr,"missing plugMain");
    check(entry(FF_INITIALISE_V2,{},nullptr).UIntValue==FF_SUCCESS,"initialise");
    auto info=static_cast<PluginInfoStruct*>(entry(FF_GET_INFO,{},nullptr).PointerValue);
    check(info && std::strncmp(info->PluginName,"PulseSweep",16)==0,"plugin name");
    check(std::memcmp(info->PluginUniqueID,"MWA1",4)==0,"plugin ID");
    check(entry(FF_GET_NUM_PARAMETERS,{},nullptr).UIntValue==13,"parameter count");
    const unsigned types[]={FF_TYPE_HUE,FF_TYPE_SATURATION,FF_TYPE_BRIGHTNESS,FF_TYPE_ALPHA};
    for(unsigned i=0;i<4;i++) {
        FFMixed index{};index.UIntValue=6+i;
        check(entry(FF_GET_PARAMETER_TYPE,index,nullptr).UIntValue==types[i],"color picker types");
    }
    check(entry(FF_DEINITIALISE,{},nullptr).UIntValue==FF_SUCCESS,"deinitialise");
    dlclose(library);
    std::cout<<"PASS: bundle load, FFGL identity and color parameter metadata (native architecture only).\n";
    std::cout<<"Rendering still needs validation in Resolume.\n";
    return 0;
} catch(const std::exception& e) { std::cerr<<e.what()<<'\n';return 1; }
