/**
 * @file test_os_sema.cpp
 * @author TheSomeMan
 * @date 2020-12-05
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include <string>
#include <sys/stat.h>
#include "gtest/gtest.h"
#include "esp_log_wrapper.hpp"
#include "os_sema.h"

using namespace std;

/*** Google-test class implementation
 * *********************************************************************************/

class TestOsSema;
static TestOsSema* g_pTestClass;

extern "C" {

struct QueueDefinition
{
    union
    {
        StaticSemaphore_t static_sema;
        struct
        {
#if ((configSUPPORT_STATIC_ALLOCATION == 1) && (configSUPPORT_DYNAMIC_ALLOCATION == 1))
            bool isStaticallyAllocated;
#endif
            bool       isUsed;
            bool       isLocked;
            TickType_t xTicksToWait;
        };
    };
};

} // extern "C"

class TestOsSema : public ::testing::Test
{
private:
protected:
    void
    SetUp() override
    {
        for (auto& sema : this->arrOfSemaphores)
        {
#if ((configSUPPORT_STATIC_ALLOCATION == 1) && (configSUPPORT_DYNAMIC_ALLOCATION == 1))
            sema.isStaticallyAllocated = false;
#endif
            sema.isUsed       = false;
            sema.isLocked     = false;
            sema.xTicksToWait = 0;
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
    TestOsSema();

    ~TestOsSema() override;

    string m_taskName;

    struct QueueDefinition arrOfSemaphores[2];
};

TestOsSema::TestOsSema()
    : Test()
    , m_taskName("os_sema")
    , arrOfSemaphores()
{
}

TestOsSema::~TestOsSema() = default;

extern "C" {

char*
pcTaskGetName(TaskHandle_t xTaskToQuery)
{
    assert(nullptr == xTaskToQuery);
    if (g_pTestClass->m_taskName == string(""))
    {
        return nullptr;
    }
    return const_cast<char*>(g_pTestClass->m_taskName.c_str());
}

static void
prvInitialiseSema(struct QueueDefinition* p_sema, const bool isStaticallyAllocated)
{
    p_sema->isUsed                = true;
    p_sema->isStaticallyAllocated = isStaticallyAllocated;
}

static void
prvDeinitialiseSema(struct QueueDefinition* p_sema)
{
    p_sema->isUsed = false;
}

QueueHandle_t
xQueueGenericCreate(const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize, const uint8_t ucQueueType)
{
    assert(queueQUEUE_TYPE_BINARY_SEMAPHORE == ucQueueType);
    assert(1 == uxQueueLength);
    assert(semSEMAPHORE_QUEUE_ITEM_LENGTH == uxItemSize);
    struct QueueDefinition* p_sema = std::find_if(
        std::begin(g_pTestClass->arrOfSemaphores),
        std::end(g_pTestClass->arrOfSemaphores),
        [&](const struct QueueDefinition& x) { return !x.isUsed; });
    if (std::end(g_pTestClass->arrOfSemaphores) == p_sema)
    {
        return nullptr;
    }
    prvInitialiseSema(p_sema, false);
    return p_sema;
}

QueueHandle_t
xQueueGenericCreateStatic(
    const UBaseType_t uxQueueLength,
    const UBaseType_t uxItemSize,
    uint8_t*          pucQueueStorage,
    StaticQueue_t*    pxStaticQueue,
    const uint8_t     ucQueueType)
{
    assert(queueQUEUE_TYPE_BINARY_SEMAPHORE == ucQueueType);
    assert(1 == uxQueueLength);
    assert(semSEMAPHORE_QUEUE_ITEM_LENGTH == uxItemSize);
    assert(nullptr == pucQueueStorage);
    assert(nullptr != pxStaticQueue);
    auto* p_sema = reinterpret_cast<struct QueueDefinition*>(pxStaticQueue);
    prvInitialiseSema(p_sema, true);
    return p_sema;
}

void
vQueueDelete(QueueHandle_t xQueue)
{
    auto* p_sema = reinterpret_cast<struct QueueDefinition*>(xQueue);
    if (!p_sema->isStaticallyAllocated)
    {
        struct QueueDefinition* p_sema2 = std::find_if(
            std::begin(g_pTestClass->arrOfSemaphores),
            std::end(g_pTestClass->arrOfSemaphores),
            [&](const struct QueueDefinition& x) { return &x == xQueue; });
        assert(std::end(g_pTestClass->arrOfSemaphores) != p_sema2);
        assert(p_sema == p_sema2);
    }
    prvDeinitialiseSema(p_sema);
}

BaseType_t
xQueueSemaphoreTake(QueueHandle_t xQueue, TickType_t xTicksToWait)
{
    (void)xTicksToWait;

    auto* p_sema = reinterpret_cast<struct QueueDefinition*>(xQueue);

    if (!p_sema->isStaticallyAllocated)
    {
        struct QueueDefinition* p_sema2 = std::find_if(
            std::begin(g_pTestClass->arrOfSemaphores),
            std::end(g_pTestClass->arrOfSemaphores),
            [&](const struct QueueDefinition& x) { return &x == xQueue; });
        assert(std::end(g_pTestClass->arrOfSemaphores) != p_sema2);
        assert(p_sema == p_sema2);
    }
    assert(p_sema->isUsed);
    if (p_sema->isLocked)
    {
        return pdFALSE;
    }
    p_sema->xTicksToWait = xTicksToWait;
    p_sema->isLocked     = true;
    return pdTRUE;
}

BaseType_t
xQueueGenericSend(
    QueueHandle_t     xQueue,
    const void* const pvItemToQueue,
    TickType_t        xTicksToWait,
    const BaseType_t  xCopyPosition)
{
    assert(nullptr == pvItemToQueue);
    assert(semGIVE_BLOCK_TIME == xTicksToWait);
    assert(queueSEND_TO_BACK == xCopyPosition);

    auto* p_sema = reinterpret_cast<struct QueueDefinition*>(xQueue);

    if (!p_sema->isStaticallyAllocated)
    {
        struct QueueDefinition* p_sema2 = std::find_if(
            std::begin(g_pTestClass->arrOfSemaphores),
            std::end(g_pTestClass->arrOfSemaphores),
            [&](const struct QueueDefinition& x) { return &x == xQueue; });
        assert(std::end(g_pTestClass->arrOfSemaphores) != p_sema2);
        assert(p_sema == p_sema2);
    }

    assert(p_sema->isUsed);
    if (!p_sema->isLocked)
    {
        return pdFALSE;
    }
    p_sema->isLocked = false;

    return pdPASS;
}

} // extern "C"

/*** Unit-Tests
 * *******************************************************************************************************/

TEST_F(TestOsSema, os_sema_create_delete_dynamic_only) // NOLINT
{
    os_sema_t h_sema1 = os_sema_create();
    ASSERT_EQ(&this->arrOfSemaphores[0], h_sema1);

    os_sema_t h_sema2 = os_sema_create();
    ASSERT_EQ(&this->arrOfSemaphores[1], h_sema2);

    os_sema_t h_sema3 = os_sema_create();
    ASSERT_EQ(nullptr, h_sema3);

    os_sema_delete(&h_sema1);
    ASSERT_EQ(nullptr, h_sema1);

    h_sema1 = os_sema_create();
    ASSERT_EQ(&this->arrOfSemaphores[0], h_sema1);

    os_sema_delete(&h_sema2);
    ASSERT_EQ(nullptr, h_sema2);

    h_sema2 = os_sema_create();
    ASSERT_EQ(&this->arrOfSemaphores[1], h_sema2);

    os_sema_delete(&h_sema1);
    ASSERT_EQ(nullptr, h_sema1);
    os_sema_delete(&h_sema2);
    ASSERT_EQ(nullptr, h_sema2);
}

TEST_F(TestOsSema, os_sema_create_delete_static_only) // NOLINT
{
    static StaticSemaphore_t static_sema1;
    os_sema_t                h_sema1 = os_sema_create_static(&static_sema1);
    ASSERT_EQ(reinterpret_cast<void*>(&static_sema1), reinterpret_cast<void*>(h_sema1));

    os_sema_delete(&h_sema1);
    ASSERT_EQ(nullptr, h_sema1);
}

TEST_F(TestOsSema, os_sema_create_delete_static_and_dynamic) // NOLINT
{
    static StaticSemaphore_t static_sema1;
    static StaticSemaphore_t static_sema2;

    os_sema_t h_sema_dyn1 = os_sema_create();
    ASSERT_EQ(&this->arrOfSemaphores[0], h_sema_dyn1);

    os_sema_t h_sema_sta1 = os_sema_create_static(&static_sema1);
    ASSERT_EQ(reinterpret_cast<void*>(&static_sema1), reinterpret_cast<void*>(h_sema_sta1));

    os_sema_t h_sema_dyn2 = os_sema_create();
    ASSERT_EQ(&this->arrOfSemaphores[1], h_sema_dyn2);

    os_sema_t h_sema_sta2 = os_sema_create_static(&static_sema2);
    ASSERT_EQ(reinterpret_cast<void*>(&static_sema2), reinterpret_cast<void*>(h_sema_sta2));

    os_sema_t h_sema_dyn3 = os_sema_create();
    ASSERT_EQ(nullptr, h_sema_dyn3);

    os_sema_delete(&h_sema_sta1);
    ASSERT_EQ(nullptr, h_sema_sta1);
    os_sema_delete(&h_sema_sta2);
    ASSERT_EQ(nullptr, h_sema_sta2);

    os_sema_delete(&h_sema_dyn1);
    ASSERT_EQ(nullptr, h_sema_dyn1);

    h_sema_dyn1 = os_sema_create();
    ASSERT_EQ(&this->arrOfSemaphores[0], h_sema_dyn1);

    os_sema_delete(&h_sema_dyn2);
    ASSERT_EQ(nullptr, h_sema_dyn2);

    h_sema_dyn2 = os_sema_create();
    ASSERT_EQ(&this->arrOfSemaphores[1], h_sema_dyn2);

    os_sema_delete(&h_sema_dyn1);
    ASSERT_EQ(nullptr, h_sema_dyn1);
    os_sema_delete(&h_sema_dyn2);
    ASSERT_EQ(nullptr, h_sema_dyn2);
}

TEST_F(TestOsSema, os_sema_wait_infinite) // NOLINT
{
    os_sema_t               h_sema = os_sema_create();
    struct QueueDefinition* p_sema = &this->arrOfSemaphores[0];
    ASSERT_EQ(p_sema, h_sema);
    ASSERT_FALSE(p_sema->isLocked);

    os_sema_wait_infinite(h_sema);
    ASSERT_TRUE(p_sema->isLocked);
    ASSERT_EQ(portMAX_DELAY, p_sema->xTicksToWait);

    os_sema_signal(h_sema);
    ASSERT_FALSE(p_sema->isLocked);

    os_sema_delete(&h_sema);
    ASSERT_EQ(nullptr, h_sema);
}

TEST_F(TestOsSema, os_sema_wait_infinite_static) // NOLINT
{
    static StaticSemaphore_t static_sema1;
    os_sema_t                h_sema = os_sema_create_static(&static_sema1);
    ASSERT_EQ(reinterpret_cast<void*>(&static_sema1), reinterpret_cast<void*>(h_sema));
    struct QueueDefinition* p_sema = h_sema;
    ASSERT_FALSE(p_sema->isLocked);

    os_sema_wait_infinite(h_sema);
    ASSERT_TRUE(p_sema->isLocked);
    ASSERT_EQ(portMAX_DELAY, p_sema->xTicksToWait);

    os_sema_signal(h_sema);
    ASSERT_FALSE(p_sema->isLocked);

    os_sema_delete(&h_sema);
    ASSERT_EQ(nullptr, h_sema);
}

TEST_F(TestOsSema, os_sema_wait_immediate) // NOLINT
{
    os_sema_t               h_sema = os_sema_create();
    struct QueueDefinition* p_sema = &this->arrOfSemaphores[0];
    ASSERT_EQ(p_sema, h_sema);
    ASSERT_FALSE(p_sema->isLocked);

    ASSERT_TRUE(os_sema_wait_immediate(h_sema));
    ASSERT_TRUE(p_sema->isLocked);
    ASSERT_EQ(0, p_sema->xTicksToWait);

    ASSERT_FALSE(os_sema_wait_immediate(h_sema));

    os_sema_signal(h_sema);
    ASSERT_FALSE(p_sema->isLocked);

    os_sema_delete(&h_sema);
    ASSERT_EQ(nullptr, h_sema);
}

TEST_F(TestOsSema, os_sema_wait_with_timeout) // NOLINT
{
    os_sema_t               h_sema = os_sema_create();
    struct QueueDefinition* p_sema = &this->arrOfSemaphores[0];
    ASSERT_EQ(p_sema, h_sema);
    ASSERT_FALSE(p_sema->isLocked);

    const TickType_t xTicksToWait = 10;
    ASSERT_TRUE(os_sema_wait_with_timeout(h_sema, xTicksToWait));
    ASSERT_TRUE(p_sema->isLocked);
    ASSERT_EQ(xTicksToWait, p_sema->xTicksToWait);

    ASSERT_FALSE(os_sema_wait_with_timeout(h_sema, xTicksToWait));

    os_sema_signal(h_sema);
    ASSERT_FALSE(p_sema->isLocked);

    os_sema_delete(&h_sema);
    ASSERT_EQ(nullptr, h_sema);
}
