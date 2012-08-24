/**
@file main.c
@brief The main entry point for the code
@author Joe Brown
*/
#include "msplib.h"

void HardwareInit(void)
{
    IO_AUX_FUNCTION(UART_TX,SPECIAL);
    IO_AUX_FUNCTION(UART_RX,SPECIAL);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                            __  ___        _
//                           /  |/  /____ _ (_)____
//                          / /|_/ // __ `// // __ \
//                         / /  / // /_/ // // / / /
//                        /_/  /_/ \__,_//_//_/ /_/
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void main(void)
{
#ifndef NON_BLOCKING_UART_RX
    #error "Define NON_BLOCKING_UART_RX in hardware.h for this example"
#endif

    WD_STOP();
    HardwareInit();
    ClockConfig(16);
    // init uart at 115k
    UartInit(115200);

    _EINT();

    while(1)
    {
        printf("\n\nEnter your name please: ");
        uint8_t buf[10];
        memset(buf,0,sizeof(buf));
        uint8_t* cur_char = &buf[0];
        uint8_t poll_complete = FALSE;
        while (poll_complete == FALSE)
        {
            // poll the rx buffer for new data
            if (!UartBufEmpty())
            {
                // pull the data out one byte at a time
                *cur_char++ = getchar();
                // check length
                if (cur_char - &buf[0] > sizeof(buf))
                {
                    printf("\nYour name is too long!");
                    poll_complete = TRUE;
                }
                // was the last character a carriage return?
                else if (*(cur_char - 1) == '\r')
                {
                    printf("\nHello %s",buf);
                    poll_complete = TRUE;
                }
            }
        }
        DelayDumb(100000);
    }
}
