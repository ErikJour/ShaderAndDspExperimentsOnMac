#include "sharedShader.h"
#include "squareShaders.h"


fragment float4 fragmentMain(RasterizerData in [[stage_in]],
                                constant Uniforms &uniforms [[buffer(11)]])
{
    if (uniforms.materialId == MAT_BOTTOM_LEFT)  { return fragmentPlane  (in, uniforms); }
    
    return in.color;
}
