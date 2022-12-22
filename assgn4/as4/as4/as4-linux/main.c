#include "LEDs.h"
#include "morsecode.h"
#include "binary.h"
#include "sharedMem-linux.h"
#include <stdio.h>
#include <stdbool.h>
#include <time.h>


int main()
{
    runCommand("config-pin p9_27 pruout");
    runCommand("config-pin p9_28 pruin"); 
    disp_init();
    get_user_input(); 

    return 0;
}