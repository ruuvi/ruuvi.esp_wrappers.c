/**
 * @file test_os_task_freertos.cpp
 * @author TheSomeMan
 * @date 2020-12-06
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include <string>
#include <semaphore.h>
#include <sys/time.h>
#include "gtest/gtest.h"
#include "os_task.h"
#include "TQueue.hpp"
#include "esp_log_wrapper.hpp"

using namespace std;

typedef enum MainTaskCmd_Tag
{
    MainTaskCmd_Exit,
} MainTaskCmd_e;

/*** Google-test class implementation
 * *********************************************************************************/

class TestOsTaskFreertos;
static TestOsTaskFreertos* g_pTestClass;

static void*
freertosStartup(void* arg);

class TestOsTaskFreertos : public ::testing::Test
{
private:
protected:
    pthread_t pid;

    void
    SetUp() override
    {
        esp_log_wrapper_init();
        sem_init(&semaFreeRTOS, 0, 0);
        const int err = pthread_create(&pid, nullptr, &freertosStartup, this);
        assert(0 == err);
        while (0 != sem_wait(&semaFreeRTOS))
        {
        }
        g_pTestClass = this;
    }

    void
    TearDown() override
    {
        cmdQueue.push_and_wait(MainTaskCmd_Exit);
        vTaskEndScheduler();
        void* ret_code = nullptr;
        pthread_join(pid, &ret_code);
        sem_destroy(&semaFreeRTOS);
        esp_log_wrapper_deinit();
        g_pTestClass = nullptr;
    }

public:
    sem_t                                  semaFreeRTOS;
    TQueue<MainTaskCmd_e>                  cmdQueue;
    volatile uint32_t                      m_counter;
    std::array<os_task_stack_type_t, 2048> m_stack_mem;
    os_task_static_t                       m_task_mem;

    TestOsTaskFreertos()
        : Test()
        , pid(0)
        , semaFreeRTOS({})
        , m_counter(0)
        , m_stack_mem({})
        , m_task_mem({})
    {
    }

    ~TestOsTaskFreertos() override = default;
};

extern "C" {

static void
sleep_ms_unchecked(uint32_t msec)
{
    struct timespec time_spec = {
        .tv_sec  = msec / 1000U,
        .tv_nsec = (msec % 1000U) * 1000000U,
    };

    (void)nanosleep(&time_spec, &time_spec);
}

static void
sleep_ms(uint32_t msec)
{
    const TickType_t tick_start = xTaskGetTickCount();
    while (true)
    {
        const TickType_t delta_ticks = xTaskGetTickCount() - tick_start;
        if (delta_ticks >= pdMS_TO_TICKS(msec))
        {
            break;
        }
        const uint32_t remain_msec = (pdMS_TO_TICKS(msec) - delta_ticks) * 1000 / configTICK_RATE_HZ;
        sleep_ms_unchecked(remain_msec);
    }
}

} // extern "C"

static void
cmdHandlerTask(void* p_param)
{
    auto* pObj     = static_cast<TestOsTaskFreertos*>(p_param);
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
            default:
                printf("Error: Unknown cmd %d\n", (int)cmd);
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
    auto*      pObj = static_cast<TestOsTaskFreertos*>(arg);
    const bool res
        = xTaskCreate(&cmdHandlerTask, "cmdHandlerTask", configMINIMAL_STACK_SIZE, pObj, tskIDLE_PRIORITY + 1, nullptr);
    assert(res);
    vTaskStartScheduler();
    return nullptr;
}

/*** Unit-Tests
 * *******************************************************************************************************/

static ATTR_NORETURN void
task_func_infinite_with_param(void* p_arg)
{
    auto* p_counter = reinterpret_cast<volatile uint32_t*>(p_arg);
    for (;;)
    {
        *p_counter += 1;
        os_task_delay(10);
    }
}

static void
task_func_finite_with_param(void* p_arg)
{
    auto* p_counter = reinterpret_cast<volatile uint32_t*>(p_arg);
    *p_counter += 1;
    os_task_delay(10);
}

typedef struct PtrToCounter_t
{
    volatile uint32_t* p_counter;
} PtrToCounter_t;

static ATTR_NORETURN void
task_func_infinite_with_const_param(const void* p_arg)
{
    auto*              p_param   = reinterpret_cast<const PtrToCounter_t*>(p_arg);
    volatile uint32_t* p_counter = p_param->p_counter;
    for (;;)
    {
        *p_counter += 1;
        os_task_delay(10);
    }
}

static void
task_func_finite_with_const_param(const void* p_arg)
{
    auto*              p_param   = reinterpret_cast<const PtrToCounter_t*>(p_arg);
    volatile uint32_t* p_counter = p_param->p_counter;
    *p_counter += 1;
    os_task_delay(10);
}

