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

//========================================================
inline float4 fragmentBottomLeft(RasterizerData in,
       constant Uniforms &uniforms)
{
        float data    = uniforms.audioData;
        float3 mod    = (1.0 * uniforms.clickedSquare.bottomLeft) + 1.0;
        float3 colorA = float3(0.149,0.141,0.312);
        float3 colorB = float3(1.000,0.333,0.224);
        float2 st     = in.uv;
        float3 color  = float3(0.0);
        float3 pct    = float3(st.x);
    
        pct.r = smoothstep(0.0,1.0, st.x * data);
        pct.g = sin(st.x*PI * data);
        pct.b = pow(st.x, 0.5);
        color = mix(colorA, colorB, pct);

        return float4(color * mod, 1.0);
}
//========================================================

inline float4 fragmentBottomRight(RasterizerData in,
       constant Uniforms &uniforms)
{
       float3 colorA = float3(0.39,0.141,0.912);
       float3 colorB = float3(1.000,0.233,0.224);
       float2 st     = in.uv;
       float3 color  = float3(0.0);
       float3 pct    = float3(st.x);
        float3 mod   = (1.0 * uniforms.clickedSquare.bottomRight) + 1.0;
    
       pct.r = smoothstep(0.0,1.0, st.x);
       pct.g = sin(st.x*PI);
       pct.b = pow(st.x, 0.5);
       color = mix(colorA, colorB, pct);

       return float4(color * mod, 1.0);
}

//========================================================

inline float4 fragmentTopLeft(RasterizerData in,
       constant Uniforms &uniforms)
{
       float3 colorA = float3(0.239,0.841,0.312);
       float3 colorB = float3(0.243,0.443,0.224);
       float2 st     = in.uv;
       float3 color  = float3(0.0);
       float3 pct    = float3(st.x);
       float3 mod    = (1.0 * uniforms.clickedSquare.topLeft) + 1.0;
    
       pct.r = smoothstep(0.0,1.0, st.x);
       pct.g = sin(st.x*PI);
       pct.b = pow(st.x, 0.5);
       color = mix(colorA, colorB, pct);

       return float4(color * mod, 1.0);
}

//========================================================

inline float4 fragmentTopRight(RasterizerData in,
       constant Uniforms &uniforms)
{
       float3 colorA = float3(0.839,0.741,0.312);
       float3 colorB = float3(0.475,0.543,0.345);
       float2 st     = in.uv;
       float3 color  = float3(0.0);
       float3 pct    = float3(st.x);
       float3 mod    = (1.0 * uniforms.clickedSquare.topRight) + 1.0;
    
       pct.r = smoothstep(0.0,1.0, st.x);
       pct.g = sin(st.x*PI);
       pct.b = pow(st.x, 0.5);
       color = mix(colorA, colorB, pct);
    
       return float4(color * mod, 1.0);
}
