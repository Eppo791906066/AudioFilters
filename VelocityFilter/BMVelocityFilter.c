//
//  BMVelocityFilter.c
//  VelocityFilter
//
//  Created by Hans on 16/3/16.
//  Copyright © 2016 Hans. All rights reserved.
//

#include "BMVelocityFilter.h"
#include "Constants.h"


void BMVelocityFilter_randomiseRR(BMVelocityFilter* vf);

void BMVelocityFilter_init(BMVelocityFilter* vf, float sampleRate, size_t rrNumBands, float rrMinFc, float rrMaxFc, float rrGainRange_db, bool hasVelocity, bool stereo){
    vf->rrNumBands = rrNumBands;
    vf->rrMinFc = rrMinFc;
    vf->rrMaxFc = rrMaxFc;
    vf->rrGainRange_db = rrGainRange_db;
    vf->velocityEnabled = hasVelocity;
    vf->roundRobinEnabled = true;
    
    // velocity requires one additional level of filtering
    size_t numLevels = hasVelocity ? rrNumBands+1 : rrNumBands;
    
    BMMultiLevelBiquad_init(&vf->bqf, numLevels, sampleRate, stereo, true);
    
    // initialize the round robin filter with a random setting
    BMVelocityFilter_randomiseRR(vf);
    
    // initialize the velocity filter with bypass
    if (hasVelocity) {
        float fcForMIDINote = 1000.0;
        BMVelocityFilter_newNoteWithVelocity(vf, 100, 0, &fcForMIDINote);
    }
}


void BMVelocityFilter_initNoRR(BMVelocityFilter* vf, float sampleRate, bool stereo){
    vf->velocityEnabled = true;
    vf->roundRobinEnabled = false;
    
    // velocity requires one additional level of filtering
    size_t numLevels = 1;
    
    BMMultiLevelBiquad_init(&vf->bqf, numLevels, sampleRate, stereo, true);
    
    float fcForMIDINote = 1000.0;
    BMVelocityFilter_newNoteWithVelocity(vf, 100, 0, &fcForMIDINote);
}


// this is just an alias for randomiseRR
void BMVelocityFilter_newNoteWithoutVelocity(BMVelocityFilter* vf){
    BMVelocityFilter_randomiseRR(vf);
}

inline void BMVelocityFilter_randomiseRR(BMVelocityFilter* vf){
    float fc = vf->rrMinFc;
    
    // find the space between individual peaks in the filters
    // spacing them evenly in linear frequency
    float peakSpacing = (vf->rrMaxFc - vf->rrMinFc)/(float)(vf->rrNumBands - 1);
    
    for (size_t i=0; i<vf->rrNumBands; i++) {
        // find out if the rr filters start at level 1 or 0
        size_t shiftForVelFilter = (vf->velocityEnabled) ? 1 : 0;
        
        float gain_db = (((float)rand()/(float)RAND_MAX)-0.5)*2.0*vf->rrGainRange_db;
        
        // note that Q = peakSpacing
        BMMultiLevelBiquad_setBell(&(vf->bqf), fc, peakSpacing, gain_db, i+shiftForVelFilter);
        
        // move the fc over for the next peak
        fc += peakSpacing;
    }
}



void BMVelocityFilter_processBufferStereo(BMVelocityFilter* vf, const float* inL, const float* inR, float* outR, float* outL, size_t numSamples){
    BMMultiLevelBiquad_processBufferStereo(&vf->bqf, inL, inR, outL, outR, numSamples);
}



void BMVelocityFilter_processBufferMono(BMVelocityFilter* vf, const float* input, float* output, size_t numSamples){
    BMMultiLevelBiquad_processBufferMono(&vf->bqf, input, output, numSamples);
}



void BMVelocityFilter_newNoteWithVelocity(BMVelocityFilter* vf, float velocity, size_t MIDINoteNumber, float* fcForMIDINote){
    if (vf->roundRobinEnabled) BMVelocityFilter_randomiseRR(vf);
    
    // find the high shelf filter gain in decibels for the give velocity
    float gain;
    if (velocity > 100.0) gain = vf->velMaxGainDb*(velocity-100.0)/27.0;
    else gain = vf->velMinGainDb*(100.0-velocity)/100.0;
    float gain_db = BM_GAIN_TO_DB(gain);
    
    // set the high shelf filter on the first level,
    // looking up the fc from the table in fcForMIDINote
    BMMultiLevelBiquad_setHighShelf(&vf->bqf,
                                    fcForMIDINote[MIDINoteNumber],
                                    gain_db,
                                    1);
    
    // do a new randomised filtering for round robin simulation
    if(vf->roundRobinEnabled) BMVelocityFilter_randomiseRR(vf);
}



void BMVelocityFilter_setRRGainRange(BMVelocityFilter* vf, float rrGainRange_db){
    vf->rrGainRange_db = rrGainRange_db;
}