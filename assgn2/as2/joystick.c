#include "joystick.h"

int getVoltageReading(char n)
{
    // Open file
    int a2dReading = 0;
    //the file for the x axis of the voltage 
    if (n == 'x' || n == 'X')
    {
        FILE *f = fopen(A2D_FILE_VOLTAGE2, "r");
        if (!f)
        {
            printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
            printf(" Check /boot/uEnv.txt for correct options.\n");
            exit(-1);
        }
        a2dReading = 0;
        int itemsRead = fscanf(f, "%d", &a2dReading);
        if (itemsRead <= 0)
        {
            printf("ERROR: Unable to read values from voltage input file.\n");
            exit(-1);
        }
        // Close file
        fclose(f);
    }
    else if (n == 'Y' || n == 'y')//the file for the y axis of the voltage 
    {
        FILE *f = fopen(A2D_FILE_VOLTAGE3, "r");
        if (!f)
        {
            printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
            printf(" Check /boot/uEnv.txt for correct options.\n");
            exit(-1);
        }
        a2dReading = 0;
        int itemsRead = fscanf(f, "%d", &a2dReading);
        if (itemsRead <= 0)
        {
            printf("ERROR: Unable to read values from voltage input file.\n");
            exit(-1);
        }
        // Close file
        fclose(f);
    }

    return a2dReading;
}

int getJoystickDirection(double x, double y)
{
    if (x < 0.45)
    {
        //UP
        return 1;
    }
    else if (x > 1.35)
    {
        //DOWN
        return 2;
    }
    else if (y < 0.45)
    {
        //LEFT
        return 4;
    }
    else if (y > 1.35)
    {
        //RIGHT
        return 3;
    }
    else
    {
        //CENTER
        return 5;
    }
}