/**
 * @file test_os_task_create_static_finite_without_param.cpp
 * @author TheSomeMan
 * @date 2020-12-08
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "test_os_task.hpp"

extern "C" {

struct tskTaskControlBlock
{
    int stub;
};

static void
task_func()
{
}

} // extern "C"

TEST_F(TestOsTask, os_task_create_static_finite_without_param_ok) // NOLINT
{
    const char *             task_name   = "my_task_name2";
    const uint32_t           stack_depth = sizeof(this->stack_mem) / sizeof(this->stack_mem[0]);
    const os_task_priority_t priority    = 3;
    this->m_taskName.assign(task_name);
    struct tskTaskControlBlock taskControlBlock = {};
    this->m_createdTaskHandle                   = &taskControlBlock;
    ASSERT_TRUE(os_task_create_static_finite_without_param(
        &task_func,
        task_name,
        &this->stack_mem[0],
        stack_depth,
        priority,
        &this->task_mem));
    ASSERT_TRUE(g_pTestClass->m_is_static);
    ASSERT_NE(nullptr, g_pTestClass->m_createdTaskFunc);
    ASSERT_EQ(g_pTestClass->m_createdTaskName, task_name);
    ASSERT_EQ(g_pTestClass->m_createdTaskStackDepth, stack_depth);
    ASSERT_NE(nullptr, g_pTestClass->m_createdTaskParam);
    ASSERT_EQ(g_pTestClass->m_createdTaskPriority, priority);
    TEST_CHECK_LOG_RECORD_WITH_THREAD(
        ESP_LOG_INFO,
        "my_task_name2",
        0,
        "Start thread(static) 'my_task_name2' with priority 3, stack size 2048 bytes");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}

TEST_F(TestOsTask, os_task_create_static_finite_without_param_fail) // NOLINT
{
    const char *             task_name   = "my_task_name2";
    const uint32_t           stack_depth = 2048;
    const os_task_priority_t priority    = 3;
    this->m_taskName.assign(task_name);
    this->m_createdTaskHandle = nullptr;
    ASSERT_FALSE(os_task_create_static_finite_without_param(
        &task_func,
        task_name,
        &this->stack_mem[0],
        stack_depth,
        priority,
        &this->task_mem));
    TEST_CHECK_LOG_RECORD_WITH_THREAD(
        ESP_LOG_INFO,
        "my_task_name2",
        0,
        "Start thread(static) 'my_task_name2' with priority 3, stack size 2048 bytes");
    TEST_CHECK_LOG_RECORD(ESP_LOG_ERROR, "Failed to start thread 'my_task_name2'");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}
