
#include <string.h>
#include <stdio.h>
#include "sapi.h"

int
main(int argc, char *argv[])
{
    // config openocd
    // D:\programas_instalados\openocd\bin\openocd.exe
    // -f D:\documentos\ingenieria\uba\tesis\sapi\ftdi_lpc4337.cfg
    // D:\documentos\ingenieria\uba\tesis\ciaa\ide\cygwin\usr\arm-none-eabi\bin\arm-none-eabi-gdb.exe
    // set mem inaccessible-by-default off

    boardConfig();

    while(1) {

        /* Prendo el led azul */
        gpioWrite( LED2, ON );

        delay(500);

        /* Apago el led azul */
        gpioWrite( LED2, OFF );

        delay(500);

    }

    return 0;
}