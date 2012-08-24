#include "../lib/global.h"
#include "test.h"
#include "unit_tests.h"

uint16_t tests_run = 0;
uint16_t tests_failed = 0;
uint32_t _cycle_cnt = 0;
uint32_t _cycle_start = 0;

void UartTests(void)
{
    printf("\nUart Suite\n");
    TEST_RUN(test_RxBufferEnqueue);
    TEST_RUN(test_RxBufferDequeue);
}

void AllTests(void)
{
    UartTests();
}

int main()
{
    printf("\n~~~~UNIT TESTING~~~~\n");
    AllTests();
    printf("\nTests run: %d\nTests Failed: %d\n", tests_run, tests_failed);
    if (!tests_failed)
    {
        printf("ALL TESTS PASSED\n");
    }
    return tests_failed;
}
