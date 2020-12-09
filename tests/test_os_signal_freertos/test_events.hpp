#ifndef TEST_EVENTS_H
#define TEST_EVENTS_H

typedef enum TestEventType_Tag
{
    TestEventType_Signal,
    TestEventType_ThreadExit,
} TestEventType_e;

/*** Test-Events classes implementation
 * *******************************************************************************/

class TestEvent
{
public:
    TestEventType_e eventType;

    explicit TestEvent(const TestEventType_e eventType)
        : eventType(eventType)
    {
    }
};

class TestEventSignal : public TestEvent
{
public:
    os_signal_num_e sig_num;

    explicit TestEventSignal(os_signal_num_e sig_num)
        : TestEvent(TestEventType_Signal)
        , sig_num(sig_num)
    {
    }
};

class TestEventThreadExit : public TestEvent
{
public:
    int thread_num;

    explicit TestEventThreadExit(int thread_num)
        : TestEvent(TestEventType_ThreadExit)
        , thread_num(thread_num)
    {
    }
};

#endif // TEST_EVENTS_H
