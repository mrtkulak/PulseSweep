#pragma once
namespace mertwave {
inline constexpr char Vertex[] = R"GLSL(#version 410 core
layout(location=0) in vec4 vPosition;
layout(location=1) in vec2 vUV;
out vec2 uv;
void main() { gl_Position=vPosition; uv=vUV; }
)GLSL";
inline constexpr char Fragment[] = R"GLSL(#version 410 core
uniform sampler2D InputTexture;
uniform vec2 MaxUV, Texel, Canvas;
uniform int WaveCount;
// progress, width, direction in radians, mode
uniform vec4 Geometry[32];
// flash, noise, bump, elapsed seconds
uniform vec4 Power[32];
uniform vec3 Tint[32];
in vec2 uv;
out vec4 fragColor;
float hash(vec2 p) { return fract(sin(dot(p,vec2(127.1,311.7)))*43758.5453); }
float noise(vec2 p) {
    vec2 i=floor(p), f=fract(p); f=f*f*(3.0-2.0*f);
    return mix(mix(hash(i),hash(i+vec2(1,0)),f.x),
               mix(hash(i+vec2(0,1)),hash(i+vec2(1,1)),f.x),f.y)*2.0-1.0;
}
void main() {
    // Normalized physical coordinates keep radial waves circular on wide canvases.
    vec2 scale=Canvas/min(Canvas.x,Canvas.y);
    vec2 p=(uv-.5)*scale;
    vec2 displacement=vec2(0);
    vec3 light=vec3(0);
    float coverage=0.0;
    for(int i=0;i<WaveCount;i++) {
        vec4 g=Geometry[i]; vec4 power=Power[i];
        vec2 axis=vec2(cos(g.z),sin(g.z));
        float extent=.5*dot(abs(axis),scale);
        float coordinate; vec2 normal; float span;
        if(g.w<.5) {
            span=length(scale*.5); coordinate=length(p);
            normal=p/max(length(p),.0001);
        } else if(g.w<1.5) {
            span=2.0*extent; coordinate=dot(p,axis)+extent; normal=axis;
        } else {
            span=extent; coordinate=abs(dot(p,axis));
            normal=axis*(dot(p,axis)<0.0 ? -1.0 : 1.0);
        }
        float width=max(.006, g.y*span);
        float front=mix(-width,span+width,g.x);
        float behind=front-coordinate;
        float envelope=smoothstep(-.035*width,.06*width,behind)*
            (1.0-smoothstep(.08*width,width,behind));
        float n1=noise(p*22.0+vec2(power.w*3.0,17.0));
        float n2=noise(p*25.0+vec2(31.0,-power.w*2.0));
        displacement += envelope*(normal*power.z*.12 + vec2(n1,n2)*power.y*.045)/scale;
        float glow=envelope*power.x;
        light += Tint[i]*glow;
        coverage += glow;
    }
    vec2 sampleUV=clamp((uv+displacement)*MaxUV,Texel*.5,MaxUV-Texel*.5);
    vec4 base=texture(InputTexture,sampleUV);
    float a=clamp(base.a+coverage,0.0,1.0);
    fragColor=vec4(clamp(base.rgb+light,vec3(0),vec3(a)),a);
}
)GLSL";
}
