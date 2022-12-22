#include "binary.h"
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "sharedDataStruct.h"

static bool binary_num[16];
static bool msg_arr[16];

bool *Binary(int character_code, int *size)
{
    *size = 0;
    int i = 0;
    while (character_code > 0)
    {
        binary_num[i++] = character_code % 2;
        character_code /= 2;
    }

    int k = 0; // size of the filled array without zeros at the end
    // prints and stores the morse code in reverse order
    for (int j = i - 1; j >= 0; j--)
    {
        if (!binary_num[j]) // false
        {
            msg_arr[k] = 0;
            (*size)++;
            k++;
        }
        else if (binary_num[j]) // true
        {
            msg_arr[k] = 1;
            (*size)++;
            k++;
        }
        if (binary_num[j - 2] == 0 && binary_num[j - 1] == 0)
        {
            break;
        }
    }
    // array containing the binary code of a single character
    return msg_arr;
}