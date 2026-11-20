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

class AnimatedRenderer {
public:
    explicit AnimatedRenderer(MTL::Device* device);
    ~AnimatedRenderer();

    void Draw(MTK::View* view);

private:
    MTL::Device* device{nullptr};
    MTL::CommandQueue* commandQueue{nullptr};
};


#endif /* AnimatedRenderer_hpp */
