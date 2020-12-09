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

extern TestOsTask *g_pTestClass;

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
    {
    }

    ~TestOsTask() = default;

    string m_taskName;
    int    m_taskParam;

    TaskHandle_t           m_createdTaskHandle;
    TaskFunction_t         m_createdTaskFunc;
    bool                   m_is_static;
    string                 m_createdTaskName;
    configSTACK_DEPTH_TYPE m_createdTaskStackDepth;
    void *                 m_createdTaskParam;
    UBaseType_t            m_createdTaskPriority;
    os_task_stack_type_t   stack_mem[2048];
    os_task_static_t       task_mem;
};

#define TEST_CHECK_LOG_RECORD_EX(tag_, level_, msg_, flag_skip_file_info_) \
    do \
    { \
        ASSERT_FALSE(esp_log_wrapper_is_empty()); \
        const LogRecord log_record = esp_log_wrapper_pop(); \
        ASSERT_EQ(level_, log_record.level); \
        ASSERT_EQ(string(tag_), log_record.tag); \
        if (flag_skip_file_info_) \
        { \
            const char *p = strchr(log_record.message.c_str(), ' '); \
            assert(NULL != p); \
            p += 1; \
            p = strchr(p, ' '); \
            assert(NULL != p); \
            p += 1; \
            p = strchr(p, ' '); \
            assert(NULL != p); \
            p += 1; \
            ASSERT_EQ(string(msg_), p); \
        } \
        else \
        { \
            ASSERT_EQ(string(msg_), log_record.message); \
        } \
    } while (0)

#define TEST_CHECK_LOG_RECORD(level_, msg_)         TEST_CHECK_LOG_RECORD_EX("os_task", level_, msg_, false);
#define TEST_CHECK_LOG_RECORD_NO_FILE(level_, msg_) TEST_CHECK_LOG_RECORD_EX("os_task", level_, msg_, true);

#endif // TEST_OS_TASK_HPP
