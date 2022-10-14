/**
 * @file test_os_task.hpp
 * @author TheSomeMan
 * @date 2020-12-08
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef TEST_OS_TASK_HPP
#define TEST_OS_TASK_HPP

#include "gtest/gtest.h"
#include "esp_log_wrapper.hpp"
#include "os_task.h"

using namespace std;

class TestOsTask;

extern TestOsTask* g_pTestClass;

/*** Google-test class implementation
 * *********************************************************************************/

class TestOsTask : public ::testing::Test
{
private:
protected:
    void
    SetUp() override
    {
        esp_log_wrapper_init();
        g_pTestClass = this;
    }

    void
    TearDown() override
    {
        esp_log_wrapper_deinit();
        g_pTestClass = nullptr;
    }

public:
    TestOsTask()
        : Test()
        , m_taskParam(0)
        , m_createdTaskHandle(nullptr)
        , m_createdTaskFunc(nullptr)
        , m_is_static(false)
        , m_createdTaskStackDepth(0)
        , m_createdTaskParam(nullptr)
        , m_createdTaskPriority(0)
        , stack_mem({})
        , task_mem({})
    {
    }

    ~TestOsTask() override = default;

    string m_taskName;
    int    m_taskParam;

    TaskHandle_t                           m_createdTaskHandle;
    TaskFunction_t                         m_createdTaskFunc;
    bool                                   m_is_static;
    string                                 m_createdTaskName;
    configSTACK_DEPTH_TYPE                 m_createdTaskStackDepth;
    void*                                  m_createdTaskParam;
    UBaseType_t                            m_createdTaskPriority;
    std::array<os_task_stack_type_t, 2048> stack_mem;
    os_task_static_t                       task_mem;
};

#define TEST_CHECK_LOG_RECORD(level_, msg_) ESP_LOG_WRAPPER_TEST_CHECK_LOG_RECORD("os_task", level_, msg_);

#define TEST_CHECK_LOG_RECORD_WITH_THREAD(level_, thread_, priority_, msg_) \
    ESP_LOG_WRAPPER_TEST_CHECK_LOG_RECORD_WITH_THREAD("os_task", level_, thread_, priority_, msg_);

#endif // TEST_OS_TASK_HPP
