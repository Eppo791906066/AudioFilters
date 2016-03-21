//
//  BMVelocityFilter.h
//  VelocityFilter
//
//  Created by Hans on 16/3/16.
//  Copyright © 2016 Hans. All rights reserved.
//

#ifndef BMVelocityFilter_h
#define BMVelocityFilter_h

#include <stdio.h>
#include "BMMultiLevelBiquad.h"

typedef struct BMVelocityFilter{
    BMMultiLevelBiquad bqf;
    bool roundRobinEnabled, velocityEnabled;
    float velMaxGainDb, velMinGainDb, rrMinFc, rrMaxFc, rrGainRange_db;
    size_t rrNumBands;
} BMVelocityFilter;


void BMVelocityFilter_processBufferStereo(BMVelocityFilter* vf, const float* inL, const float* inR, float* outR, float* outL, size_t numSamples);


void BMVelocityFilter_processBufferMono(BMVelocityFilter* vf, const float* input, float* output, size_t numSamples);

/* 
 * Call this function before the start of each new note if you are passing
 * velocity information to the filter.
 *
 * velocity in [0,127]
 *
 * fcForMIDINote - an array of 127 floats, specifying the cutoff frequency 
 * for velocity simulation. Above this frequency the gain is increased for 
 * higher velocity.  Below, it remains constant for all velocities. The 
 * filter is a second order high-shelf so the cutoff is very gradual and 
 * there will be some attenuation below the cutoff.
 */
void BMVelocityFilter_newNoteWithVelocity(BMVelocityFilter* vf, float velocity, size_t MIDINoteNumber, float* fcForMIDINote);



/*
 * Call this function before the start of each new note if you are not 
 * passing velocity information.
 */
void BMVelocityFilter_newNoteWithoutVelocity(BMVelocityFilter* vf);



/*
 * init a velocity filter without round-robin filtering
 */
void BMVelocityFilter_initNoRR(BMVelocityFilter* vf, float sampleRate, bool isStereo);


/*
 * init a velocity simulation filter with round-robin filtering
 *
 * rrNumBands - number of bell filters to use in simulating
 *                      round-robin samples
 *
 * rrMinFc - the cutoff frequency of the lowest bell filter for round robin
 * rrMaxFc - the cutoff frequency of the highest bell filter for round robin
 * rrMaxDb - the maximum excursion of gain used in round robin filtering, 
 *              in decibels
 *
 */
void BMVelocityFilter_init(BMVelocityFilter* vf, float sampleRate, size_t rrNumBands, float rrMinFc, float rrMaxFc, float rrGainRange_db, bool hasVelocity, bool isStereo);



/*
 * sets the range in db from the min to the max excursion of the high-shelf
 * filter used to simulate velocity layers
 */
void BMVelocityFilter_setVelocityGainRange(BMVelocityFilter* vf, float gainMin_db, float gainMax_db);


/*
 * the round robin filters will have random gain within +- gain_db
 */
void BMVelocityFilter_setRRGainRange(BMVelocityFilter* vf, float gain_db);



#endif /* BMVelocityFilter_h */
