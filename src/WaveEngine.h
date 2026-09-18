#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

namespace mertwave {
constexpr int MaxWaves = 32;
enum Param { Trigger, Speed, Mode, Length, Direction, Flash, Hue, Saturation, Brightness, Alpha, Noise, Bump, Count };
using Params = std::array<float, Count>;
inline Params defaults() { return {0, .65f, 1, .3f, 0, .8f, 0, 0, 1, 1, .35f, .25f}; }
inline std::array<float,3> rgb(float hue,float saturation,float brightness) {
    std::array<float,3> result{};
    const float offsets[]={0.f, 2.f/3.f, 1.f/3.f};
    for(int i=0;i<3;i++) {
        float h=hue+offsets[i]; h-=std::floor(h);
        float channel=std::clamp(std::abs(h*6.f-3.f)-1.f,0.f,1.f);
        result[i]=brightness*(1.f-saturation+saturation*channel);
    }
    return result;
}
struct Wave { double born; Params p; };
inline double duration(float speed) { return 4.0 * std::pow(.0625, double(speed)); }
class Engine {
public:
    std::vector<Wave> waves;
    void prune(double now) {
        waves.erase(std::remove_if(waves.begin(), waves.end(), [now](const Wave& w) {
            return now-w.born >= duration(w.p[Speed]) || now < w.born;
        }), waves.end());
    }
    void trigger(double now, const Params& p) {
        prune(now);
        if (waves.size() == MaxWaves) waves.erase(waves.begin());
        waves.push_back({now,p});
    }
};
}
