#include "LEDs.h"
#include "sharedDataStruct.h"
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stddef.h>


#define I2C_DEVICE_ADDRESS 0x70
#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

static void runCommand(char *command);
static int initI2cBus(char *bus, int address);
static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value);
static unsigned char concatenate(unsigned char  arr[]); 

static int initI2cBus(char *bus, int address)
{
    int i2cFileDesc = open(bus, O_RDWR);
    int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
    if (result < 0)
    {
        perror("I2C: Unable to set I2C device to slave address.");
        exit(1);
    }
    return i2cFileDesc;
}

static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
{
    unsigned char buff[2];
    buff[0] = regAddr;
    buff[1] = value;
    int res = write(i2cFileDesc, buff, 2);
    if (res != 2)
    {
        perror("I2C: Unable to write i2c register.");
        exit(1);
    }
}

static void runCommand(char *command)
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

void disp_init()
{
    runCommand("config-pin P9_18 i2c");
    runCommand("config-pin P9_17 i2c");
    runCommand("i2cset -y 1 0x70 0x21 0x00");
    runCommand("i2cset -y 1 0x70 0x81 0x00");
    clearLEDs(); 
}


void Display_waveform(sharedMemStruct_t shared_Data, int indx)
{
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    unsigned char temp_arr[8]; 
    
    if (shared_Data.isReady )
    {
        int j=0; 
        for (int i=indx; i < indx+8; i++)
        {  
           
            temp_arr[j]=shared_Data.msgToFlash[i];
            //printf("%d",temp_arr[j]); 
            j++; 
             
        } 
        unsigned int num = concatenate(temp_arr);
      
        //conditions for the last seven column display 
        if (shared_Data.size== 1 )
        {
            num &= 0x40; 
        }

        if (indx == shared_Data.size- 7 )
        {
            num &= 0x7F; 
        }
        if (indx == shared_Data.size- 6 )
        {
            num &= 0x7E; 
        }
        if (indx == shared_Data.size- 5 )
        {
            num &= 0x7C; 
        }
        if (indx == shared_Data.size- 4 )
        {
            num &= 0x78; 
        }
        if (indx == shared_Data.size- 3 )
        {
            num &= 0x70; 
        }
        if (indx == shared_Data.size- 2)
        {
            num &= 0x60; 
        }
        if (indx == shared_Data.size- 1)
        {
            num &= 0x40; 
        }
    
        writeI2cReg(i2cFileDesc,0x06, num);
        writeI2cReg(i2cFileDesc,0x04, num);
        writeI2cReg(i2cFileDesc,0x02, num);
        writeI2cReg(i2cFileDesc,0x00, num);


    }
    close(i2cFileDesc);
}


void clearLEDs()
{
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    writeI2cReg(i2cFileDesc,0x0E, 0x00);
    writeI2cReg(i2cFileDesc,0x0C, 0x00);
    writeI2cReg(i2cFileDesc,0x0A, 0x00);
    writeI2cReg(i2cFileDesc,0x08, 0x00);
    writeI2cReg(i2cFileDesc,0x06, 0x00);
    writeI2cReg(i2cFileDesc,0x04, 0x00);
    writeI2cReg(i2cFileDesc,0x02, 0x00);
    writeI2cReg(i2cFileDesc,0x00, 0x00);
}
//function to concatenate the array as a string
static unsigned char concatenate(unsigned char  arr[])
{
    int hex=0; 
    for (int k=0; k<8; k++)
    {
        if (arr[k]!=0)
        {
            hex |= 1<< (7-k); 
        }
    }
    //rotate right 
    unsigned char res= (hex >> 1)|(hex << (8 - 1)); 
    return res; 
}