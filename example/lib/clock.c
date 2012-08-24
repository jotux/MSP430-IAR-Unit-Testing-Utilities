/**
@file clock.c
@brief Clock config data and functions
@author Joe Brown
*/
#include "global.h"
#include "clock.h"
#include "hardware.h"

// to be externed in global.h so it is accessible to everyone
volatile uint32_t g_clock_speed = 0;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void ClockConfig(uint8_t mhz)
{
    #define FR5739_CLOCK_PASSWORD 0xA5
    // Calibrated DCO values are 5.33,6.67,8,16,20,24
    // 8,16,20,24 are avaiable here
    CSCTL0_H = FR5739_CLOCK_PASSWORD;
    switch(mhz)
    {
        case 8:
        {
            CSCTL1 = DCOFSEL0 + DCOFSEL1;
            break;
        }
        case 16:
        {
            CSCTL1 = DCORSEL;
            break;
        }
        case 20:
        {
            CSCTL1 = DCORSEL + DCOFSEL0;
            break;
        }
        case 24:
        {
            CSCTL1 = DCORSEL + DCOFSEL0 + DCOFSEL1;
            break;
        }
        default:
        {
            while(1)
            {
                // If you're here the clock was configured incorrectly
                _NOP();
            }
        }
    }
    /** @todo Add more control over ACLK, currently only set to XT1 */
    // ACLK source = XT1
    CSCTL2  = SELA__XT1CLK;
    // SMCLK source = DCO
    CSCTL2 |= SELS__DCOCLK;
    // MCLK source = DCO
    CSCTL2 |= SELM__DCOCLK;
    // ACLK divider = 1
    CSCTL3  = DIVA_0;
    // SMCLK divider = 1
    CSCTL3 |= DIVS_0;
    // MCLK divider = 1
    CSCTL3 |= DIVM_0;
    // Drive settings
    CSCTL4 |= XT1DRIVE_3;
    CSCTL4 &= ~XT1OFF;

    // Save the DCO clock frequency
    g_clock_speed = mhz * 1000000;
}
