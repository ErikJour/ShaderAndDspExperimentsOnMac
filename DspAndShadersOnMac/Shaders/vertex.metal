#include "sharedShader.h"

vertex RasterizerData vertexMain(uint vertexID [[ vertex_id ]],
                                constant game_vertex *vertexArray [[ buffer(0) ]],
                                constant Uniforms &uniforms [[buffer(11)]])
{
    RasterizerData out;


    float2 pixelSpacePosition = vertexArray[vertexID].position.xy;
    float2 uv                 = vertexArray[vertexID].uv;
        
    float normalizedPositionX = (pixelSpacePosition.x / (GLOBAL_WIDTH / 2.0)) - 1.0;
    float normalizedPositionY = (pixelSpacePosition.y / (GLOBAL_HEIGHT / 2.0)) - 1.0;
    

    switch (uniforms.materialId)
    {
        case MAT_BOTTOM_LEFT:
        default:
            out.position = float4(normalizedPositionX,
                                  normalizedPositionY,
                                  0.0,
                                  1.0);
            break;
    }

    out.color    = vertexArray[vertexID].color;
    
    //UV is a coordinate system for the shape being rendered
    out.uv       = vertexArray[vertexID].uv;

    
    return out;
}


