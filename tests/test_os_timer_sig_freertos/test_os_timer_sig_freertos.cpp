/**
 * @file test_os_sema.cpp
 * @author TheSomeMan
 * @date 2020-12-07
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include <string>
#include <sys/time.h>
#include <semaphore.h>
#include "gtest/gtest.h"
#include "os_timer_sig.h"
#include "os_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "TQueue.hpp"
#include "esp_log_wrapper.hpp"
#include "test_events.hpp"

using namespace std;

typedef enum MainTaskCmd_Tag
{
    MainTaskCmd_Exit,
    MainTaskCmd_RunSignalHandlerTask1,
    MainTaskCmd_TimerSigPeriodicCreate,
    MainTaskCmd_TimerSigPeriodicCreateStatic,
    MainTaskCmd_TimerSigPeriodicStart,
    MainTaskCmd_TimerSigPeriodicRestart200,
    MainTaskCmd_TimerSigPeriodicRestart500,
    MainTaskCmd_TimerSigPeriodicStop,
    MainTaskCmd_TimerSigPeriodicDelete,
    MainTaskCmd_TimerSigPeriodicSimulate,
    MainTaskCmd_TimerSigOneShotCreate,
    MainTaskCmd_TimerSigOneShotCreateStatic,
    MainTaskCmd_TimerSigOneShotStart,
    MainTaskCmd_TimerSigOneShotRestart,
    MainTaskCmd_TimerSigOneShotStop,
    MainTaskCmd_TimerSigOneShotDelete,
    MainTaskCmd_TimerSigOneShotSimulate,
} MainTaskCmd_e;

/*** Google-test class implementation
 * *********************************************************************************/

class TestOsTimerSigFreertos;
static TestOsTimerSigFreertos* g_pTestClass;

static void*
freertosStartup(void* arg);

class TestOsTimerSigFreertos : public ::testing::Test
{
private:
protected:
    void
    SetUp() override
    {
        esp_log_wrapper_init();
        sem_init(&semaFreeRTOS, 0, 0);
        pid_test      = pthread_self();
        const int err = pthread_create(&pid_freertos, nullptr, &freertosStartup, this);
        assert(0 == err);
        while (0 != sem_wait(&semaFreeRTOS))
        {
        }
    }

    void
    TearDown() override
    {
        cmdQueue.push_and_wait(MainTaskCmd_Exit);
        vTaskEndScheduler();
        void* ret_code = nullptr;
        pthread_join(pid_freertos, &ret_code);
        sem_destroy(&semaFreeRTOS);
        esp_log_wrapper_deinit();
        g_pTestClass = nullptr;
    }

public:
    pthread_t                      pid_test;
    pthread_t                      pid_freertos;
    sem_t                          semaFreeRTOS;
    TQueue<MainTaskCmd_e>          cmdQueue;
    os_timer_sig_periodic_static_t timer_sig_periodic_mem;
    os_timer_sig_one_shot_static_t timer_sig_one_shot_mem;
    bool                           result_run_signal_handler_task;
    os_signal_t*                   p_signal;
    os_timer_sig_periodic_t*       p_timer_sig_periodic;
    os_timer_sig_one_shot_t*       p_timer_sig_one_shot;
    uint32_t                       counter0;
    uint32_t                       counter1;

    TestOsTimerSigFreertos();

    ~TestOsTimerSigFreertos() override;

    bool
    wait_until_thread_registered(const uint32_t timeout_ms) const;

    static void
    delay_ms(const uint32_t timeout_ms);
};

TestOsTimerSigFreertos::TestOsTimerSigFreertos()
    : Test()
    , pid_test(0)
    , pid_freertos(0)
    , semaFreeRTOS({})
    , timer_sig_periodic_mem({})
    , timer_sig_one_shot_mem({})
    , result_run_signal_handler_task(false)
    , p_signal(nullptr)
    , p_timer_sig_periodic(nullptr)
    , p_timer_sig_one_shot(nullptr)
    , counter0(0)
    , counter1(0)
{
    g_pTestClass = this;
}

TestOsTimerSigFreertos::~TestOsTimerSigFreertos()
{
    g_pTestClass = nullptr;
}