static ATTR_NORETURN void
task_func_infinite_without_param()
{
    auto* p_counter = &g_pTestClass->m_counter;
    for (;;)
    {
        *p_counter += 1;
        os_task_delay(10);
    }
}

static void
task_func_finite_without_param()
{
    auto* p_counter = &g_pTestClass->m_counter;
    *p_counter += 1;
    os_task_delay(10);
}

TEST_F(TestOsTaskFreertos, test1) // NOLINT
{
    const os_task_priority_t priority = 3;
    {
        const uint32_t   stack_depth = 4096U;
        os_task_handle_t h_task      = nullptr;
        this->m_counter              = 0;
        ASSERT_TRUE(os_task_create(
            &task_func_infinite_with_param,
            "dyn_with_param",
            stack_depth,
            reinterpret_cast<void*>(const_cast<uint32_t*>(&this->m_counter)),
            priority,
            &h_task));
        sleep_ms(50);
        const uint32_t saved_counter = this->m_counter;
        ASSERT_NE(0, saved_counter);
        sleep_ms(50);
        ASSERT_NE(saved_counter, this->m_counter);

        os_task_delete(&h_task);
        ASSERT_EQ(nullptr, h_task);
        sleep_ms(20);
        const uint32_t saved_counter2 = this->m_counter;
        sleep_ms(50);
        ASSERT_EQ(saved_counter2, this->m_counter);
    }
    {
        const uint32_t   stack_depth      = 4096U;
        os_task_handle_t h_task           = nullptr;
        this->m_counter                   = 0;
        const PtrToCounter_t ptrToCounter = {
            .p_counter = &this->m_counter,
        };
        ASSERT_TRUE(os_task_create_with_const_param(
            &task_func_infinite_with_const_param,
            "dyn_with_cparam",
            stack_depth,
            reinterpret_cast<const void*>(&ptrToCounter),
            priority,
            &h_task));
        sleep_ms(50);
        const uint32_t saved_counter = this->m_counter;
        ASSERT_NE(0, saved_counter);
        sleep_ms(50);
        ASSERT_NE(saved_counter, this->m_counter);

        os_task_delete(&h_task);
        ASSERT_EQ(nullptr, h_task);
        sleep_ms(20);
        const uint32_t saved_counter2 = this->m_counter;
        sleep_ms(50);
        ASSERT_EQ(saved_counter2, this->m_counter);
    }
    {
        const uint32_t   stack_depth = 4096U;
        os_task_handle_t h_task      = nullptr;
        ASSERT_TRUE(os_task_create_without_param(
            &task_func_infinite_without_param,
            "dyn_with_param",
            stack_depth,
            priority,
            &h_task));
        sleep_ms(50);
        const uint32_t saved_counter = this->m_counter;
        ASSERT_NE(0, saved_counter);
        sleep_ms(50);
        ASSERT_NE(saved_counter, this->m_counter);

        os_task_delete(&h_task);
        ASSERT_EQ(nullptr, h_task);
        sleep_ms(20);
        const uint32_t saved_counter2 = this->m_counter;
        sleep_ms(50);
        ASSERT_EQ(saved_counter2, this->m_counter);
    }
    {
        const uint32_t stack_depth = 4096U;
        this->m_counter            = 0;
        ASSERT_TRUE(os_task_create_finite(
            &task_func_finite_with_param,
            "dyn_fin_with_param",
            stack_depth,
            reinterpret_cast<void*>(const_cast<uint32_t*>(&this->m_counter)),
            priority));
        sleep_ms(50);
        const uint32_t saved_counter = this->m_counter;
        ASSERT_NE(0, saved_counter);
        sleep_ms(50);
        ASSERT_EQ(saved_counter, this->m_counter);
    }
    {
        const uint32_t stack_depth        = 4096U;
        this->m_counter                   = 0;
        const PtrToCounter_t ptrToCounter = {
            .p_counter = &this->m_counter,
        };
        ASSERT_TRUE(os_task_create_finite_with_const_param(
            &task_func_finite_with_const_param,
            "dyn_fin_with_cparam",
            stack_depth,
            reinterpret_cast<const void*>(&ptrToCounter),
            priority));
        sleep_ms(50);
        const uint32_t saved_counter = this->m_counter;
        ASSERT_NE(0, saved_counter);
        sleep_ms(50);
        ASSERT_EQ(saved_counter, this->m_counter);
    }
    {
        const uint32_t stack_depth = 4096U;
        ASSERT_TRUE(os_task_create_finite_without_param(
            &task_func_finite_without_param,
            "dyn_fin_with_param",
            stack_depth,
            priority));
        sleep_ms(50);
        const uint32_t saved_counter = this->m_counter;
        ASSERT_NE(0, saved_counter);
        sleep_ms(50);
        ASSERT_EQ(saved_counter, this->m_counter);
    }

    {
        os_task_handle_t h_task = nullptr;
        this->m_counter         = 0;
        ASSERT_TRUE(os_task_create_static(
            &task_func_infinite_with_param,
            "dyn_with_param",
            &this->m_stack_mem[0],
            sizeof(this->m_stack_mem) / sizeof(this->m_stack_mem[0]),
            reinterpret_cast<void*>(const_cast<uint32_t*>(&this->m_counter)),
            priority,
            &this->m_task_mem,
            &h_task));
        sleep_ms(50);
        const uint32_t saved_counter = this->m_counter;
        ASSERT_NE(0, saved_counter);
        sleep_ms(50);
        ASSERT_NE(saved_counter, this->m_counter);

        os_task_delete(&h_task);
        ASSERT_EQ(nullptr, h_task);
        sleep_ms(20);
        const uint32_t saved_counter2 = this->m_counter;
        sleep_ms(50);
        ASSERT_EQ(saved_counter2, this->m_counter);
    }
    {
        os_task_handle_t h_task           = nullptr;
        this->m_counter                   = 0;
        const PtrToCounter_t ptrToCounter = {
            .p_counter = &this->m_counter,
        };
        ASSERT_TRUE(os_task_create_static_with_const_param(
            &task_func_infinite_with_const_param,
            "dyn_with_cparam",
            &this->m_stack_mem[0],
            sizeof(this->m_stack_mem) / sizeof(this->m_stack_mem[0]),
            reinterpret_cast<const void*>(&ptrToCounter),
            priority,
            &this->m_task_mem,
            &h_task));
        sleep_ms(50);
        const uint32_t saved_counter = this->m_counter;
        ASSERT_NE(0, saved_counter);
        sleep_ms(50);
        ASSERT_NE(saved_counter, this->m_counter);

        os_task_delete(&h_task);
        ASSERT_EQ(nullptr, h_task);
        sleep_ms(20);
        const uint32_t saved_counter2 = this->m_counter;
        sleep_ms(50);
        ASSERT_EQ(saved_counter2, this->m_counter);
    }
    {
        os_task_handle_t h_task = nullptr;
        ASSERT_TRUE(os_task_create_static_without_param(
            &task_func_infinite_without_param,
            "dyn_with_param",
            &this->m_stack_mem[0],
            sizeof(this->m_stack_mem) / sizeof(this->m_stack_mem[0]),
            priority,
            &this->m_task_mem,
            &h_task));
        sleep_ms(50);
        const uint32_t saved_counter = this->m_counter;
        ASSERT_NE(0, saved_counter);
        sleep_ms(50);
        ASSERT_NE(saved_counter, this->m_counter);

        os_task_delete(&h_task);
        ASSERT_EQ(nullptr, h_task);
        sleep_ms(20);
        const uint32_t saved_counter2 = this->m_counter;
        sleep_ms(50);
        ASSERT_EQ(saved_counter2, this->m_counter);
    }
    {
        this->m_counter = 0;
        ASSERT_TRUE(os_task_create_static_finite(
            &task_func_finite_with_param,
            "dyn_fin_with_param",
            &this->m_stack_mem[0],
            sizeof(this->m_stack_mem) / sizeof(this->m_stack_mem[0]),
            reinterpret_cast<void*>(const_cast<uint32_t*>(&this->m_counter)),
            priority,
            &this->m_task_mem));
        sleep_ms(50);
        const uint32_t saved_counter = this->m_counter;
        ASSERT_NE(0, saved_counter);
        sleep_ms(50);
        ASSERT_EQ(saved_counter, this->m_counter);
    }
    {
        this->m_counter                   = 0;
        const PtrToCounter_t ptrToCounter = {
            .p_counter = &this->m_counter,
        };
        ASSERT_TRUE(os_task_create_static_finite_with_const_param(
            &task_func_finite_with_const_param,
            "dyn_fin_with_cparam",
            &this->m_stack_mem[0],
            sizeof(this->m_stack_mem) / sizeof(this->m_stack_mem[0]),
            reinterpret_cast<const void*>(&ptrToCounter),
            priority,
            &this->m_task_mem));
        sleep_ms(50);
        const uint32_t saved_counter = this->m_counter;
        ASSERT_NE(0, saved_counter);
        sleep_ms(50);
        ASSERT_EQ(saved_counter, this->m_counter);
    }
    {
        ASSERT_TRUE(os_task_create_static_finite_without_param(
            &task_func_finite_without_param,
            "dyn_fin_with_param",
            &this->m_stack_mem[0],
            sizeof(this->m_stack_mem) / sizeof(this->m_stack_mem[0]),
            priority,
            &this->m_task_mem));
        sleep_ms(50);
        const uint32_t saved_counter = this->m_counter;
        ASSERT_NE(0, saved_counter);
        sleep_ms(50);
        ASSERT_EQ(saved_counter, this->m_counter);
    }
}