#ifndef _SHARED_DATA_STRUCT_H_
#define _SHARED_DATA_STRUCT_H_

#include <stdbool.h>
#include <stdint.h>

// WARNING:
// Fields in the struct must be aligned to match ARM's alignment
//    bool/char, uint8_t:   byte aligned
//    int/long, uint32_t:   word (4 byte) aligned
//    double, uint64_t:     dword (8 byte) aligned
// Add padding fields (char _p1) to pad out to alignment.

// My Shared Memory Structure
// ----------------------------------------------------------------
typedef struct
{
    uint32_t _p0;
    uint32_t msgToFlash[2048];
    unsigned char _p1;
    bool isReady;
    bool toggleLed;
    bool isButtonPressed;
    uint32_t current_indx;
    uint32_t size;
    uint32_t sleepSize;
} sharedMemStruct_t;

#endif
