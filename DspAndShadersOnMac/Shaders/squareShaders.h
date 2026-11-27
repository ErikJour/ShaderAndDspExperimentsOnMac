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

//===============================================================================================
inline float4 fragmentPlane(RasterizerData in,
       constant Uniforms &uniforms)
{
    //Uv and color
    float2 vUv          = in.uv;
    float3 color        = float3(vUv.x,
                                 vUv.y,
                                 1.0);
    //Mod data
    float modData       = uniforms.audioData;
    float valueToMod    = 20; //Noise depth
    
    //Noise modulation function
    float3 cuasticInput = float3(vUv * valueToMod,
                                 modData);
    float causticsNoise = pow(mx_worley_noise_float(cuasticInput),
                              6.0);
    
    //Mixing
    float3 deep    = float3( 0.1,
                            .22,
                            .34);
    
    float3 shallow = float3(0.0667f,
                            0.933f,
                            1.0f);
    
    float3 depthColor   = mix(deep,
                              shallow,
                              causticsNoise);
    color               = float3(depthColor);
    
    return float4(color, 1.0);
}

