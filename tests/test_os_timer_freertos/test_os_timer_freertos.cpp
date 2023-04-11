/**
 * @file test_os_sema.cpp
 * @author TheSomeMan
 * @date 2020-12-06
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include <string>
#include <semaphore.h>
#include "gtest/gtest.h"
#include "os_timer.h"
#include "os_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "TQueue.hpp"
#include "esp_log_wrapper.hpp"
#include <sys/time.h>

using namespace std;

typedef enum MainTaskCmd_Tag
{
    MainTaskCmd_Exit,
    MainTaskCmd_TimerPeriodicCreate,
    MainTaskCmd_TimerPeriodicCreateStatic,
    MainTaskCmd_TimerPeriodicStart,
    MainTaskCmd_TimerPeriodicStop,
    MainTaskCmd_TimerPeriodicDelete,
    MainTaskCmd_TimerOneShotCreate,
    MainTaskCmd_TimerOneShotCreateStatic,
    MainTaskCmd_TimerOneShotStart,
    MainTaskCmd_TimerOneShotStop,
    MainTaskCmd_TimerOneShotDelete,
} MainTaskCmd_e;

/*** Google-test class implementation
 * *********************************************************************************/

class TestOsTimerFreertos;
static TestOsTimerFreertos* g_pTestClass;

static void*
freertosStartup(void* arg);

class TestOsTimerFreertos : public ::testing::Test
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
    pthread_t                  pid_test;
    pthread_t                  pid_freertos;
    sem_t                      semaFreeRTOS;
    TQueue<MainTaskCmd_e>      cmdQueue;
    os_timer_periodic_static_t timer_periodic_static;
    os_timer_one_shot_static_t timer_one_shot_static;
    os_timer_periodic_t*       p_timer_periodic;
    os_timer_one_shot_t*       p_timer_one_shot;
    uint32_t                   counter;

    TestOsTimerFreertos();

    ~TestOsTimerFreertos() override;

    static void
    delay_ms(const uint32_t timeout_ms);
};

TestOsTimerFreertos::TestOsTimerFreertos()
    : Test()
    , pid_test(0)
    , pid_freertos(0)
    , semaFreeRTOS({})
    , timer_periodic_static({})
    , timer_one_shot_static({})
    , p_timer_periodic(nullptr)
    , p_timer_one_shot(nullptr)
    , counter(0)
{
    g_pTestClass = this;
}

TestOsTimerFreertos::~TestOsTimerFreertos()
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

void
TestOsTimerFreertos::delay_ms(const uint32_t timeout_ms)
{
    struct timespec t1 = timespec_get_clock_monotonic();
    struct timespec t2 = t1;
    while (timespec_diff_ms(&t2, &t1) < timeout_ms)
    {
        sleep_ms(1);
        t2 = timespec_get_clock_monotonic();
    }
}

static void
timer_callback_periodic(os_timer_periodic_t* p_timer, void* p_arg)
{
    (void)p_timer;
    auto* pObj = static_cast<TestOsTimerFreertos*>(p_arg);
    pObj->counter += 1;
}

static void
timer_callback_one_shot(os_timer_one_shot_t* p_timer, void* p_arg)
{
    (void)p_timer;
    auto* pObj = static_cast<TestOsTimerFreertos*>(p_arg);
    pObj->counter += 1;
}

static void
cmdHandlerTask(void* p_param)
{
    auto* pObj     = static_cast<TestOsTimerFreertos*>(p_param);
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
            case MainTaskCmd_TimerPeriodicCreate:
                pObj->p_timer_periodic = os_timer_periodic_create(
                    "timer_periodic",
                    100,
                    &timer_callback_periodic,
                    pObj);
                break;
            case MainTaskCmd_TimerPeriodicCreateStatic:
                pObj->p_timer_periodic = os_timer_periodic_create_static(
                    &pObj->timer_periodic_static,
                    "timer_periodic",
                    100,
                    &timer_callback_periodic,
                    pObj);
                break;
            case MainTaskCmd_TimerPeriodicStart:
                os_timer_periodic_start(pObj->p_timer_periodic);
                break;
            case MainTaskCmd_TimerPeriodicStop:
                os_timer_periodic_stop(pObj->p_timer_periodic);
                break;
            case MainTaskCmd_TimerPeriodicDelete:
                os_timer_periodic_delete(&pObj->p_timer_periodic);
                break;
            case MainTaskCmd_TimerOneShotCreate:
                pObj->p_timer_one_shot = os_timer_one_shot_create(
                    "timer_one_shot",
                    100,
                    &timer_callback_one_shot,
                    pObj);
                break;
            case MainTaskCmd_TimerOneShotCreateStatic:
                pObj->p_timer_one_shot = os_timer_one_shot_create_static(
                    &pObj->timer_one_shot_static,
                    "timer_one_shot",
                    100,
                    &timer_callback_one_shot,
                    pObj);
                break;
            case MainTaskCmd_TimerOneShotStart:
                os_timer_one_shot_start(pObj->p_timer_one_shot);
                break;
            case MainTaskCmd_TimerOneShotStop:
                os_timer_one_shot_stop(pObj->p_timer_one_shot);
                break;
            case MainTaskCmd_TimerOneShotDelete:
                os_timer_one_shot_delete(&pObj->p_timer_one_shot);
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
    auto* pObj = static_cast<TestOsTimerFreertos*>(arg);
    disableCheckingIfCurThreadIsFreeRTOS();
    const bool res
        = xTaskCreate(&cmdHandlerTask, "cmdHandlerTask", configMINIMAL_STACK_SIZE, pObj, tskIDLE_PRIORITY + 1, nullptr);
    assert(res);
    vTaskStartScheduler();
    return nullptr;
}