extern "C" {

static struct timespec
timespec_get_clock_monotonic(void)
{
    struct timespec timestamp = {};
    clock_gettime(CLOCK_MONOTONIC, &timestamp);
    return timestamp;
}

uint32_t
get_uptime_ms(void)
{
    static uint32_t g_base_time;
    struct timespec ts       = timespec_get_clock_monotonic();
    const uint32_t  delta_ms = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    if (0 == g_base_time)
    {
        g_base_time = delta_ms;
    }
    return delta_ms - g_base_time;
}

static struct timespec
timespec_diff(const struct timespec* p_t2, const struct timespec* p_t1)
{
    struct timespec result = {
        .tv_sec  = p_t2->tv_sec - p_t1->tv_sec,
        .tv_nsec = p_t2->tv_nsec - p_t1->tv_nsec,
    };
    if (result.tv_nsec < 0)
    {
        result.tv_sec -= 1;
        result.tv_nsec += 1000000000;
    }
    return result;
}

static uint32_t
timespec_diff_ms(const struct timespec* p_t2, const struct timespec* p_t1)
{
    struct timespec diff = timespec_diff(p_t2, p_t1);
    return diff.tv_sec * 1000 + diff.tv_nsec / 1000000;
}

static void
sleep_ms_unchecked(uint32_t msec)
{
    struct timespec ts = {
        .tv_sec  = msec / 1000,
        .tv_nsec = (msec % 1000) * 1000000,
    };

    int res = 0;
    do
    {
        res = nanosleep(&ts, &ts);
    } while ((0 != res) && (EINTR == errno));
}

static void
sleep_ms(uint32_t msec)
{
    disableCheckingIfCurThreadIsFreeRTOS();
    const TickType_t tick_start = xTaskGetTickCount();
    enableCheckingIfCurThreadIsFreeRTOS();
    while (true)
    {
        disableCheckingIfCurThreadIsFreeRTOS();
        const TickType_t delta_ticks = xTaskGetTickCount() - tick_start;
        enableCheckingIfCurThreadIsFreeRTOS();
        if (delta_ticks >= pdMS_TO_TICKS(msec))
        {
            break;
        }
        const uint32_t remain_msec = (pdMS_TO_TICKS(msec) - delta_ticks) * 1000 / configTICK_RATE_HZ;
        sleep_ms_unchecked(remain_msec);
    }
}

void
os_task_delay(const os_delta_ticks_t delay_ticks)
{
    vTaskDelay(delay_ticks);
}

void
tdd_assert_trap(void)
{
    assert(0);
}

static volatile int32_t g_flagDisableCheckIsThreadFreeRTOS;

void
disableCheckingIfCurThreadIsFreeRTOS(void)
{
    ++g_flagDisableCheckIsThreadFreeRTOS;
}

void
enableCheckingIfCurThreadIsFreeRTOS(void)
{
    --g_flagDisableCheckIsThreadFreeRTOS;
    assert(g_flagDisableCheckIsThreadFreeRTOS >= 0);
}

int
checkIfCurThreadIsFreeRTOS(void)
{
    if (nullptr == g_pTestClass)
    {
        return false;
    }
    if (g_flagDisableCheckIsThreadFreeRTOS)
    {
        return true;
    }
    const pthread_t cur_thread_pid = pthread_self();
    if (cur_thread_pid == g_pTestClass->pid_test)
    {
        return false;
    }
    return true;
}

} // extern "C"

bool
TestOsTimerSigFreertos::wait_until_thread_registered(const uint32_t timeout_ms) const
{
    struct timespec t1 = timespec_get_clock_monotonic();
    struct timespec t2 = t1;
    while (timespec_diff_ms(&t2, &t1) < timeout_ms)
    {
        if (nullptr != this->p_signal)
        {
            if (os_signal_is_any_thread_registered(this->p_signal))
            {
                return true;
            }
        }
        sleep(1);
        t2 = timespec_get_clock_monotonic();
    }
    return false;
}

void
TestOsTimerSigFreertos::delay_ms(const uint32_t timeout_ms)
{
    struct timespec t1 = timespec_get_clock_monotonic();
    struct timespec t2 = t1;
    while (timespec_diff_ms(&t2, &t1) < timeout_ms)
    {
        sleep_ms(1);
        t2 = timespec_get_clock_monotonic();
    }
}

ATTR_NORETURN
static void
signalHandlerTask1(void* p_param)
{
    auto* pObj     = static_cast<TestOsTimerSigFreertos*>(p_param);
    pObj->p_signal = os_signal_create();
    assert(nullptr != pObj->p_signal);
    if (!os_signal_add(pObj->p_signal, OS_SIGNAL_NUM_0))
    {
        assert(0);
    }
    if (!os_signal_add(pObj->p_signal, OS_SIGNAL_NUM_1))
    {
        assert(0);
    }
    os_signal_register_cur_thread(pObj->p_signal);
    for (;;)
    {
        os_signal_events_t sig_events = {};
        os_signal_wait(pObj->p_signal, &sig_events);
        for (;;)
        {
            os_signal_num_e sig_num = os_signal_num_get_next(&sig_events);
            if (OS_SIGNAL_NUM_NONE == sig_num)
            {
                break;
            }
            if (OS_SIGNAL_NUM_0 == sig_num)
            {
                pObj->counter0 += 1;
            }
            else if (OS_SIGNAL_NUM_1 == sig_num)
            {
                pObj->counter1 += 1;
            }
        }
    }
}

