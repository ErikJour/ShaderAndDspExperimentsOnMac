// Common.h
//A bunch of custom data structures. Need to clean this up


#pragma once

#ifndef __METAL_VERSION__
// C++ STL imports (ignored by MSL shader compiler)
#include <iostream>
#include <simd/simd.h>
#endif

#define GLOBAL_WIDTH  700
#define GLOBAL_HEIGHT 394

#ifndef __METAL_VERSION__
// CPU-only global variable declaration
extern vector_float4 buttonColor;
static const NS::UInteger kMaxBuffers = 3;
#endif

//For clicked squares
typedef struct {
    bool         topLeft;
    bool         bottomLeft;
    bool         topRight;
    bool         bottomRight;
    }Clicked;

//Uniforms for cpu to gpu data sharing
typedef struct {
    float         audioData;
    vector_float2 viewportSize;
    float         peakX;
    uint32_t      materialId;
    Clicked       clickedSquare;
} Uniforms;

//Shader materials
typedef enum :uint32_t {

    MAT_BOTTOM_LEFT  = 0,
    MAT_BOTTOM_RIGHT = 1,
    MAT_TOP_LEFT     = 2,
    MAT_TOP_RIGHT    = 3,
    MAT_COUNT
} MaterialId;

//vertex buffers 
struct game_vertex {
    vector_float4 position;
    vector_float4 color;
};

struct Colors {
    vector_float4 blue  = { 0.0f, 0.0f, 1.0f, 1.0f };
    vector_float4 red   = { 1.0f, 0.0f, 0.0f, 1.0f };
    vector_float4 green = { 0.0f, 1.0f, 0.0f, 1.0f };
    vector_float4 cream = { 0.5f, 1.0f, 1.0f, 1.0f };
};

#ifndef __METAL_VERSION__
// CPU-only C++ buffer structures
struct VertexBuffer {
    game_vertex *vertices;
    uint32_t     drawCount;
};

struct GameRenderCommands {
    VertexBuffer *vertexBuffer[3];
    uint32_t      currentFrame;
};
#endif
