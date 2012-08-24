/**
@file uart.c
@brief Blocking and non-blocking UART interface
@author Joe Brown
*/
#include "global.h"
#include "uart.h"
#include "hardware.h"

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                            ____        _  __
//                           /  _/____   (_)/ /_
//                           / / / __ \ / // __/
//                         _/ / / / / // // /_
//                        /___//_/ /_//_/ \__/
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/** @brief baud rate lookup table */
static const BaudRateConfig rate_table[] =
{
    // see userguide table 18-5
    {  9600,  8000000,  52, 0x49,  1},
    { 19200,  8000000,  26, 0xB6,  0},
    { 38400,  8000000,  13, 0x84,  0},
    { 57600,  8000000,   8, 0xF7, 10},
    {115200,  8000000,   4, 0x55,  5},
    {  9600, 16000000, 104, 0xD6,  2},
    { 19200, 16000000,  52, 0x49,  1},
    { 38400, 16000000,  26, 0xB6,  0},
    { 57600, 16000000,  17, 0xDD,  5},
    {115200, 16000000,   8, 0xF7, 10},
    {  9600, 20000000, 104, 0x25,  3},
    { 19200, 20000000,  52, 0xD6,  1},
    { 38400, 20000000,  26, 0xEE,  8},
    { 57600, 20000000,  17, 0x22, 11},
    {115200, 20000000,   8, 0xAD, 13},
    {  9600, 24000000, 156, 0x00,  4},
    { 19200, 24000000,  78, 0x00,  2},
    { 38400, 24000000,  39, 0x00,  1},
    { 57600, 24000000,  26, 0xD6,  0},
    {115200, 24000000,  13, 0x49,  0}
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void UartInit(uint32_t baud_rate)
{
    uint8_t i = 0;
    uint8_t index = 0;
    // find where the baud rate is in the table
    for(i = 0;i < sizeof(rate_table) / sizeof(BaudRateConfig);i++)
    {
        if (baud_rate == rate_table[i].baud &&
            g_clock_speed == rate_table[i].clock)
        {
            index = i;
            break;
        }
    }

    // SMCLK sources uart
    UCA0CTL1 |= UCSSEL_2;
    // set the divisor
    UCA0BR0 = rate_table[index].ucbr;
    // we always use modulation and never go below 9600 baud which means BR will
    // always be < 256
    UCA0BR1 = 0;
    // UCBRS is at bits 1-3
    UCA0MCTL = rate_table[index].ucbrs << UCBRS_OFFSET;
    // UCBRF is at bits 4-7
    UCA0MCTL |= rate_table[index].ucbrf << USBRF_OFFSET;
    // set UCOS16 to enable oversampling
    UCA0MCTL |= UCOS16;

    // Initialize USCI state machine
    UCA0CTL1 &= ~UCSWRST;
#ifdef NON_BLOCKING_UART_RX
    UART_INT_ENABLE |= UCA0RXIE;
#endif
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                ____                     _
//               / __ \ ___   _____ ___   (_)_   __ ___
//              / /_/ // _ \ / ___// _ \ / /| | / // _ \
//             / _, _//  __// /__ /  __// / | |/ //  __/
//            /_/ |_| \___/ \___/ \___//_/  |___/ \___/
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#ifdef NON_BLOCKING_UART_RX
/** @brief start index for rx circular buffer */
static uint16_t rx_start = 0;
/** @brief size of rx circular buffer */
static uint16_t rx_size  = 0;
/** @brief rx buffer */
static uint8_t rx_buf[MAX_UART_RX_BUF_CNT];

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
int8_t RxBufferEnqueue(uint8_t data)
{
    // is the buffer full?
    if (rx_size == MAX_UART_RX_BUF_CNT)
    {
        goto error;
    }

    // stuff the data
    if((rx_start + rx_size) >= MAX_UART_RX_BUF_CNT)
    {
        rx_buf[(rx_start + rx_size) - MAX_UART_RX_BUF_CNT] = data;
    }
    else
    {
        rx_buf[(rx_start + rx_size)] = data;
    }

    // adjust the buf size
    rx_size++;

    return (SUCCESS);
error:
    return (FAILURE);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
uint8_t RxBufferDequeue(void)
{
    _DINT();
    uint8_t ret = 0;
    // is the buffer empty?
    if (rx_size)
    {
        // return data
        ret = rx_buf[rx_start];
        // adjust the start
        rx_start = (rx_start == (MAX_UART_RX_BUF_CNT - 1)) ? 0 : rx_start + 1;
        // adjust the size
        rx_size--;
    }
    _EINT();
    return ret;
}
#endif // NON_BLOCKING_UART_RX

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//        ____        __                                   __
//       /  _/____   / /_ ___   _____ _____ __  __ ____   / /_ _____
//       / / / __ \ / __// _ \ / ___// ___// / / // __ \ / __// ___/
//     _/ / / / / // /_ /  __// /   / /   / /_/ // /_/ // /_ (__  )
//    /___//_/ /_/ \__/ \___//_/   /_/    \__,_// .___/ \__//____/
//                                             /_/
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#if defined NON_BLOCKING_UART_RX
#pragma vector=UART_INT_VECTOR
__interrupt void UartInt(void)
{
    if (UCA0IV & UCTXIFG)
    {
        RxBufferEnqueue(UCA0RXBUF);
    }
}
#endif

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//            ____        __               ____
//           /  _/____   / /_ ___   _____ / __/____ _ _____ ___
//           / / / __ \ / __// _ \ / ___// /_ / __ `// ___// _ \
//         _/ / / / / // /_ /  __// /   / __// /_/ // /__ /  __/
//        /___//_/ /_/ \__/ \___//_/   /_/   \__,_/ \___/ \___/
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
uint8_t UartGetC(void)
{
#ifdef NON_BLOCKING_UART_RX
    return RxBufferDequeue();
#else
    while (!(UART_INT_FLAG & UCRXIFG));    // RX a byte?
    return UART_RX_BUF;
#endif
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void UartPutC(uint8_t data)
{
    while (!(UART_INT_FLAG & UCTXIFG));    // TX buffer ready?
    UCA0TXBUF = data;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#ifdef NON_BLOCKING_UART_RX
uint8_t UartBufEmpty(void)
{
    return !(rx_size);
}
#endif

#ifndef TEST
// Use small printf formatter in IAR
int putchar(int c)
{
    UartPutC(c);
    return c;
}

int getchar(void)
{
    return UartGetC();
}
#endif