static void
cmdHandlerTask(void* p_param)
{
    auto* pObj     = static_cast<TestOsTimerSigFreertos*>(p_param);
    bool  flagExit = false;
    sem_post(&pObj->semaFreeRTOS);
    while (!flagExit)
    {
        const MainTaskCmd_e cmd = pObj->cmdQueue.pop();
        switch (cmd)
        {
            case MainTaskCmd_Exit:
                flagExit = true;
                break;
            case MainTaskCmd_RunSignalHandlerTask1:
            {
                os_task_handle_t h_task              = nullptr;
                pObj->result_run_signal_handler_task = os_task_create(
                    &signalHandlerTask1,
                    "SignalHandler",
                    configMINIMAL_STACK_SIZE,
                    pObj,
                    tskIDLE_PRIORITY + 1,
                    &h_task);
                break;
            }
            case MainTaskCmd_TimerSigPeriodicCreate:
                pObj->p_timer_sig_periodic = os_timer_sig_periodic_create(
                    "timer_periodic",
                    pObj->p_signal,
                    OS_SIGNAL_NUM_1,
                    100);
                break;
            case MainTaskCmd_TimerSigPeriodicCreateStatic:
                pObj->p_timer_sig_periodic = os_timer_sig_periodic_create_static(
                    &pObj->timer_sig_periodic_mem,
                    "timer_periodic",
                    pObj->p_signal,
                    OS_SIGNAL_NUM_1,
                    pdMS_TO_TICKS(100));
                break;
            case MainTaskCmd_TimerSigPeriodicStart:
                os_timer_sig_periodic_start(pObj->p_timer_sig_periodic);
                break;
            case MainTaskCmd_TimerSigPeriodicRestart200:
                os_timer_sig_periodic_restart_with_period(pObj->p_timer_sig_periodic, pdMS_TO_TICKS(200), false);
                break;
            case MainTaskCmd_TimerSigPeriodicRestart500:
                os_timer_sig_periodic_restart_with_period(pObj->p_timer_sig_periodic, pdMS_TO_TICKS(500), false);
                break;
            case MainTaskCmd_TimerSigPeriodicStop:
                os_timer_sig_periodic_stop(pObj->p_timer_sig_periodic);
                break;
            case MainTaskCmd_TimerSigPeriodicDelete:
                os_timer_sig_periodic_delete(&pObj->p_timer_sig_periodic);
                break;
            case MainTaskCmd_TimerSigPeriodicSimulate:
                os_timer_sig_periodic_simulate(pObj->p_timer_sig_periodic);
                break;
            case MainTaskCmd_TimerSigOneShotCreate:
                pObj->p_timer_sig_one_shot = os_timer_sig_one_shot_create(
                    "timer_one_shot",
                    pObj->p_signal,
                    OS_SIGNAL_NUM_0,
                    pdMS_TO_TICKS(100));
                break;
            case MainTaskCmd_TimerSigOneShotCreateStatic:
                pObj->p_timer_sig_one_shot = os_timer_sig_one_shot_create_static(
                    &pObj->timer_sig_one_shot_mem,
                    "timer_one_shot",
                    pObj->p_signal,
                    OS_SIGNAL_NUM_0,
                    pdMS_TO_TICKS(100));
                break;
            case MainTaskCmd_TimerSigOneShotStart:
                os_timer_sig_one_shot_start(pObj->p_timer_sig_one_shot);
                break;
            case MainTaskCmd_TimerSigOneShotRestart:
                os_timer_sig_one_shot_restart_with_period(pObj->p_timer_sig_one_shot, pdMS_TO_TICKS(200), false);
                break;
            case MainTaskCmd_TimerSigOneShotStop:
                os_timer_sig_one_shot_stop(pObj->p_timer_sig_one_shot);
                break;
            case MainTaskCmd_TimerSigOneShotDelete:
                os_timer_sig_one_shot_delete(&pObj->p_timer_sig_one_shot);
                break;
            case MainTaskCmd_TimerSigOneShotSimulate:
                os_timer_sig_one_shot_simulate(pObj->p_timer_sig_one_shot);
                break;
            default:
                exit(1);
                break;
        }
        pObj->cmdQueue.notify_handled();
    }
    vTaskDelete(nullptr);
}

