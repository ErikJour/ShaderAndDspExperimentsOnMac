//
// Created by Erik Jourgensen on 10/27/26.
//

#pragma once
#include "AudioToolBox/AudioToolBox.h"
#include "../../AudioTypes.h"
#include <iostream>

//======================================================================================

struct Oscillator {

    double phase       = 0.0;
    double increment   = 0.0;
    double mFrequency  = 400.0;
    double mSampleRate = 44100.0;
    
    void setFrequency(double frequency) {
        mFrequency = frequency;
        prepareOscillator();
    }
    
    void setSampleRate(double sampleRate) { mSampleRate = sampleRate; }

    void prepareOscillator() { increment  = mFrequency / mSampleRate; }

    float nextValue()
    {
        auto sine = (Float32) sin(2 * M_PI * phase);
        
        phase += increment;
        
        if (phase >- 1.0) { phase -= 1.0; }
        
        return sine;
    }

};
