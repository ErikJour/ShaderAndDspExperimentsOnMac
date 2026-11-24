//
// Created by Erik Jourgensen on 10/7/26.
//

#pragma once
#include "AudioToolBox/AudioToolBox.h"
#include "../AudioUtilities.h"
#include "../AudioTypes.h"
#include <iostream>
#include <stdatomic.h>
#include <cmath>


float gainFactor = 0.9f;

//======================================================================================
inline void renderVoice(MyRenderer *renderer, AudioBufferList *ioData, UInt32 inNumberFrames) {

    auto *leftChannel  = (Float32 *) ioData->mBuffers[0].mData;
    auto *rightChannel = (Float32 *) ioData->mBuffers[1].mData;

    float silence = 0.0f;
    
    float average = 0.0f;

    
        for (int frame = 0; frame < inNumberFrames; ++frame) {
            
            float oscSample       = renderer->osc.nextValue();
            float noiseSample     = renderer->noise.nextValue() * silence;
            float sample          = (oscSample + noiseSample) * renderer->gain.applyGain(0.5f);
            average += std::abs(sample);
            
//            float release         = renderer->env.nextValue();

            (leftChannel)[frame]  = (rightChannel)[frame] = (sample) /** release*/;
          
            //Ok, now an oscillator plays by holding down the bottom left square
         
//            (leftChannel)[frame]   *= 0.0;
//            (rightChannel)[frame]  *= 0.0;
          
            
            renderer->currentSample++;
        }
        average = average / inNumberFrames;
        renderer->data.setOutputData(average);
    }
