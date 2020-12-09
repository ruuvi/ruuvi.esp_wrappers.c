#ifndef TEST_EVENTS_H
#define TEST_EVENTS_H

typedef enum TestEventType_Tag
{
    TestEventType_Signal,
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

    TestEventSignal(os_signal_num_e sig_num)
        : TestEvent(TestEventType_Signal)
        , sig_num(sig_num)
    {
    }
};

#endif // TEST_EVENTS_H
