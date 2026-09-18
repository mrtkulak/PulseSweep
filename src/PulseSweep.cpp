#include <FFGLSDK.h>
#include <chrono>
#include <mutex>
#include "WaveEngine.h"
#include "Shaders.h"

using namespace ffglex;
using namespace mertwave;
class PulseSweep final : public CFFGLPlugin {
    FFGLShader shader;
    FFGLScreenQuad quad;
    Params params=defaults();
    Engine engine;
    std::mutex guard;
    using Clock=std::chrono::steady_clock;
    Clock::time_point origin=Clock::now();
    double now() const { return std::chrono::duration<double>(Clock::now()-origin).count(); }
public:
    PulseSweep() {
        SetMinInputs(1); SetMaxInputs(1);
        SetParamInfo(Trigger,"Trigger",FF_TYPE_EVENT,false);
        SetParamInfo(Speed,"Speed",FF_TYPE_STANDARD,params[Speed]);
        SetOptionParamInfo(Mode,"Wipe Mode",3,params[Mode]);
        SetParamElementInfo(Mode,0,"Radial",0);
        SetParamElementInfo(Mode,1,"Linear",1);
        SetParamElementInfo(Mode,2,"Mirrored",2);
        SetParamInfo(Length,"Wipe Length",FF_TYPE_STANDARD,params[Length]);
        SetParamInfo(Direction,"Wipe Direction",FF_TYPE_STANDARD,params[Direction]);
        SetParamInfo(Flash,"Flash Power",FF_TYPE_STANDARD,params[Flash]);
        // Resolume's unified color widget uses consecutive HSB + alpha types.
        SetParamInfo(Hue,"Flash Color",FF_TYPE_HUE,0.f);
        SetParamInfo(Saturation,"Flash Color_saturation",FF_TYPE_SATURATION,0.f);
        SetParamInfo(Brightness,"Flash Color_brightness",FF_TYPE_BRIGHTNESS,1.f);
        SetParamInfo(Alpha,"Flash Color_alpha",FF_TYPE_ALPHA,1.f);
        SetParamInfo(Noise,"Noise Power",FF_TYPE_STANDARD,params[Noise]);
        SetParamInfo(Bump,"Bump Power",FF_TYPE_STANDARD,params[Bump]);
    }
    FFResult InitGL(const FFGLViewportStruct* vp) override {
        if(!vp) return FF_FAIL;
        if(!shader.Compile(Vertex,Fragment) || !quad.Initialise()) { DeInitGL(); return FF_FAIL; }
        return CFFGLPlugin::InitGL(vp);
    }
    FFResult DeInitGL() override {
        shader.FreeGLResources(); quad.Release();
        std::lock_guard<std::mutex> lock(guard); engine.waves.clear(); return FF_SUCCESS;
    }
    unsigned int Disconnect() override {
        std::lock_guard<std::mutex> lock(guard); engine.waves.clear(); return FF_SUCCESS;
    }
    FFResult SetFloatParameter(unsigned int i,float value) override {
        if(i>=Count || !std::isfinite(value)) return FF_FAIL;
        std::lock_guard<std::mutex> lock(guard);
        if(i==Trigger) {
            // FFGL events are pulses: each positive call is a distinct trigger.
            if(value>.5f) engine.trigger(now(),params);
            return FF_SUCCESS;
        }
        params[i]=i==Mode ? std::clamp(std::round(value),0.f,2.f) : std::clamp(value,0.f,1.f);
        return FF_SUCCESS;
    }
    float GetFloatParameter(unsigned int i) override {
        std::lock_guard<std::mutex> lock(guard);
        return i<Count ? params[i] : 0.f;
    }
    FFResult ProcessOpenGL(ProcessOpenGLStruct* data) override {
        if(!data || data->numInputTextures<1 || !data->inputTextures || !data->inputTextures[0]) return FF_FAIL;
        const auto& input=*data->inputTextures[0];
        if(!input.Width || !input.Height || !input.HardwareWidth || !input.HardwareHeight) return FF_FAIL;
        std::array<float,MaxWaves*4> geometry{},power{};
        std::array<float,MaxWaves*3> colors{};
        int count;
        {
            std::lock_guard<std::mutex> lock(guard);
            double t=now(); engine.prune(t); count=int(engine.waves.size());
            for(int i=0;i<count;i++) {
                const auto& w=engine.waves[i]; const auto& p=w.p;
                geometry[i*4]=float((t-w.born)/duration(p[Speed]));
                geometry[i*4+1]=.015f+.65f*p[Length];
                geometry[i*4+2]=p[Direction]*6.283185307f;
                geometry[i*4+3]=p[Mode];
                power[i*4]=p[Flash]*p[Alpha];power[i*4+1]=p[Noise];power[i*4+2]=p[Bump];power[i*4+3]=float(t-w.born);
                auto color=rgb(p[Hue],p[Saturation],p[Brightness]);
                colors[i*3]=color[0];colors[i*3+1]=color[1];colors[i*3+2]=color[2];
            }
        }
        ScopedShaderBinding binding(shader.GetGLID());
        ScopedSamplerActivation sampler(0);
        Scoped2DTextureBinding texture(input.Handle);
        shader.Set("InputTexture",0);shader.Set("WaveCount",count);
        auto maxUV=GetMaxGLTexCoords(input);
        shader.Set("MaxUV",maxUV.s,maxUV.t);
        shader.Set("Texel",1.f/input.HardwareWidth,1.f/input.HardwareHeight);
        shader.Set("Canvas",float(std::max(1u,currentViewport.width)),float(std::max(1u,currentViewport.height)));
        if(count) {
            glUniform4fv(shader.FindUniform("Geometry[0]"),count,geometry.data());
            glUniform4fv(shader.FindUniform("Power[0]"),count,power.data());
            glUniform3fv(shader.FindUniform("Tint[0]"),count,colors.data());
        }
        quad.Draw(); return FF_SUCCESS;
    }
};
static CFFGLPluginInfo Info(PluginFactory<PulseSweep>,"MWA1","PulseSweep",2,1,1,1,FF_EFFECT,
    "Triggered color sweeps with noise and lens distortion.","PulseSweep 1.1 - independent FFGL effect");
