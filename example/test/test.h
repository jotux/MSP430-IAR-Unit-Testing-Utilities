#define PRINT_PASS()     printf("PASS - %s(%s:%d)",__FUNCTION__,__FILE__,__LINE__)
#define PRINT_FAIL(m)    printf("FAIL - %s(%s:%d)-%s",__FUNCTION__,__FILE__,__LINE__,m)

#define TEST_ASSERT(message, test)  \
    do                              \
    {                               \
        if (test)                   \
        {                           \
            tests_failed++;         \
            PRINT_FAIL(message);    \
        }                           \
        else                        \
        {                           \
            PRINT_PASS();           \
        }                           \
        tests_run++;                \
        printf("|time:%.3f|\n",(float)((_cycle_cnt - _cycle_start))/16000000); \
        _cycle_start = _cycle_cnt;  \
    } while (0)

#define TEST_RUN(test)                          \
    do                                          \
    {                                           \
        _cycle_start = _cycle_cnt;              \
        test();                                 \
    } while (0)

extern uint16_t tests_run;
extern uint16_t tests_failed;
extern uint32_t _cycle_cnt;
extern uint32_t _cycle_start;

#define TEST_SUCCESS 0