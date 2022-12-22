#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <pru_cfg.h>
#include "resource_table_empty.h"
#include "../as4-Linux/sharedDataStruct.h"

#define DELAY_250_MS 50000000
#define DELAY_500_MS (DELAY_250_MS * 2)
#define DELAY_1_MS (50000000 / 250)

void delayInMs(int msDelay)
{
    for (int i = 0; i < msDelay; i++)
    {
        __delay_cycles(DELAY_1_MS);
    }
}
// Reference for shared RAM:
// https://markayoder.github.io/PRUCookbook/05blocks/blocks.html#_controlling_the_pwm_frequency

// GPIO Configuration
// ----------------------------------------------------------
volatile register uint32_t __R30; // output GPIO register
volatile register uint32_t __R31; // input GPIO register

// Use pru0_rpu_r30_5 as an output (bit 5 = 0b0010 0000)
#define LED_MASK (1 << 5)

// Use pru0_pru_r31_3 as a button (bit 3 = 0b0000 1000)
#define BUTTON_MASK (1 << 3)

// Shared Memory Configuration
// -----------------------------------------------------------
#define THIS_PRU_DRAM 0x00000 // Address of DRAM
#define OFFSET 0x200          // Skip 0x100 for Stack, 0x100 for Heap (from makefile)
#define THIS_PRU_DRAM_USABLE (THIS_PRU_DRAM + OFFSET)

// This works for both PRU0 and PRU1 as both map their own memory to 0x0000000
volatile sharedMemStruct_t *pSharedMemStruct = (volatile void *)THIS_PRU_DRAM_USABLE;

void main(void)
{
    // Initialize:
    pSharedMemStruct->isReady = false;
    pSharedMemStruct->msgToFlash[0] = false;
    pSharedMemStruct->current_indx = 0;
    pSharedMemStruct->size = 0;
    pSharedMemStruct->sleepSize = 300;
    pSharedMemStruct->toggleLed = false;
    while (true)
    {
        if (pSharedMemStruct->toggleLed)
            __R30 |= LED_MASK;
        else
            __R30 &= ~LED_MASK;
        while ((pSharedMemStruct->isReady))
        {
            for (int i = 0; i < pSharedMemStruct->size; i++)
            {
                if (pSharedMemStruct->msgToFlash[i])
                {
                    //on
                     __R30 |= LED_MASK;
                }
                   
                else
                {
                    //off
                    __R30 &= ~LED_MASK;
                }
                //change the delay when button is pressed 
                pSharedMemStruct->isButtonPressed = (__R31 & BUTTON_MASK) != 0;
                if (pSharedMemStruct->isButtonPressed)
                {
                    pSharedMemStruct->sleepSize = 1000;
                }
                    
                else
                {
                    pSharedMemStruct->sleepSize = 300;
                }
                    
                pSharedMemStruct->current_indx = i;
                delayInMs(pSharedMemStruct->sleepSize);
                if (i == (pSharedMemStruct->size) + 1)
                {
                    break;
                }
            }
        }
    }
}
