/**
 * @file test_os_mutex.cpp
 * @author TheSomeMan
 * @date 2020-11-22
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include <string>
#include <sys/stat.h>
#include "gtest/gtest.h"
#include "esp_log_wrapper.hpp"
#include "esp_err.h"
#include "os_mutex.h"

using namespace std;

/*** Google-test class implementation
 * *********************************************************************************/

class TestOsMutex;
static TestOsMutex *g_pTestClass;

extern "C" {

struct QueueDefinition
{
    bool isUsed;
    bool isLocked;
    TickType_t xTicksToWait;
};

} // extern "C"

class TestOsMutex : public ::testing::Test
{
private:
protected:
    void
    SetUp() override
    {
        for (auto &mutex : this->arrOfMutexes)
        {
            mutex.isUsed = false;
            mutex.isLocked = false;
        }
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
    TestOsMutex();

    ~TestOsMutex() override;

    string m_taskName;

    struct QueueDefinition arrOfMutexes[2];
};

TestOsMutex::TestOsMutex()
    : Test()
    , m_taskName("os_mutex")
    , arrOfMutexes()
{
}

TestOsMutex::~TestOsMutex() = default;

extern "C" {

char *
pcTaskGetName(TaskHandle_t xTaskToQuery)
{
    assert(nullptr == xTaskToQuery);
    if (g_pTestClass->m_taskName == string(""))
    {
        return nullptr;
    }
    return const_cast<char *>(g_pTestClass->m_taskName.c_str());
}

QueueHandle_t
xQueueCreateMutex(const uint8_t ucQueueType)
{
    assert(queueQUEUE_TYPE_MUTEX == ucQueueType);
    struct QueueDefinition *p_mutex = std::find_if(
        std::begin(g_pTestClass->arrOfMutexes),
        std::end(g_pTestClass->arrOfMutexes),
        [&](const struct QueueDefinition &x) { return !x.isUsed; });
    if (std::end(g_pTestClass->arrOfMutexes) == p_mutex)
    {
        return nullptr;
    }
    p_mutex->isUsed = true;
    return p_mutex;
}

void
vQueueDelete(QueueHandle_t xQueue)
{
    struct QueueDefinition *p_mutex = std::find_if(
        std::begin(g_pTestClass->arrOfMutexes),
        std::end(g_pTestClass->arrOfMutexes),
        [&](const struct QueueDefinition &x) { return &x == xQueue; });
    assert (std::end(g_pTestClass->arrOfMutexes) != p_mutex);
    p_mutex->isUsed = false;
}

BaseType_t
xQueueSemaphoreTake(QueueHandle_t xQueue, TickType_t xTicksToWait)
{
    (void)xTicksToWait;

    struct QueueDefinition *p_mutex = std::find_if(
        std::begin(g_pTestClass->arrOfMutexes),
        std::end(g_pTestClass->arrOfMutexes),
        [&](const struct QueueDefinition &x) { return &x == xQueue; });
    assert (std::end(g_pTestClass->arrOfMutexes) != p_mutex);
    assert(p_mutex->isUsed);
    if (p_mutex->isLocked)
    {
        return pdFALSE;
    }
    p_mutex->xTicksToWait = xTicksToWait;
    p_mutex->isLocked = true;
    return pdTRUE;
}

BaseType_t
xQueueGenericSend(
    QueueHandle_t     xQueue,
    const void *const pvItemToQueue,
    TickType_t        xTicksToWait,
    const BaseType_t  xCopyPosition)
{
    assert(nullptr == pvItemToQueue);
    assert(semGIVE_BLOCK_TIME == xTicksToWait);
    assert(queueSEND_TO_BACK == xCopyPosition);

    struct QueueDefinition *p_mutex = std::find_if(
        std::begin(g_pTestClass->arrOfMutexes),
        std::end(g_pTestClass->arrOfMutexes),
        [&](const struct QueueDefinition &x) { return &x == xQueue; });
    assert (std::end(g_pTestClass->arrOfMutexes) != p_mutex);
    assert(p_mutex->isUsed);
    if (!p_mutex->isLocked)
    {
        return pdFALSE;
    }
    p_mutex->isLocked = false;

    return pdPASS;
}

} // extern "C"

/*** Unit-Tests
 * *******************************************************************************************************/

TEST_F(TestOsMutex, os_mutex_create_delete) // NOLINT
{
    os_mutex_t h_mutex1 = os_mutex_create();
    ASSERT_EQ(&this->arrOfMutexes[0], h_mutex1);

    os_mutex_t h_mutex2 = os_mutex_create();
    ASSERT_EQ(&this->arrOfMutexes[1], h_mutex2);

    os_mutex_t h_mutex3 = os_mutex_create();
    ASSERT_EQ(nullptr, h_mutex3);

    os_mutex_delete(&h_mutex1);
    ASSERT_EQ(nullptr, h_mutex1);

    h_mutex1 = os_mutex_create();
    ASSERT_EQ(&this->arrOfMutexes[0], h_mutex1);

    os_mutex_delete(&h_mutex2);
    ASSERT_EQ(nullptr, h_mutex2);

    h_mutex2 = os_mutex_create();
    ASSERT_EQ(&this->arrOfMutexes[1], h_mutex2);

    os_mutex_delete(&h_mutex1);
    ASSERT_EQ(nullptr, h_mutex1);
    os_mutex_delete(&h_mutex2);
    ASSERT_EQ(nullptr, h_mutex2);
}

TEST_F(TestOsMutex, os_mutex_lock_unlock) // NOLINT
{
    os_mutex_t h_mutex = os_mutex_create();
    struct QueueDefinition* p_mutex = &this->arrOfMutexes[0];
    ASSERT_EQ(p_mutex, h_mutex);
    ASSERT_FALSE(p_mutex->isLocked);

    os_mutex_lock(h_mutex);
    ASSERT_TRUE(p_mutex->isLocked);
    ASSERT_EQ(portMAX_DELAY, p_mutex->xTicksToWait);

    os_mutex_unlock(h_mutex);
    ASSERT_FALSE(p_mutex->isLocked);
}

TEST_F(TestOsMutex, os_mutex_try_lock_unlock) // NOLINT
{
    os_mutex_t h_mutex = os_mutex_create();
    struct QueueDefinition* p_mutex = &this->arrOfMutexes[0];
    ASSERT_EQ(p_mutex, h_mutex);
    ASSERT_FALSE(p_mutex->isLocked);

    ASSERT_TRUE(os_mutex_try_lock(h_mutex));
    ASSERT_TRUE(p_mutex->isLocked);
    ASSERT_EQ(0, p_mutex->xTicksToWait);

    ASSERT_FALSE(os_mutex_try_lock(h_mutex));

    os_mutex_unlock(h_mutex);
    ASSERT_FALSE(p_mutex->isLocked);
}

TEST_F(TestOsMutex, os_mutex_lock_with_timeout_unlock) // NOLINT
{
    os_mutex_t h_mutex = os_mutex_create();
    struct QueueDefinition* p_mutex = &this->arrOfMutexes[0];
    ASSERT_EQ(p_mutex, h_mutex);
    ASSERT_FALSE(p_mutex->isLocked);

    const TickType_t xTicksToWait = 10;
    ASSERT_TRUE(os_mutex_lock_with_timeout(h_mutex, xTicksToWait));
    ASSERT_TRUE(p_mutex->isLocked);
    ASSERT_EQ(xTicksToWait, p_mutex->xTicksToWait);

    ASSERT_FALSE(os_mutex_lock_with_timeout(h_mutex, xTicksToWait));

    os_mutex_unlock(h_mutex);
    ASSERT_FALSE(p_mutex->isLocked);
}