static void*
freertosStartup(void* arg)
{
    auto* pObj = static_cast<TestOsTimerSigFreertos*>(arg);
    disableCheckingIfCurThreadIsFreeRTOS();
    const bool res
        = xTaskCreate(&cmdHandlerTask, "cmdHandlerTask", configMINIMAL_STACK_SIZE, pObj, tskIDLE_PRIORITY + 1, nullptr);
    assert(res);
    vTaskStartScheduler();
    return nullptr;
}

/*** Unit-Tests
 * *******************************************************************************************************/

TEST_F(TestOsTimerSigFreertos, test1) // NOLINT
{
    cmdQueue.push_and_wait(MainTaskCmd_RunSignalHandlerTask1);
    ASSERT_TRUE(this->result_run_signal_handler_task);
    ASSERT_TRUE(wait_until_thread_registered(1000));

#if 1
    //
    // Test os_timer_sig_one_shot
    //

    this->counter0 = 0;
    this->counter1 = 0;
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotCreate);
    ASSERT_NE(nullptr, this->p_timer_sig_one_shot);
    sleep_ms(200);
    ASSERT_EQ(0, this->counter0);
    ASSERT_EQ(0, this->counter1);
    ASSERT_FALSE(os_timer_sig_one_shot_is_active(this->p_timer_sig_one_shot));

    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotStart);
    ASSERT_TRUE(os_timer_sig_one_shot_is_active(this->p_timer_sig_one_shot));
    sleep_ms(250);
    ASSERT_EQ(1, this->counter0);
    ASSERT_FALSE(os_timer_sig_one_shot_is_active(this->p_timer_sig_one_shot));

    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotStart);
    sleep_ms(150);
    ASSERT_EQ(2, this->counter0);

    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotStart);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotStop);
    sleep_ms(150);
    ASSERT_EQ(2, this->counter0);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotSimulate);
    sleep_ms(100);
    ASSERT_EQ(2, this->counter0);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotStart);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotSimulate);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotStop);
    sleep_ms(100);
    ASSERT_EQ(3, this->counter0);

    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotStart);
    sleep_ms(50);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotStop);
    sleep_ms(150);
    ASSERT_EQ(3, this->counter0);

    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotStart);
    sleep_ms(50);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotRestart);
    sleep_ms(150);
    ASSERT_EQ(3, this->counter0);
    sleep_ms(100);
    ASSERT_EQ(4, this->counter0);

    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotDelete);
    ASSERT_EQ(nullptr, this->p_timer_sig_one_shot);
#endif

#if 1
    //
    // Test os_timer_sig_one_shot_static
    //

    this->counter0 = 0;
    this->counter1 = 0;
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotCreateStatic);
    ASSERT_NE(nullptr, this->p_timer_sig_one_shot);
    sleep_ms(200);
    ASSERT_EQ(0, this->counter0);
    ASSERT_EQ(0, this->counter1);
    ASSERT_FALSE(os_timer_sig_one_shot_is_active(this->p_timer_sig_one_shot));

    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotStart);
    ASSERT_TRUE(os_timer_sig_one_shot_is_active(this->p_timer_sig_one_shot));
    sleep_ms(250);
    ASSERT_EQ(1, this->counter0);
    ASSERT_FALSE(os_timer_sig_one_shot_is_active(this->p_timer_sig_one_shot));

    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotStart);
    sleep_ms(150);
    ASSERT_EQ(2, this->counter0);

    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotStart);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotStop);
    sleep_ms(150);
    ASSERT_EQ(2, this->counter0);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotSimulate);
    sleep_ms(100);
    ASSERT_EQ(2, this->counter0);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotStart);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotSimulate);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotStop);
    sleep_ms(100);
    ASSERT_EQ(3, this->counter0);

    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotStart);
    sleep_ms(50);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotStop);
    sleep_ms(150);
    ASSERT_EQ(3, this->counter0);

    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotStart);
    sleep_ms(50);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotRestart);
    sleep_ms(150);
    ASSERT_EQ(3, this->counter0);
    sleep_ms(100);
    ASSERT_EQ(4, this->counter0);

    cmdQueue.push_and_wait(MainTaskCmd_TimerSigOneShotDelete);
    ASSERT_EQ(nullptr, this->p_timer_sig_one_shot);
#endif

