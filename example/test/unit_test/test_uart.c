#include "../../lib/global.h"
#include "../../lib/uart.c"
#include "test_uart.h"
#include "../test.h"


void ClearUartRxBuf(void)
{
    rx_start = 0;
    rx_size = 0;
    memset(rx_buf,0,MAX_UART_RX_BUF_CNT);
}

void test_RxBufferEnqueue(void)
{
    uint16_t i = 0;

    // A single byte add at the start of the buffer
    ClearUartRxBuf();
    RxBufferEnqueue(5);
    TEST_ASSERT("Rx buf size incorrect after buffering 1 byte",rx_size != 1);

    // fill the buffer completely and then add 1
    ClearUartRxBuf();
    for (i = 0;i < MAX_UART_RX_BUF_CNT + 1;i++)
    {
        RxBufferEnqueue((uint8_t)i);
    }

    TEST_ASSERT("Rx buf size somehow incremented past the end of the buffer",
                rx_size > MAX_UART_RX_BUF_CNT);

    // Write bytes over the end of the buffer (expected to wrap around)
    ClearUartRxBuf();
    // start one byte before the end
    rx_start = MAX_UART_RX_BUF_CNT - 1;
    // this will be the last byte in the buffer before wrapping
    RxBufferEnqueue(1);
    // this should be at buf[0]
    RxBufferEnqueue(2);
    // this should be at buf[1]
    RxBufferEnqueue(3);
    TEST_ASSERT("Error writing over end of circular buf",
               ((rx_buf[MAX_UART_RX_BUF_CNT - 1] != 1) ||
                (rx_buf[0] != 2) ||
                (rx_buf[1] != 3)));
}

void test_RxBufferDequeue(void)
{
    uint8_t buf[5];
    uint8_t* ptr = buf;

    // Dequeue a single byte from the buffer
    ClearUartRxBuf();
    RxBufferEnqueue(5);
    TEST_ASSERT("Data returned from buffer does not match data enqueued",
                RxBufferDequeue() != 5);

    // Dequeue multiple bytes from the buffer
    ClearUartRxBuf();
    RxBufferEnqueue('1');
    RxBufferEnqueue('2');
    RxBufferEnqueue('3');
    *ptr++ = RxBufferDequeue();
    *ptr++ = RxBufferDequeue();
    *ptr++ = RxBufferDequeue();
    TEST_ASSERT("Data returned from byte buffer does not match data enqueued",
                strncmp((char*)buf,"123",3));

    // dequeue bytes that pass the end of the buffer
    ClearUartRxBuf();
    rx_start = MAX_UART_RX_BUF_CNT - 1;
    RxBufferEnqueue('x');
    RxBufferEnqueue('y');
    TEST_ASSERT("Data returned from end of buffer does not match data enqueued",
                RxBufferDequeue() != 'x');
    TEST_ASSERT("Buffer dequeue caused erroneous change in rx buffer",
                rx_start != 0);
    TEST_ASSERT("Data returned from start of buffer does not match data enqueued",
                RxBufferDequeue() != 'y');

}
