#include "../src/WaveEngine.h"
#include <cassert>
#include <iostream>
using namespace mertwave;
int main() {
    auto close=[](float a,float b){return std::abs(a-b)<1e-5f;};
    auto red=rgb(0,1,1),green=rgb(1.f/3,1,1),blue=rgb(2.f/3,1,1),white=rgb(.4f,0,1),gray=rgb(.9f,0,.5f);
    assert(close(red[0],1)&&close(red[1],0)&&close(red[2],0));
    assert(close(green[0],0)&&close(green[1],1)&&close(green[2],0));
    assert(close(blue[0],0)&&close(blue[1],0)&&close(blue[2],1));
    for(int i=0;i<3;i++){assert(close(white[i],1));assert(close(gray[i],.5f));}
    assert(close(rgb(1,1,1)[0],1));
    Engine e;auto p=defaults();p[Speed]=0;
    e.trigger(10,p);p[Mode]=2;e.trigger(10.5,p);
    assert(e.waves.size()==2 && e.waves[0].p[Mode]==1 && e.waves[1].p[Mode]==2);
    e.prune(14);assert(e.waves.size()==1);
    e.prune(14.5);assert(e.waves.empty());
    assert(std::abs(duration(1)-.25)<1e-6 && duration(0)==4);
    for(int i=0;i<100;i++)e.trigger(20+i*.001,p);
    assert(e.waves.size()==32 && e.waves.front().born==20+.068);
    e.prune(0);assert(e.waves.empty());
    std::cout<<"PASS: overlapping waves, parameter snapshots, expiry, speed range, capacity, clock reset\n";
}
