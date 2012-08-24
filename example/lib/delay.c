/**
@file delay.c
@brief Simple delay using global timer
@author Joe Brown
*/
#include "global.h"
#include "delay.h"

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void DelayDumb(uint32_t delay_time)
{
    volatile uint32_t cnt = 0;
    while(cnt++ < delay_time);
}
