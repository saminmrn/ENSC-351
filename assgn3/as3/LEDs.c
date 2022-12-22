#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stddef.h>
#include "LEDs.h"

#define I2C_DEVICE_ADDRESS 0x70
#define REG_DIR1 0x00
#define REG_DIR2 0x02
#define REG_DIR3 0x04
#define REG_DIR4 0x06
#define REG_DIR5 0x08
#define REG_DIR6 0x0A
#define REG_DIR7 0x0C
#define REG_DIR8 0x0E
#define REG_OUTA 0x14
#define REG_OUTB 0x15
#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"
#define LED_ROWS 8
#define shift(a) (a >> 4)
#define MODE_NUM 4

static void runCommand(char *command);
static int initI2cBus(char *bus, int address);
static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value);

typedef struct
{
    unsigned char address_row;
    int row_hex_value;
} hex_display_t;

hex_display_t num0[] = {
    {REG_DIR1, 0x00},
    {REG_DIR2, 0x20},
    {REG_DIR3, 0x50},
    {REG_DIR4, 0x50},
    {REG_DIR5, 0x50},
    {REG_DIR6, 0x50},
    {REG_DIR7, 0x50},
    {REG_DIR8, 0x20},
};
hex_display_t num1[] = {
    {REG_DIR1, 0x00},
    {REG_DIR2, 0x70},
    {REG_DIR3, 0x20},
    {REG_DIR4, 0x20},
    {REG_DIR5, 0x20},
    {REG_DIR6, 0x20},
    {REG_DIR7, 0x60},
    {REG_DIR8, 0x20},
};
hex_display_t num2[] = {
    {REG_DIR1, 0x00},
    {REG_DIR2, 0x70},
    {REG_DIR3, 0x40},
    {REG_DIR4, 0x20},
    {REG_DIR5, 0x10},
    {REG_DIR6, 0x10},
    {REG_DIR7, 0x50},
    {REG_DIR8, 0x20},
};
hex_display_t num3[] = {
    {REG_DIR1, 0x00},
    {REG_DIR2, 0x20},
    {REG_DIR3, 0x50},
    {REG_DIR4, 0x10},
    {REG_DIR5, 0x20},
    {REG_DIR6, 0x10},
    {REG_DIR7, 0x50},
    {REG_DIR8, 0x20},
};
hex_display_t num4[] = {
    {REG_DIR1, 0x00},
    {REG_DIR2, 0x10},
    {REG_DIR3, 0x10},
    {REG_DIR4, 0x10},
    {REG_DIR5, 0x70},
    {REG_DIR6, 0x50},
    {REG_DIR7, 0x50},
    {REG_DIR8, 0x50},
};
hex_display_t num5[] = {
    {REG_DIR1, 0x00},
    {REG_DIR2, 0x70},
    {REG_DIR3, 0x50},
    {REG_DIR4, 0x10},
    {REG_DIR5, 0x70},
    {REG_DIR6, 0x40},
    {REG_DIR7, 0x50},
    {REG_DIR8, 0x70},
};
hex_display_t num6[] = {
    {REG_DIR1, 0x00},
    {REG_DIR2, 0x20},
    {REG_DIR3, 0x50},
    {REG_DIR4, 0x50},
    {REG_DIR5, 0x60},
    {REG_DIR6, 0x40},
    {REG_DIR7, 0x20},
    {REG_DIR8, 0x10},
};
hex_display_t num7[] = {
    {REG_DIR1, 0x00},
    {REG_DIR2, 0x10},
    {REG_DIR3, 0x10},
    {REG_DIR4, 0x10},
    {REG_DIR5, 0x38},
    {REG_DIR6, 0x10},
    {REG_DIR7, 0x10},
    {REG_DIR8, 0x70},
};
hex_display_t num8[] = {
    {REG_DIR1, 0x00},
    {REG_DIR2, 0x20},
    {REG_DIR3, 0x50},
    {REG_DIR4, 0x50},
    {REG_DIR5, 0x20},
    {REG_DIR6, 0x50},
    {REG_DIR7, 0x50},
    {REG_DIR8, 0x20},
};
hex_display_t num9[] = {
    {REG_DIR1, 0x00},
    {REG_DIR2, 0x10},
    {REG_DIR3, 0x10},
    {REG_DIR4, 0x10},
    {REG_DIR5, 0x70},
    {REG_DIR6, 0x50},
    {REG_DIR7, 0x50},
    {REG_DIR8, 0x20},
};
// Modes
hex_display_t Mode0[] = {
    {REG_DIR1, 0x00},
    {REG_DIR2, 0x45},
    {REG_DIR3, 0xC6},
    {REG_DIR4, 0xC6},
    {REG_DIR5, 0xC6},
    {REG_DIR6, 0xD6},
    {REG_DIR7, 0xEE},
    {REG_DIR8, 0x45},
};
hex_display_t Mode1[] = {
    {REG_DIR1, 0x00},
    {REG_DIR2, 0xC7},
    {REG_DIR3, 0x45},
    {REG_DIR4, 0x45},
    {REG_DIR5, 0x45},
    {REG_DIR6, 0x55},
    {REG_DIR7, 0x6F},
    {REG_DIR8, 0x45},
};
hex_display_t Mode2[] = {
    {REG_DIR1, 0x00},
    {REG_DIR2, 0xC7},
    {REG_DIR3, 0x46},
    {REG_DIR4, 0x45},
    {REG_DIR5, 0xC4},
    {REG_DIR6, 0xD4},
    {REG_DIR7, 0xEE},
    {REG_DIR8, 0x45},
};
hex_display_t Mode3[] = {
    {REG_DIR1, 0x00},
    {REG_DIR2, 0x45},
    {REG_DIR3, 0xC6},
    {REG_DIR4, 0xC4},
    {REG_DIR5, 0x45},
    {REG_DIR6, 0xD4},
    {REG_DIR7, 0xEE},
    {REG_DIR8, 0x45},
};
// array for integers 0 to 9
static hex_display_t *num_array[10] = {num0, num1, num2, num3, num4, num5, num6, num7, num8, num9};

