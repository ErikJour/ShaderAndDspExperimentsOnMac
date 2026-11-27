//
// Created by Erik Jourgensen on 9/30/26.
//

#ifndef HOMEMADELPG_SQUAREGEO_H
#define HOMEMADELPG_SQUAREGEO_H

#include "../UI/Common.h"
constexpr float scale = 0.5f;

inline void drawSquare (game_vertex *vertices,
                        uint32_t *vertexCount,
                        float side,
                        vector_float4 color,
                        float offsetX,
                        float offsetY)
{
    float centerX     = GLOBAL_WIDTH * scale + offsetX;
    float centerY     = GLOBAL_HEIGHT * scale + offsetY;

    float minX        = centerX - side;
    float maxX        = centerX + side;

    float minY        = centerY - side;
    float maxY        = centerY + side;

    game_vertex v1 = {{minX, minY, 0.0f, 1.0f }, color, {0.0f, 0.0f} };
    vertices[(*vertexCount)++] = v1;

    game_vertex v2 = {{minX, maxY, 0.0f, 1.0f }, color, {0.0f, 1.0f} };
    vertices[(*vertexCount)++] = v2;

    game_vertex v3 = {{maxX, maxY, 0.0f, 1.0f }, color, {1.0f, 1.0f} };
    vertices[(*vertexCount)++] = v3;

    game_vertex v4 = {{minX, minY, 0.0f, 1.0f }, color, {0.0f, 0.0f} };
    vertices[(*vertexCount)++] = v4;

    game_vertex v5 = {{maxX, minY, 0.0f, 1.0f }, color, {1.0f, 0.0f} };
    vertices[(*vertexCount)++] = v5;

    game_vertex v6 = {{maxX, maxY, 0.0f, 1.0f }, color, {1.0f, 1.0f} };
    vertices[(*vertexCount)++] = v6;
}

#endif //HOMEMADELPG_SQUAREGEO_H
