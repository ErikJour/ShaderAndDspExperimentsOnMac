//
//  squareShaders.h
//  DspAndShadersOnMac
//
//  Created by Erik Jourgensen on 10/31/26.
//
#include "Helpers/PerlinNoise.h"

float plot (float2 st, float pct)
{
    return smoothstep(pct - 0.01, pct, st.y)
    - smoothstep(pct, pct + 0.01, st.y) ;
}

// Fast, low-bias 32-bit integer hash (Chris Wellons' lowbias32)
uint hash_uint(uint x) {
    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    x ^= x >> 16;
    return x;
}

// Convert hashed uint to a normalized float in [0.0, 1.0)
float hash_to_float(uint x) {
    return float(hash_uint(x)) / float(0xFFFFFFFFU);
}

//Noise function
float2 grad2(float2 p) {
    // Deterministic 2D pseudo-random angle from integer lattice point p
    float h = fract(sin(dot(p, float2(127.1, 311.7))) * 43758.5453123);
    float angle = h * 6.28318530718;
    return float2(cos(angle), sin(angle));
}

//========================================================
inline float4 fragmentPlane(RasterizerData in,
       constant Uniforms &uniforms)
{
    
    //You can look at three.js/src/nodes/procedural for some inspiration
      
    float2 vUv    = in.uv;
    
    //Pattern 1 vUv=============================
    float3 color  = float3(vUv.x, vUv.y, 1.0);
    
    //Pattern 2 -> grey gradient=================
    color = float3(vUv.x);
    
    //Pattern 3 -> Fan grayscale=================
    color = float3(fract(vUv.x * 10.0));
    
    //Pattern 4 -> Checkerboard===================Good for different notes, try an octave with 8 colored cells on a single plane
    float g = abs(step(0.5,
                fract(vUv.x * 10.0)) + step(0.5,
                fract(vUv.y * 10.0)) - 1.0);
    
    color = float3(g);
    
    //Pattern 5 -> distance from center============Good for sine experiment
    //Approach one
    float d = distance(vUv, float2(0.5));
    color   = float3(d);
    //Approach two
    float2 vUvSub = vUv - float2(0.5);
    float len     = length(vUvSub);
    color         = float3(len);
    
    //Pattern 6 -> Around the center==================Perhaps for filtering
    float2 polarUv = vUv - float2(0.5);
    float angle    = atan2(polarUv.x, polarUv.y);
    color          = float3((angle + PI) / (2 * PI));
    
    //Pattern 7 -> Randomization=======================Good for noise experiment
    float subdivision = 50;
    float2 gridUv     = floor(vUv * subdivision);
    float random      = hash_to_float(gridUv.x * subdivision + gridUv.y);
    color             = float3(random);
    
    //Pattern 8 -> Perlin Noise ===============================
    float2 perlinUv   = vUv * 15.0;
    float perlinNoise = mx_noise_float(perlinUv);
    color = float3(step(0.8,
                        fract(perlinNoise + uniforms.audioData)));
    
    //Pattern 9 -> Worley Noise ===============================
    float2 worleyUv = vUv * 10.0;
    float worleyNoise = mx_worley_noise_float(float3(worleyUv, uniforms.audioData));
    float worleyPower = pow(worleyNoise, 0.8);
    color = float3(palette(
                           worleyPower,
                           float3(0.5, 0.3, 0.4),
                           float3(0.9, 0.5, 0.4),
                           float3(1.0, 1.0, 1.0),
                           float3(0.0, 0.1, 0.2)));
    
    //Pattern 10 -> Worley Noise ===============================
    float3 cuasticInput = float3(vUv * 6.0, uniforms.audioData);
    float causticsNoise = pow(mx_worley_noise_float(cuasticInput), 6.0);
    float3 deep         = float3( 27,  57,  86) / 255.0f;
    float3 shallow      = float3( 17, 238, 255) / 255.0f;

    float3 depthColor   = mix(deep, shallow, causticsNoise);
    
    color               = float3(depthColor);

    
    return float4(color, 1.0);
}
