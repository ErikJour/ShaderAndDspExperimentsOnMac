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
inline float4 fragmentPlane(RasterizerData in,
       constant Uniforms &uniforms)
{
    
    //You can look at three.js/src/nodes/procedural for some inspiration
      
    float2 vUv    = in.uv;
    
    //Pattern 1 vUv
    float3 color  = float3(vUv.x, vUv.y, 1.0);
    
    //Pattern 2 -> grey gradient
    color = float3(vUv.x);
    
    //Pattern 3 -> Fan grayscale
    color = float3(fract(vUv.x * 10.0));
    
    //Pattern 4 -> Checkerboard
    float g = abs(step(0.5, fract(vUv.x * 10.0)) + step(0.5, fract(vUv.y * 10.0)) - 1.0);
    color = float3(g);
    
    //Pattern 5 ->
    

    return float4(color, 1.0);
}
