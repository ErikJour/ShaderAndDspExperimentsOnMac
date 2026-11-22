//
//  squareShaders.h
//  DspAndShadersOnMac
//
//  Created by Erik Jourgensen on 10/31/26.
//

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

float perlin2D(float2 p) {
    float2 i = floor(p);
    float2 f = fract(p);

    // Quintic fade curve (6t^5 - 15t^4 + 10t^3) used by classic Perlin / MaterialX noise
    float2 u = f * f * f * (f * (f * 6.0 - 15.0) + 10.0);

    float n00 = dot(grad2(i + float2(0.0, 0.0)), f - float2(0.0, 0.0));
    float n10 = dot(grad2(i + float2(1.0, 0.0)), f - float2(1.0, 0.0));
    float n01 = dot(grad2(i + float2(0.0, 1.0)), f - float2(0.0, 1.0));
    float n11 = dot(grad2(i + float2(1.0, 1.0)), f - float2(1.0, 1.0));

    float nx0 = mix(n00, n10, u.x);
    float nx1 = mix(n01, n11, u.x);
    float nxy = mix(nx0, nx1, u.y);

    // Remap signed noise into roughly [0.0, 1.0] for greyscale display
    return clamp(nxy * 0.70710678 + 0.5, 0.0, 1.0);
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
    
    //Pattern 8 -> Perlin Noise (not working yet)
    float perlinNoise = perlin2D(vUv);
    color = float3(perlinNoise);
    
    return float4(color, 1.0);
}
