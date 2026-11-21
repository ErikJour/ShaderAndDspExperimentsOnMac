//
//  AnimatedRenderer.hpp
//  DspAndShadersOnMacApp
//
//  Created by Erik Jourgensen on 11/19/26.
//

#ifndef AnimatedRenderer_hpp
#define AnimatedRenderer_hpp
#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>
#include <stdio.h>
#include "Common.h"
#include "../Audio/AudioRenderer.h"
#include "../Geometries/SquareGeo.h"

static constexpr size_t kMaxFramesInFlight = 3;
inline constexpr vector_float4 blue    = { 0.0f, 0.0f, 1.0f, 1.0f };
inline constexpr vector_float4 red     = { 1.0f, 0.0f, 0.0f, 1.0f };
inline constexpr vector_float4 green   = { 0.0f, 1.0f, 0.0f, 1.0f };
inline constexpr vector_float4 magenta = { 1.0f, 0.0f, 1.0f, 1.0f };

class AnimatedRenderer {
public:
    explicit AnimatedRenderer(MTL::Device* device, MyRenderer* audioRenderer);
    ~AnimatedRenderer();

    void Draw(MTK::View* view);
    void BuildShaders();
    void BuildBuffers();
    void MouseDown(float x, float y);
    void MouseUp(float x, float y);
    void setBottomLeftClicked(bool clicked);
    void setBottomRightClicked(bool clicked);
    void setTopLeftClicked(bool clicked);
    void setTopRightClicked(bool clicked);
   
private:
    MTL::Device*                                 mDevice;
    std::array<NS::SharedPtr<MTL::Buffer>,
                            kMaxFramesInFlight>  mVertexBuffers;
    MTL::CommandQueue*                           mCommandQueue;
    MTL::RenderPipelineState*                    mPipelineState;
    GameRenderCommands                           mGameRenderCommands;
    Uniforms                                     mUniforms{};
    MyRenderer*                                  mAudioRenderer;
    UInt32                                       mX{0};
    dispatch_semaphore_t                         mFrameBoundarySemaphore;
    uint32_t                                     mCurrentFrameIndex;
};


#endif /* AnimatedRenderer_hpp */
