/**
 * @file test_os_task.cpp
 * @author TheSomeMan
 * @date 2020-11-06
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "test_os_task.hpp"

/*** Unit-Tests
 * *******************************************************************************************************/

TEST_F(TestOsTask, os_task_get_name_ok) // NOLINT
{
    this->m_taskName.assign("my_task_name1");
    const char* taskName = os_task_get_name();
    ASSERT_NE(nullptr, taskName);
    ASSERT_EQ(string(taskName), string("my_task_name1"));
}

TEST_F(TestOsTask, os_task_get_name_null) // NOLINT
{
    const char* taskName = os_task_get_name();
    ASSERT_NE(nullptr, taskName);
    ASSERT_EQ(string(taskName), string("???"));
}