/*** Unit-Tests
 * *******************************************************************************************************/

TEST_F(TestOsTimerFreertos, test1) // NOLINT
{
    //
    // Test TimerOneShot
    //

    this->counter = 0;
    cmdQueue.push_and_wait(MainTaskCmd_TimerOneShotCreate);
    ASSERT_NE(nullptr, this->p_timer_one_shot);
    sleep_ms(200);
    ASSERT_EQ(0, this->counter);

    cmdQueue.push_and_wait(MainTaskCmd_TimerOneShotStart);
    sleep_ms(250);
    ASSERT_EQ(1, this->counter);

    cmdQueue.push_and_wait(MainTaskCmd_TimerOneShotStart);
    sleep_ms(150);
    ASSERT_EQ(2, this->counter);

    cmdQueue.push_and_wait(MainTaskCmd_TimerOneShotStart);
    cmdQueue.push_and_wait(MainTaskCmd_TimerOneShotStop);
    sleep_ms(150);
    ASSERT_EQ(2, this->counter);

    cmdQueue.push_and_wait(MainTaskCmd_TimerOneShotStart);
    sleep_ms(50);
    cmdQueue.push_and_wait(MainTaskCmd_TimerOneShotStop);
    sleep_ms(150);
    ASSERT_EQ(2, this->counter);

    cmdQueue.push_and_wait(MainTaskCmd_TimerOneShotStart);
    sleep_ms(150);
    ASSERT_EQ(3, this->counter);

    cmdQueue.push_and_wait(MainTaskCmd_TimerOneShotDelete);
    ASSERT_EQ(nullptr, this->p_timer_one_shot);

    //
    // Test TimerOneShotStatic
    //

    this->counter = 0;
    cmdQueue.push_and_wait(MainTaskCmd_TimerOneShotCreateStatic);
    ASSERT_NE(nullptr, this->p_timer_one_shot);
    sleep_ms(200);
    ASSERT_EQ(0, this->counter);

    cmdQueue.push_and_wait(MainTaskCmd_TimerOneShotStart);
    sleep_ms(250);
    ASSERT_EQ(1, this->counter);

    cmdQueue.push_and_wait(MainTaskCmd_TimerOneShotStart);
    sleep_ms(150);
    ASSERT_EQ(2, this->counter);

    cmdQueue.push_and_wait(MainTaskCmd_TimerOneShotStart);
    cmdQueue.push_and_wait(MainTaskCmd_TimerOneShotStop);
    sleep_ms(150);
    ASSERT_EQ(2, this->counter);

    cmdQueue.push_and_wait(MainTaskCmd_TimerOneShotStart);
    sleep_ms(50);
    cmdQueue.push_and_wait(MainTaskCmd_TimerOneShotStop);
    sleep_ms(150);
    ASSERT_EQ(2, this->counter);

    cmdQueue.push_and_wait(MainTaskCmd_TimerOneShotStart);
    sleep_ms(150);
    ASSERT_EQ(3, this->counter);

    cmdQueue.push_and_wait(MainTaskCmd_TimerOneShotDelete);
    ASSERT_EQ(nullptr, this->p_timer_one_shot);

    //
    // Test TimerPeriodic
    //

    this->counter = 0;
    cmdQueue.push_and_wait(MainTaskCmd_TimerPeriodicCreate);
    ASSERT_NE(nullptr, this->p_timer_periodic);
    sleep_ms(250);
    ASSERT_EQ(0, this->counter);

    cmdQueue.push_and_wait(MainTaskCmd_TimerPeriodicStart);
    sleep_ms(250);
    ASSERT_EQ(2, this->counter);

    cmdQueue.push_and_wait(MainTaskCmd_TimerPeriodicStop);
    sleep_ms(150);
    ASSERT_EQ(2, this->counter);

    cmdQueue.push_and_wait(MainTaskCmd_TimerPeriodicStart);
    cmdQueue.push_and_wait(MainTaskCmd_TimerPeriodicStop);
    sleep_ms(150);
    ASSERT_EQ(2, this->counter);

    cmdQueue.push_and_wait(MainTaskCmd_TimerPeriodicDelete);
    ASSERT_EQ(nullptr, this->p_timer_periodic);

    //
    // Test TimerPeriodicStatic
    //

    this->counter = 0;
    cmdQueue.push_and_wait(MainTaskCmd_TimerPeriodicCreateStatic);
    ASSERT_NE(nullptr, this->p_timer_periodic);
    sleep_ms(250);
    ASSERT_EQ(0, this->counter);

    cmdQueue.push_and_wait(MainTaskCmd_TimerPeriodicStart);
    sleep_ms(250);
    ASSERT_EQ(2, this->counter);

    cmdQueue.push_and_wait(MainTaskCmd_TimerPeriodicStop);
    sleep_ms(150);
    ASSERT_EQ(2, this->counter);

    cmdQueue.push_and_wait(MainTaskCmd_TimerPeriodicStart);
    cmdQueue.push_and_wait(MainTaskCmd_TimerPeriodicStop);
    sleep_ms(150);
    ASSERT_EQ(2, this->counter);

    cmdQueue.push_and_wait(MainTaskCmd_TimerPeriodicDelete);
    ASSERT_EQ(nullptr, this->p_timer_periodic);
}