#if 1
    //
    // Test os_timer_sig_periodic
    //

    this->counter0 = 0;
    this->counter1 = 0;
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicCreate);
    ASSERT_NE(nullptr, this->p_timer_sig_periodic);
    sleep_ms(250);
    ASSERT_EQ(0, this->counter1);
    ASSERT_FALSE(os_timer_sig_periodic_is_active(this->p_timer_sig_periodic));

    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicStart);
    ASSERT_TRUE(os_timer_sig_periodic_is_active(this->p_timer_sig_periodic));
    sleep_ms(250);
    ASSERT_EQ(2, this->counter1);
    ASSERT_TRUE(os_timer_sig_periodic_is_active(this->p_timer_sig_periodic));

    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicStop);
    sleep_ms(150);
    ASSERT_EQ(2, this->counter1);

    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicStart);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicStop);
    sleep_ms(150);
    ASSERT_EQ(2, this->counter1);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicSimulate);
    sleep_ms(100);
    ASSERT_EQ(2, this->counter1);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicStart);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicSimulate);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicStop);
    sleep_ms(100);
    ASSERT_EQ(3, this->counter1);

    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicStart);
    sleep_ms(50);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicStop);
    sleep_ms(150);
    ASSERT_EQ(3, this->counter1);

    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicStart);
    // cur tick: 0, next timer at 100
    sleep_ms(120);
    // cur tick: 120, next timer at 200
    ASSERT_EQ(4, this->counter1);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicRestart200);
    // cur tick: 120, next timer at 300
    sleep_ms(150);
    // cur tick: 270, next timer at 300
    ASSERT_EQ(4, this->counter1);
    sleep_ms(70);
    // cur tick: 340, next timer at 500
    ASSERT_EQ(5, this->counter1);
    sleep_ms(200);
    // cur tick: 540, next timer at 700
    ASSERT_EQ(6, this->counter1);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicStop);

    // cur tick: 0
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicRestart200);
    // cur tick: 0, next timer at 200
    sleep_ms(180);
    // cur tick: 180, next timer at 200
    ASSERT_EQ(6, this->counter1);
    sleep_ms(50);
    // cur tick: 230, next timer at 400
    ASSERT_EQ(7, this->counter1);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicStop);
    sleep_ms(100);
    ASSERT_EQ(7, this->counter1);

    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicStart);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicRestart500);
    // cur tick: 0, next timer at 500
    sleep_ms(400);
    // cur tick: 400, next timer at 500
    ASSERT_EQ(7, this->counter1);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicRestart200);
    // cur tick: 400, next timer at 600
    sleep_ms(150);
    // cur tick: 550, next timer at 600
    ASSERT_EQ(7, this->counter1);
    sleep_ms(100);
    // cur tick: 750, next timer at 800
    ASSERT_EQ(8, this->counter1);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicStop);

    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicDelete);
    ASSERT_EQ(nullptr, this->p_timer_sig_periodic);
#endif

#if 1
    //
    // Test os_timer_sig_periodic_static
    //

    this->counter0 = 0;
    this->counter1 = 0;
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicCreateStatic);
    ASSERT_NE(nullptr, this->p_timer_sig_periodic);
    sleep_ms(250);
    ASSERT_EQ(0, this->counter1);
    ASSERT_FALSE(os_timer_sig_periodic_is_active(this->p_timer_sig_periodic));

    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicStart);
    ASSERT_TRUE(os_timer_sig_periodic_is_active(this->p_timer_sig_periodic));
    sleep_ms(250);
    ASSERT_EQ(2, this->counter1);
    ASSERT_TRUE(os_timer_sig_periodic_is_active(this->p_timer_sig_periodic));

    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicStop);
    sleep_ms(150);
    ASSERT_EQ(2, this->counter1);

    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicStart);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicStop);
    sleep_ms(150);
    ASSERT_EQ(2, this->counter1);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicSimulate);
    sleep_ms(100);
    ASSERT_EQ(2, this->counter1);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicStart);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicSimulate);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicStop);
    sleep_ms(100);
    ASSERT_EQ(3, this->counter1);

    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicStart);
    sleep_ms(50);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicStop);
    sleep_ms(150);
    ASSERT_EQ(3, this->counter1);

    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicStart);
    sleep_ms(50);
    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicRestart200);
    sleep_ms(150);
    ASSERT_EQ(3, this->counter1);
    sleep_ms(100);
    ASSERT_EQ(4, this->counter1);
    sleep_ms(200);
    ASSERT_EQ(5, this->counter1);

    cmdQueue.push_and_wait(MainTaskCmd_TimerSigPeriodicDelete);
    ASSERT_EQ(nullptr, this->p_timer_sig_periodic);
#endif
}
