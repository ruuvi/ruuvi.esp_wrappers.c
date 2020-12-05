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
    union
    {
        StaticSemaphore_t static_sema;
        struct
        {
#if( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
            bool isStaticallyAllocated;
#endif
            bool       isUsed;
            bool       isLocked;
            TickType_t xTicksToWait;
        };
    };
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
#if( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
            mutex.isStaticallyAllocated = false;
#endif
            mutex.isUsed   = false;
            mutex.isLocked = false;
            mutex.xTicksToWait = 0;
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

static void prvInitialiseMutex(struct QueueDefinition *p_mutex, const bool isStaticallyAllocated)
{
    p_mutex->isUsed = true;
    p_mutex->isStaticallyAllocated = isStaticallyAllocated;
}

static void prvDeinitialiseMutex(struct QueueDefinition *p_mutex)
{
    p_mutex->isUsed = false;
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
    prvInitialiseMutex(p_mutex, false);
    return p_mutex;
}

QueueHandle_t
xQueueCreateMutexStatic(const uint8_t ucQueueType, StaticQueue_t *pxStaticQueue)
{
    (void)ucQueueType;
    auto *p_mutex = reinterpret_cast<struct QueueDefinition *>(pxStaticQueue);
    prvInitialiseMutex(p_mutex, true);
    return p_mutex;
}

void
vQueueDelete(QueueHandle_t xQueue)
{
    auto *p_mutex = reinterpret_cast<struct QueueDefinition *>(xQueue);
    if (!p_mutex->isStaticallyAllocated)
    {
        struct QueueDefinition *p_mutex2 = std::find_if(
            std::begin(g_pTestClass->arrOfMutexes),
            std::end(g_pTestClass->arrOfMutexes),
            [&](const struct QueueDefinition &x) { return &x == xQueue; });
        assert(std::end(g_pTestClass->arrOfMutexes) != p_mutex2);
        assert(p_mutex == p_mutex2);
    }
    prvDeinitialiseMutex(p_mutex);
}

