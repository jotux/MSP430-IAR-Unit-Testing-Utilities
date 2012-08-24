/**
@file uart.h
@brief Definitions, data structues and prototypes for UART
@author Joe Brown
*/
#ifndef UART_H
#define UART_H

#define UART_INT_VECTOR     USCI_A0_VECTOR
#define UART_INT_ENABLE     UCA0IE
#define UART_INT_FLAG       UCA0IFG
#define UCA0RXIFG           UCRXIFG
#define UCA0TXIFG           UCTXIFG
#define UCA0MCTL            UCA0MCTLW
#define UCA0TXIE            UCTXIE
#define UCA0RXIE            UCRXIE
#define UCBRS_OFFSET        4
#define USBRF_OFFSET        8

/** @brief Lookup table for uart config parameters */
typedef struct
{
    uint32_t baud;
    uint32_t clock;
    uint8_t  ucbr;
    uint8_t  ucbrs;
    uint8_t  ucbrf;
} BaudRateConfig;

/**
@brief Initialize Uart hardware
@details
Use the lookup table to get baud rate information then configure the uart
hardware for use. Depending on if blocking/non-blocking is selected different
interrupts will also be configured. If you need more baud rate configurations
you can look them up in the userguide.
@param[in] baud_rate baud rate value (9600,19200,38400,57600,115200)
*/
extern void UartInit(uint32_t baud_rate);

/**
@brief Enqueue data into the rx buffer
@details
Take data from the hardware rx buffer and enqueue it in a circular buffer for
retrieval.
@param[in] data data to enqueue
@return SUCCESS if data was enqueued, FAILURE otherwise
*/
static int8_t RxBufferEnqueue(uint8_t data);

/**
@brief Remove data from the rx buffer and place it into the provided address
@details
Dequeue data from the rx buffer and then adjust the start/size values.
@return single byte from the rx buffer
*/
static uint8_t RxBufferDequeue(void);

/**
@brief Interrupt routine for transmit and receive
*/
static __interrupt void UartInt(void);

/**
@brief Read a single byte from the uart
@details
If the uart is configured as non-blocking we will try to get one byte from the
rx buffer. If we are configured as blocking we will wait for a character to be
recieved.
@return single data from rx buffer
*/
extern uint8_t UartGetC(void);

/**
@brief Write a single byte to the uart
@details
If the uart is configured as non-blocking we will enqueue one byte to the tx
buffer. If we are configured as blocking we will stuff the data and wait for the
transmit to complete
@param[in] data data to transmit
*/
extern void UartPutC(uint8_t data);

/**
@brief Check to see if rx buffer is empty
@details
Check rx_size to see if any data is pending
@return 1 if empty, 0 if not empty
*/
extern uint8_t UartBufEmpty(void);

#endif // UART_H
