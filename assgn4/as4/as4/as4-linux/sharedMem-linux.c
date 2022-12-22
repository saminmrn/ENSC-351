#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <sys/mman.h>
#include "sharedDataStruct.h"
#include "binary.h"
#include "morsecode.h"
#include <string.h>
#include "LEDs.h"

// General PRU Memomry Sharing Routine
// ----------------------------------------------------------------
#define PRU_ADDR 0x4A300000 // Start of PRU memory Page 184 am335x TRM
#define PRU_LEN 0x80000     // Length of PRU memory
#define PRU0_DRAM 0x00000   // Offset to DRAM
#define PRU1_DRAM 0x02000
#define PRU_SHAREDMEM 0x10000  // Offset to shared memory
#define PRU_MEM_RESERVED 0x200 // Amount used by stack and heap
// Convert base address to each memory section
#define PRU0_MEM_FROM_BASE(base) ((base) + PRU0_DRAM + PRU_MEM_RESERVED)
#define PRU1_MEM_FROM_BASE(base) ((base) + PRU1_DRAM + PRU_MEM_RESERVED)
#define PRUSHARED_MEM_FROM_BASE(base) ((base) + PRU_SHAREDMEM)
// Return the address of the PRU's base memory
void runCommand(char *command)
{
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");
    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe))
    {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
            break;
    }
    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0)
    {
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
}

static int sleepForTime(long long delayInMS)
{
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;

    long long delayNS = delayInMS * NS_PER_MS;
    int seconds = delayNS / NS_PER_SECOND;
    int nanoseconds = delayNS % NS_PER_SECOND;

    struct timespec reqDelay = {seconds, nanoseconds};
    return nanosleep(&reqDelay, (struct timespec *)NULL);
}


volatile void *getPruMmapAddr(void)
{
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1)
    {
        perror("ERROR: could not open /dev/mem");
        exit(EXIT_FAILURE);
    }
    // Points to start of PRU memory.
    volatile void *pPruBase = mmap(0, PRU_LEN, PROT_READ | PROT_WRITE,
                                   MAP_SHARED, fd, PRU_ADDR);
    if (pPruBase == MAP_FAILED)
    {
        perror("ERROR: could not map memory");
        exit(EXIT_FAILURE);
    }
    close(fd);
    return pPruBase;
}
static void freePruMmapAddr(volatile void *pPruBase)
{
    if (munmap((void *)pPruBase, PRU_LEN))
    {
        perror("PRU munmap failed");
        exit(EXIT_FAILURE);
    }
}

void get_user_input()
{
    volatile void *pPruBase = getPruMmapAddr();
    volatile sharedMemStruct_t *pSharedPru0 = PRU0_MEM_FROM_BASE(pPruBase);

    

    printf("Beginning Morse Code!! \n");

    while (true)
    {
        // getting user input
        int size_phrase = 0;
        int count = 0;
        pSharedPru0->msgToFlash[2048] = 0;
        char *buff = NULL;
        size_t sizeAllocated = 0;

        printf(">");
        size_t numCh = getline(&buff, &sizeAllocated, stdin);

        //exit if enter 
        if (numCh == 1)
        {
            printf("Exiting...\n");
            free(buff);
            buff = NULL;
            break;
        }

        // printing info to the terminal
        printf("Flashing out %d characters : ", numCh - 1);
        printf("'");
        for (int i = 0; i < numCh - 1; i++)
        {
            printf("%c", buff[i]);
        }
        printf("'");
        printf("\n");

        char *token = strtok(buff, " ");
        while (token != NULL)
        {
            int size_p = 0;
            for (int s = 0; s < strlen(token); s++)
            {
                bool *msg_arr = Binary(MorseCode_getFlashCode(token[s]), &size_p);
                size_phrase += size_p;
                for (int j = 0; j < size_p; j++)
                {
                    // fillout the msgToFlash Array
                    if (*(msg_arr + j))
                        pSharedPru0->msgToFlash[count] = 1;
                    else
                        pSharedPru0->msgToFlash[count] = 0;
                    count++;
                }
                // if not the last character in the word
                if (s != (strlen(token) - 1))
                {
                    for (int k = 0; k < 3; k++)
                    {
                        pSharedPru0->msgToFlash[count] = 0;
                        count++;
                        size_phrase++;
                    }
                }
            }
            token = strtok(NULL, " ");
            if (token != NULL)
            {
                for (int i = 0; i < 7; i++)
                {
                    pSharedPru0->msgToFlash[count] = 0;
                    count++;
                    size_phrase++;
                }
            }
        }
        size_phrase -= 3;
        pSharedPru0->size = size_phrase;

        for (int i = 0; i < size_phrase; i++)
        {
            if (pSharedPru0->msgToFlash[i])
                printf("X");
            else
                printf("_");
        }

        printf("\n");
        pSharedPru0->isReady = true;
    
        while ((pSharedPru0->current_indx) < (pSharedPru0->size))
        {
            
            
            if ((pSharedPru0->msgToFlash[pSharedPru0->current_indx]) == 1)
            {
                // on
                printf("X");
                fflush(stdout);
                
            }
            else if ((pSharedPru0->msgToFlash[pSharedPru0->current_indx]) == 0)
            {
                // off
                printf("_");
                fflush(stdout);
            }
             Display_waveform(*pSharedPru0 , pSharedPru0->current_indx);
            if ((pSharedPru0->current_indx) == (pSharedPru0->size) - 1)
            {   
                sleepForTime(pSharedPru0->sleepSize);
                clearLEDs(); 
                pSharedPru0->current_indx=0; 
                pSharedPru0->size=0;
                break;
            }
           
            sleepForTime(pSharedPru0->sleepSize);
            
        }

        pSharedPru0->toggleLed = true;
        sleepForTime(1);
        pSharedPru0->toggleLed = false;
        printf("\n");
        free(buff);
        buff = NULL;
        
    }

    // Cleanup
    freePruMmapAddr(pPruBase);
}