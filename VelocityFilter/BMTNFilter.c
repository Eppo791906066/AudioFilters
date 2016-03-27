//
//  BMTNFilter.c
//  BMAudioFilters
//
//  Uses an adaptive least mean squares filter to separate the input
//  signal into tonal and noisy components
//
//  Created by Hans on 25/3/16.
//  Copyright © 2016 Hans. All rights reserved.
//

#include "BMTNFilter.h"
#include <Accelerate/Accelerate.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    void BMTNFilter_processSample(BMTNFilter* f, float input, float* toneOut, float* noiseOut);

    void BMTNFilter_init(BMTNFilter* f, size_t filterOrder, float mu, size_t delayTime){
        
        // FIR filter length = order + 1
        f->filterLength = filterOrder+1;
        
        
        // allocate memory
        size_t XmemLength = BMTNF_FILTER_WRAP_TIME + f->filterLength;
        f->Xmem = malloc(sizeof(float)*XmemLength);
        f->delayLine = malloc(sizeof(float)*delayTime);
        f->W = malloc(sizeof(float)*f->filterLength);
        
        
        // set initial position and the end marker for the delay line
        f->delayTime = delayTime;
        f->delayLineEnd = f->delayLine + delayTime;
        f->dp = f->delayLine;
        
        
        // set the initial position and end marker for the
        float* XmemEnd = f->Xmem + XmemLength;
        f->Xstart =  XmemEnd - f->filterLength;
        f->X = f->Xstart;
        
        
        // initialize arrays to zero
        memset(f->delayLine, 0, sizeof(float)*delayTime);
        memset(f->Xmem,0,sizeof(float)*XmemLength);
        memset(f->W,0,sizeof(float)*f->filterLength);
    }
    
    void BMTNFilter_processBuffer(BMTNFilter* f, const float* input, float* toneOut, float* noiseOut, size_t numSamples){
        
        // if f is not initialised, initialise it with reasonable defaults.
        if(!f->Xmem){
            BMTNFilter_init(f, 48, 0.001, 64);
        }
        
        for(size_t i=0; i<numSamples; i++)
            BMTNFilter_processSample(f, input[i], toneOut+i, noiseOut+i);
    }
    
    inline void BMTNFilter_processSample(BMTNFilter* f, float input, float* toneOut, float* noiseOut){
        
        // read and write from the delay line to get a delayed input
        float delayedInput = *f->dp;
        *f->dp = input;
        // advance the pointer and wrap if necessary
        f->dp++;
        if (f->dp == f->delayLineEnd) f->dp = f->delayLine;
        
        
        // advance the X delays
        //
        // pointer decrement and wrap if necessary
        f->X--;
        if(f->X < f->Xmem) {
            // move the data in X from the beginning to the end of the buffer
            memmove(f->Xstart+1, f->Xmem, sizeof(float)*f->filterLength-1);
            f->X = f->Xstart;
        }
        // write the delayed input into the first position in X
        *f->X = delayedInput;
        
        
        // tone is the output of the linear prediction model: tone = W.X
        vDSP_dotpr(f->W, 1, f->X, 1, toneOut, f->filterLength);

        
        // noise is the error in the prediction
        *noiseOut = input - *toneOut;
        
        
        // Update coefficients W = W + ue*X;
        float ue = *noiseOut * f->mu;
        vDSP_vsma(f->X, 1, &ue, f->W, 1, f->W, 1, f->filterLength);
    }
    
    void BMTNFilter_destroy(BMTNFilter* f){
        free(f->Xmem);
        free(f->delayLine);
        free(f->W);
    }

#ifdef __cplusplus
}
#endif