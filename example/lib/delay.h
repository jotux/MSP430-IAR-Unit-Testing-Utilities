/**
@file delay.h
@brief Definition of delay
@author Joe Brown
*/
#ifndef DELAY_H
#define DELAY_H

/**
@brief Delay for selected time
@details
Sit in a loop and wait for it to end.
@warning If you call this in an interrupt it will block interrupts until it
completes.
@param[in] delay_time time to delay
*/
extern void DelayDumb(uint32_t delay_time);

#endif // DELAY_H
