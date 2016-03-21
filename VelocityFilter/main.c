//
//  main.c
//  CReverb
//
//  Created by Hans on 7/2/16.
//  Copyright © 2016 Hans. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/sysctl.h>
#include "BMVelocityFilter.h"
#include "BMGetOSVersion.h"


#define TESTBUFFERLENGTH 128

int main(int argc, const char * argv[]) {

    // open a file for writing
    FILE* audioFile;
    audioFile = fopen("./rvImpulse.csv", "w+");
    
    
    float testBufferInL [TESTBUFFERLENGTH];
    float testBufferInR [TESTBUFFERLENGTH];
    float testBufferOutL [TESTBUFFERLENGTH];
    float testBufferOutR [TESTBUFFERLENGTH];
    
    
    // create the initial impulse followed by zeros
    testBufferInL[0] = 1.0f;
    testBufferInR[0] = 1.0f;
    memset(testBufferInL+1, 0, sizeof(float)*(TESTBUFFERLENGTH-1));
    memset(testBufferInR+1, 0, sizeof(float)*(TESTBUFFERLENGTH-1));
    
    
    // process the first frame twice (the first time to trigger an update)
    //BMCReverbProcessBuffer(&rv, testBufferInL, testBufferInR, testBufferOutL, testBufferOutR, TESTBUFFERLENGTH);
    //BMCReverbProcessBuffer(&rv, testBufferInL, testBufferInR, testBufferOutL, testBufferOutR, TESTBUFFERLENGTH);
    
    // print out the entire frame in .csv format
    for (size_t i=0; i<TESTBUFFERLENGTH; i++) {
        fprintf(audioFile, "%f,%f\n",testBufferOutL[i],testBufferOutR[i]);
    }
    
    // set the input buffers to all zeros (only the first value was non-zero)
    testBufferInL[0] = testBufferInR[0] = 0.0;
    
    
    // start a timer
    clock_t begin, end;
    double time_spent;
    begin = clock();
    
    
    
    
    // print the time taken to process reverb
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("time: %f\n", time_spent);
    
    
    fclose(audioFile);
    return 0;
}