BaseType_t
xQueueSemaphoreTake(QueueHandle_t xQueue, TickType_t xTicksToWait)
{
    (void)xTicksToWait;

    auto *p_mutex = reinterpret_cast<struct QueueDefinition *>(xQueue);

    if (!p_mutex->isStaticallyAllocated)
    {
        struct QueueDefinition *p_mutex2 = std::find_if(
            std::begin(g_pTestClass->arrOfMutexes),
            std::end(g_pTestClass->arrOfMutexes),
            [&](const struct QueueDefinition &x) { return &x == xQueue; });
        assert(std::end(g_pTestClass->arrOfMutexes) != p_mutex2);
        assert(p_mutex == p_mutex2);
    }
    assert(p_mutex->isUsed);
    if (p_mutex->isLocked)
    {
        return pdFALSE;
    }
    p_mutex->xTicksToWait = xTicksToWait;
    p_mutex->isLocked     = true;
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

    auto *p_mutex = reinterpret_cast<struct QueueDefinition *>(xQueue);

    if (!p_mutex->isStaticallyAllocated)
    {
        struct QueueDefinition *p_mutex2 = std::find_if(
            std::begin(g_pTestClass->arrOfMutexes),
            std::end(g_pTestClass->arrOfMutexes),
            [&](const struct QueueDefinition &x) { return &x == xQueue; });
        assert(std::end(g_pTestClass->arrOfMutexes) != p_mutex2);
        assert(p_mutex == p_mutex2);
    }

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

TEST_F(TestOsMutex, os_mutex_create_delete_dynamic_only) // NOLINT
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

TEST_F(TestOsMutex, os_mutex_create_delete_static_only) // NOLINT
{
    static StaticSemaphore_t static_sema1;
    os_mutex_t h_mutex1 = os_mutex_create_static(&static_sema1);
    ASSERT_EQ(reinterpret_cast<void*>(&static_sema1), reinterpret_cast<void*>(h_mutex1));

    os_mutex_delete(&h_mutex1);
    ASSERT_EQ(nullptr, h_mutex1);
}

TEST_F(TestOsMutex, os_mutex_create_delete_static_and_dynamic) // NOLINT
{
    static StaticSemaphore_t static_sema1;
    static StaticSemaphore_t static_sema2;

    os_mutex_t h_mutex_dyn1 = os_mutex_create();
    ASSERT_EQ(&this->arrOfMutexes[0], h_mutex_dyn1);

    os_mutex_t h_mutex_sta1 = os_mutex_create_static(&static_sema1);
    ASSERT_EQ(reinterpret_cast<void*>(&static_sema1), reinterpret_cast<void*>(h_mutex_sta1));

    os_mutex_t h_mutex_dyn2 = os_mutex_create();
    ASSERT_EQ(&this->arrOfMutexes[1], h_mutex_dyn2);

    os_mutex_t h_mutex_sta2 = os_mutex_create_static(&static_sema2);
    ASSERT_EQ(reinterpret_cast<void*>(&static_sema2), reinterpret_cast<void*>(h_mutex_sta2));

    os_mutex_t h_mutex_dyn3 = os_mutex_create();
    ASSERT_EQ(nullptr, h_mutex_dyn3);

    os_mutex_delete(&h_mutex_sta1);
    ASSERT_EQ(nullptr, h_mutex_sta1);
    os_mutex_delete(&h_mutex_sta2);
    ASSERT_EQ(nullptr, h_mutex_sta2);

    os_mutex_delete(&h_mutex_dyn1);
    ASSERT_EQ(nullptr, h_mutex_dyn1);

    h_mutex_dyn1 = os_mutex_create();
    ASSERT_EQ(&this->arrOfMutexes[0], h_mutex_dyn1);

    os_mutex_delete(&h_mutex_dyn2);
    ASSERT_EQ(nullptr, h_mutex_dyn2);

    h_mutex_dyn2 = os_mutex_create();
    ASSERT_EQ(&this->arrOfMutexes[1], h_mutex_dyn2);

    os_mutex_delete(&h_mutex_dyn1);
    ASSERT_EQ(nullptr, h_mutex_dyn1);
    os_mutex_delete(&h_mutex_dyn2);
    ASSERT_EQ(nullptr, h_mutex_dyn2);
}

TEST_F(TestOsMutex, os_mutex_lock_unlock) // NOLINT
{
    os_mutex_t              h_mutex = os_mutex_create();
    struct QueueDefinition *p_mutex = &this->arrOfMutexes[0];
    ASSERT_EQ(p_mutex, h_mutex);
    ASSERT_FALSE(p_mutex->isLocked);

    os_mutex_lock(h_mutex);
    ASSERT_TRUE(p_mutex->isLocked);
    ASSERT_EQ(portMAX_DELAY, p_mutex->xTicksToWait);

    os_mutex_unlock(h_mutex);
    ASSERT_FALSE(p_mutex->isLocked);

    os_mutex_delete(&h_mutex);
    ASSERT_EQ(nullptr, h_mutex);
}

TEST_F(TestOsMutex, os_mutex_lock_unlock_static) // NOLINT
{
    static StaticSemaphore_t static_sema1;
    os_mutex_t h_mutex = os_mutex_create_static(&static_sema1);
    ASSERT_EQ(reinterpret_cast<void*>(&static_sema1), reinterpret_cast<void*>(h_mutex));
    struct QueueDefinition *p_mutex = h_mutex;
    ASSERT_FALSE(p_mutex->isLocked);

    os_mutex_lock(h_mutex);
    ASSERT_TRUE(p_mutex->isLocked);
    ASSERT_EQ(portMAX_DELAY, p_mutex->xTicksToWait);

    os_mutex_unlock(h_mutex);
    ASSERT_FALSE(p_mutex->isLocked);

    os_mutex_delete(&h_mutex);
    ASSERT_EQ(nullptr, h_mutex);
}

TEST_F(TestOsMutex, os_mutex_try_lock_unlock) // NOLINT
{
    os_mutex_t              h_mutex = os_mutex_create();
    struct QueueDefinition *p_mutex = &this->arrOfMutexes[0];
    ASSERT_EQ(p_mutex, h_mutex);
    ASSERT_FALSE(p_mutex->isLocked);

    ASSERT_TRUE(os_mutex_try_lock(h_mutex));
    ASSERT_TRUE(p_mutex->isLocked);
    ASSERT_EQ(0, p_mutex->xTicksToWait);

    ASSERT_FALSE(os_mutex_try_lock(h_mutex));

    os_mutex_unlock(h_mutex);
    ASSERT_FALSE(p_mutex->isLocked);

    os_mutex_delete(&h_mutex);
    ASSERT_EQ(nullptr, h_mutex);
}

TEST_F(TestOsMutex, os_mutex_lock_with_timeout_unlock) // NOLINT
{
    os_mutex_t              h_mutex = os_mutex_create();
    struct QueueDefinition *p_mutex = &this->arrOfMutexes[0];
    ASSERT_EQ(p_mutex, h_mutex);
    ASSERT_FALSE(p_mutex->isLocked);

    const TickType_t xTicksToWait = 10;
    ASSERT_TRUE(os_mutex_lock_with_timeout(h_mutex, xTicksToWait));
    ASSERT_TRUE(p_mutex->isLocked);
    ASSERT_EQ(xTicksToWait, p_mutex->xTicksToWait);

    ASSERT_FALSE(os_mutex_lock_with_timeout(h_mutex, xTicksToWait));

    os_mutex_unlock(h_mutex);
    ASSERT_FALSE(p_mutex->isLocked);

    os_mutex_delete(&h_mutex);
    ASSERT_EQ(nullptr, h_mutex);
}
