#include "LEDs.h"

typedef struct
{
    unsigned char address_row;
    int row_hex_value;
} numbers;

numbers num0[] = {
    {REG_DIR1, 0x00},
    {REG_DIR2, 0x20},
    {REG_DIR3, 0x50},
    {REG_DIR4, 0x50},
    {REG_DIR5, 0x50},
    {REG_DIR6, 0x50},
    {REG_DIR7, 0x50},
    {REG_DIR8, 0x20},
};
numbers num1[] = {
    {REG_DIR1, 0x00},
    {REG_DIR2, 0x70},
    {REG_DIR3, 0x20},
    {REG_DIR4, 0x20},
    {REG_DIR5, 0x20},
    {REG_DIR6, 0x20},
    {REG_DIR7, 0x60},
    {REG_DIR8, 0x20},
};
numbers num2[] = {
    {REG_DIR1, 0x00},
    {REG_DIR2, 0x70},
    {REG_DIR3, 0x40},
    {REG_DIR4, 0x20},
    {REG_DIR5, 0x10},
    {REG_DIR6, 0x10},
    {REG_DIR7, 0x50},
    {REG_DIR8, 0x20},
};
numbers num3[] = {
    {REG_DIR1, 0x00},
    {REG_DIR2, 0x20},
    {REG_DIR3, 0x50},
    {REG_DIR4, 0x10},
    {REG_DIR5, 0x20},
    {REG_DIR6, 0x10},
    {REG_DIR7, 0x50},
    {REG_DIR8, 0x20},
};
numbers num4[] = {
    {REG_DIR1, 0x00},
    {REG_DIR2, 0x10},
    {REG_DIR3, 0x10},
    {REG_DIR4, 0x10},
    {REG_DIR5, 0x70},
    {REG_DIR6, 0x50},
    {REG_DIR7, 0x50},
    {REG_DIR8, 0x50},
};
numbers num5[] = {
    {REG_DIR1, 0x00},
    {REG_DIR2, 0x70},
    {REG_DIR3, 0x50},
    {REG_DIR4, 0x10},
    {REG_DIR5, 0x70},
    {REG_DIR6, 0x40},
    {REG_DIR7, 0x50},
    {REG_DIR8, 0x70},
};
numbers num6[] = {
    {REG_DIR1, 0x00},
    {REG_DIR2, 0x20},
    {REG_DIR3, 0x50},
    {REG_DIR4, 0x50},
    {REG_DIR5, 0x60},
    {REG_DIR6, 0x40},
    {REG_DIR7, 0x20},
    {REG_DIR8, 0x10},
};
numbers num7[] = {
    {REG_DIR1, 0x00},
    {REG_DIR2, 0x10},
    {REG_DIR3, 0x10},
    {REG_DIR4, 0x10},
    {REG_DIR5, 0x38},
    {REG_DIR6, 0x10},
    {REG_DIR7, 0x10},
    {REG_DIR8, 0x70},
};
numbers num8[] = {
    {REG_DIR1, 0x00},
    {REG_DIR2, 0x20},
    {REG_DIR3, 0x50},
    {REG_DIR4, 0x50},
    {REG_DIR5, 0x20},
    {REG_DIR6, 0x50},
    {REG_DIR7, 0x50},
    {REG_DIR8, 0x20},
};
numbers num9[] = {
    {REG_DIR1, 0x00},
    {REG_DIR2, 0x10},
    {REG_DIR3, 0x10},
    {REG_DIR4, 0x10},
    {REG_DIR5, 0x70},
    {REG_DIR6, 0x50},
    {REG_DIR7, 0x50},
    {REG_DIR8, 0x20},
};
//array for integers 0 to 9
numbers *num_array[10] = {num0, num1, num2, num3, num4, num5, num6, num7, num8, num9};

int initI2cBus(char *bus, int address)
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

void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
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
    numbers *num = (numbers *)malloc(sizeof(numbers) * 8);
    for (int i = 0; i < 8; i++)
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
        for (int i = 0; i < 8; i++)
        {
            writeI2cReg(i2cFileDesc, num_array[0][i].address_row, 0x00);
        }
        return;
    }

    digit1 = (int)value;
    digit2 = ((value - digit1) * 10) + 0.1;

    // merge the digits into a single number array with shift register values
    numbers *num = (numbers *)malloc(sizeof(numbers) * 8);
    for (int i = 0; i < 8; i++)
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