static hex_display_t *mode_array[MODE_NUM] = {Mode0, Mode1, Mode2, Mode3};

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
}

void Display_number(int value)
{
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    int digit1, digit2;
    // divide the number into digits
    if (value > 9)
    {
        digit1 = value / 10;
        digit2 = value % 10;
    }
    else
    {
        // get the second digit
        digit1 = 0;
        digit2 = value;
    }

    // merge the digits into a single number array with shift register values
    hex_display_t *num = (hex_display_t *)malloc(sizeof(hex_display_t) * 8);
    for (int i = 0; i < LED_ROWS; i++)
    {
        num[i].address_row = num_array[digit1][i].address_row;
        int row_shift = shift(num_array[digit2][i].row_hex_value);
        num[i].row_hex_value = num_array[digit1][i].row_hex_value + row_shift;

        // write to the shift register
        writeI2cReg(i2cFileDesc, num[i].address_row, num[i].row_hex_value);
    }
    free(num);
    close(i2cFileDesc);
}

void Display_number_double(double value)
{
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    int digit1, digit2;
    if (value > 9.9 || value < 0)
    {
        // clear the display
        for (int i = 0; i < LED_ROWS; i++)
        {
            writeI2cReg(i2cFileDesc, num_array[0][i].address_row, 0x00);
        }
        return;
    }

    digit1 = (int)value;
    digit2 = ((value - digit1) * 10) + 0.1;

    // merge the digits into a single number array with shift register values
    hex_display_t *num = (hex_display_t *)malloc(sizeof(hex_display_t) * 8);
    for (int i = 0; i < LED_ROWS; i++)
    {
        num[i].address_row = num_array[digit1][i].address_row;
        int row_shift = shift(num_array[digit2][i].row_hex_value);
        num[i].row_hex_value = num_array[digit1][i].row_hex_value + row_shift;

        // write to the shift register
        writeI2cReg(i2cFileDesc, num[i].address_row, num[i].row_hex_value);
        writeI2cReg(i2cFileDesc, 0x00, 0x08);
    }
    free(num);
    close(i2cFileDesc);
}

void Display_mode(int mode_number)
{
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);

    for (int i = 0; i < LED_ROWS; i++)
    {
        writeI2cReg(i2cFileDesc, mode_array[mode_number][i].address_row, mode_array[mode_number][i].row_hex_value);
    }

    close(i2cFileDesc);
}
