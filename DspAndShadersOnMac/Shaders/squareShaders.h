//
//  squareShaders.h
//  DspAndShadersOnMac
//
//  Created by Erik Jourgensen on 10/31/26.
//
#include "Helpers/PerlinNoise.h"

//The idea here is to modulate a single filter coefficient with some user input (probably mouse),
//and modify this worley noise shader effect by updating valueToMod with a uniform value.
//Cutoff as 0 to 1 could be multiplied with valueToMod to start.

//========================================================
inline float4 fragmentPlane(RasterizerData in,
       constant Uniforms &uniforms)
{
    //uv and color
    float2 vUv    = in.uv;
    float3 color  = float3(vUv.x, vUv.y, 1.0);
    
    float modData = uniforms.audioData;

    //From pattern 10 on TSL lesson
    float valueToMod    = 30; //Could filter this
    
    float3 cuasticInput = float3(vUv * valueToMod, modData);
    float causticsNoise = pow(mx_worley_noise_float(cuasticInput), 6.0);
    float3 deep         = float3( 27,  57,  86) / 255.0f;
    float3 shallow      = float3( 17, 238, 255) / 255.0f;
    float3 depthColor   = mix(deep, shallow, causticsNoise);
    color               = float3(depthColor);

    
    return float4(color, 1.0);
}
