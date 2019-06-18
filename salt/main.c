
#include <string.h>
#include <stdio.h>
#include "sapi.h"
#include "ledPanel.h"

int
main(int argc, char *argv[])
{
    // config openocd
    // D:\programas_instalados\openocd\bin\openocd.exe
    // -f D:\documentos\ingenieria\uba\tesis\salt-firmware\ftdi_lpc4337.cfg
    // D:\documentos\ingenieria\uba\tesis\ciaa\ide\cygwin\usr\arm-none-eabi\bin\arm-none-eabi-gdb.exe
    // set mem inaccessible-by-default off

    boardConfig();
    led_panel_init();

    LedPanelCfg testConfig;
    testConfig.digit0 = 1,
    testConfig.digit1 = 2;
    testConfig.digit2 = 3;
    testConfig.digit3 = 8;

    led_panel_set_cfg(&testConfig);

    while(1) {

        /* Prendo el led azul */
        gpioWrite( LEDB, ON );

        delay(500);

        /* Apago el led azul */
        gpioWrite( LEDB, OFF );

        delay(500);

    }

    return 0;
}