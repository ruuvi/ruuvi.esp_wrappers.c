/**
 * @file test_os_rw_lock.cpp
 * @author TheSomeMan
 * @date 2023-07-07
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "os_rw_lock.h"
#include "gtest/gtest.h"
#include "TQueue.hpp"
#include <string>
#include <thread>
#include <chrono>

using namespace std;

/*** Google-test class implementation
 * *********************************************************************************/

class TestOsRwLock;
static TestOsRwLock* g_pTestClass;

typedef enum test_task_cmd_e
{
    TEST_TASK_CMD_EXIT,
    TEST_TASK_CMD_LOCK_WRITER,
    TEST_TASK_CMD_TRY_LOCK_WRITER,
    TEST_TASK_CMD_UNLOCK_WRITER,
    TEST_TASK_CMD_LOCK_READER,
    TEST_TASK_CMD_TRY_LOCK_READER,
    TEST_TASK_CMD_UNLOCK_READER,
} test_task_cmd_e;

enum ActionType
{
    ACTION_TYPE_MUTEX_TRY_LOCK_READER,
    ACTION_TYPE_MUTEX_LOCK_READER_WITH_TIMEOUT,
    ACTION_TYPE_MUTEX_LOCK_READER,
    ACTION_TYPE_MUTEX_UNLOCK_READER,
    ACTION_TYPE_SEMA_WAIT_INFINITE_WRITER,
    ACTION_TYPE_SEMA_WAIT_IMMEDIATE_WRITER,
    ACTION_TYPE_SEMA_SIGNAL_WRITER,
};

struct ActionDesc
{
    ActionType actionType;
    union
    {
        struct
        {
            os_delta_ticks_t ticksToWait;
            uint32_t         sleep_ms;
        } lockReaderWithTimeout;
        struct
        {
            uint32_t sleep_ms;
        } tryLockReader;
        struct
        {
            uint32_t sleep_ms;
        } semaWaitImmediate;
    } param;
};

class TestTask
{
public:
    TQueue<test_task_cmd_e> cmdQueue;
    bool                    res;

    void
    setUp()
    {
    }

    void
    tearDown()
    {
        cmdQueue.push_and_wait(TEST_TASK_CMD_EXIT);
    }
};

static void
cmdHandlerTask(TestTask* p_testTask);

class TestOsRwLock : public ::testing::Test
{
private:
protected:
    void
    SetUp() override
    {
        g_pTestClass = this;

        p_arrOfActions = nullptr;
        numActions     = 0;
        curActionIdx   = 0;

        os_rw_lock_init(&m_lock);

        m_testTask1.setUp();
        m_testTask2.setUp();

        m_thread1 = std::thread(cmdHandlerTask, &m_testTask1);
        m_thread2 = std::thread(cmdHandlerTask, &m_testTask2);
    }

    void
    TearDown() override
    {
        m_testTask1.tearDown();
        m_testTask2.tearDown();
        m_thread1.join();
        m_thread2.join();
        os_rw_lock_deinit(&m_lock);
        g_pTestClass = nullptr;
    }

public:
    os_rw_lock_t      m_lock;
    TestTask          m_testTask1;
    TestTask          m_testTask2;
    std::thread       m_thread1;
    std::thread       m_thread2;
    const ActionDesc* p_arrOfActions;
    uint32_t          numActions;
    uint32_t          curActionIdx;

    TestOsRwLock();

    ~TestOsRwLock() override;
};

TestOsRwLock::TestOsRwLock()
    : Test()
{
}

TestOsRwLock::~TestOsRwLock() = default;

#ifdef __cplusplus
extern "C" {
#endif

os_mutex_t
os_mutex_create_static(os_mutex_static_t* const p_mutex_static)
{
    memset(p_mutex_static, 0, sizeof(*p_mutex_static));
    p_mutex_static->u.uxDummy2 = 0;
    return reinterpret_cast<os_mutex_t>(p_mutex_static);
}

void
os_mutex_delete(os_mutex_t* const ph_mutex)
{
    os_mutex_static_t* const p_mutex_static = reinterpret_cast<os_mutex_static_t*>(*ph_mutex);
    assert(0 == p_mutex_static->u.uxDummy2);
    memset(p_mutex_static, 0, sizeof(*p_mutex_static));
    *ph_mutex = nullptr;
}

bool
os_mutex_lock_with_timeout(os_mutex_t const h_mutex, const os_delta_ticks_t ticks_to_wait)
{
    os_mutex_static_t* const p_mutex_static = reinterpret_cast<os_mutex_static_t*>(h_mutex);

    if (0 != g_pTestClass->numActions)
    {
        assert(g_pTestClass->curActionIdx < g_pTestClass->numActions);
        const ActionDesc* const pCurAction = &g_pTestClass->p_arrOfActions[g_pTestClass->curActionIdx++];
        assert(ACTION_TYPE_MUTEX_LOCK_READER_WITH_TIMEOUT == pCurAction->actionType);
        assert(ticks_to_wait == pCurAction->param.lockReaderWithTimeout.ticksToWait);
        if (0 != pCurAction->param.lockReaderWithTimeout.sleep_ms)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(pCurAction->param.lockReaderWithTimeout.sleep_ms));
        }
    }
    if (0 == p_mutex_static->u.uxDummy2)
    {
        p_mutex_static->u.uxDummy2 += 1;
        return true;
    }

    return false;
}

void
os_mutex_lock(os_mutex_t const h_mutex)
{
    os_mutex_static_t* const p_mutex_static = reinterpret_cast<os_mutex_static_t*>(h_mutex);

    if (0 != g_pTestClass->numActions)
    {
        assert(g_pTestClass->curActionIdx < g_pTestClass->numActions);
        const ActionDesc* const pCurAction = &g_pTestClass->p_arrOfActions[g_pTestClass->curActionIdx++];
        assert(ACTION_TYPE_MUTEX_LOCK_READER == pCurAction->actionType);
    }

    assert(0 == p_mutex_static->u.uxDummy2);
    p_mutex_static->u.uxDummy2 += 1;
}

bool
os_mutex_try_lock(os_mutex_t const h_mutex)
{
    os_mutex_static_t* const p_mutex_static = reinterpret_cast<os_mutex_static_t*>(h_mutex);

    if (0 != g_pTestClass->numActions)
    {
        assert(g_pTestClass->curActionIdx < g_pTestClass->numActions);
        const ActionDesc* const pCurAction = &g_pTestClass->p_arrOfActions[g_pTestClass->curActionIdx++];
        assert(ACTION_TYPE_MUTEX_TRY_LOCK_READER == pCurAction->actionType);
        if (0 != pCurAction->param.tryLockReader.sleep_ms)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(pCurAction->param.tryLockReader.sleep_ms));
        }
    }
    if (0 == p_mutex_static->u.uxDummy2)
    {
        p_mutex_static->u.uxDummy2 += 1;
        return true;
    }
    return false;
}

void
os_mutex_unlock(os_mutex_t const h_mutex)
{
    os_mutex_static_t* const p_mutex_static = reinterpret_cast<os_mutex_static_t*>(h_mutex);

    if (0 != g_pTestClass->numActions)
    {
        assert(g_pTestClass->curActionIdx < g_pTestClass->numActions);
        const ActionDesc* const pCurAction = &g_pTestClass->p_arrOfActions[g_pTestClass->curActionIdx++];
        assert(ACTION_TYPE_MUTEX_UNLOCK_READER == pCurAction->actionType);
    }

    assert(1 == p_mutex_static->u.uxDummy2);
    p_mutex_static->u.uxDummy2 -= 1;
}

os_sema_t
os_sema_create_static(os_sema_static_t* const p_sema_static)
{
    memset(p_sema_static, 0, sizeof(*p_sema_static));
    p_sema_static->u.uxDummy2 = 1;
    return reinterpret_cast<os_sema_t>(p_sema_static);
}

void
os_sema_delete(os_sema_t* const ph_sema)
{
    os_sema_static_t* const p_sema_static = reinterpret_cast<os_sema_static_t*>(*ph_sema);
    memset(p_sema_static, 0, sizeof(*p_sema_static));
    *ph_sema = nullptr;
}

void
os_sema_wait_infinite(os_sema_t const h_sema)
{
    os_sema_static_t* const p_sema_static = reinterpret_cast<os_sema_static_t*>(h_sema);

    if (0 != g_pTestClass->numActions)
    {
        assert(g_pTestClass->curActionIdx < g_pTestClass->numActions);
        const ActionDesc* const pCurAction = &g_pTestClass->p_arrOfActions[g_pTestClass->curActionIdx++];
        assert(ACTION_TYPE_SEMA_WAIT_INFINITE_WRITER == pCurAction->actionType);
    }

    assert(0 == p_sema_static->u.uxDummy2);
    p_sema_static->u.uxDummy2 += 1;
}

bool
os_sema_wait_immediate(os_sema_t const h_sema)
{
    os_sema_static_t* const p_sema_static = reinterpret_cast<os_sema_static_t*>(h_sema);

    if (0 != g_pTestClass->numActions)
    {
        assert(g_pTestClass->curActionIdx < g_pTestClass->numActions);
        const ActionDesc* const pCurAction = &g_pTestClass->p_arrOfActions[g_pTestClass->curActionIdx++];
        assert(ACTION_TYPE_SEMA_WAIT_IMMEDIATE_WRITER == pCurAction->actionType);
        if (0 != pCurAction->param.semaWaitImmediate.sleep_ms)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(pCurAction->param.semaWaitImmediate.sleep_ms));
        }
    }
    if (0 == p_sema_static->u.uxDummy2)
    {
        p_sema_static->u.uxDummy2 += 1;
        return true;
    }
    return false;
}

void
os_sema_signal(os_sema_t const h_sema)
{
    os_sema_static_t* const p_sema_static = reinterpret_cast<os_sema_static_t*>(h_sema);

    if ((0 != g_pTestClass->numActions))
    {
        assert(g_pTestClass->curActionIdx < g_pTestClass->numActions);
        const ActionDesc* const pCurAction = &g_pTestClass->p_arrOfActions[g_pTestClass->curActionIdx++];
        assert(ACTION_TYPE_SEMA_SIGNAL_WRITER == pCurAction->actionType);
    }

    assert(1 == p_sema_static->u.uxDummy2);
    p_sema_static->u.uxDummy2 -= 1;
}

#ifdef __cplusplus
}
#endif

static void
cmdHandlerTask(TestTask* p_testTask)
{
    bool flagExit = false;
    while (!flagExit)
    {
        const test_task_cmd_e cmd = p_testTask->cmdQueue.pop();
        switch (cmd)
        {
            case TEST_TASK_CMD_EXIT:
                flagExit = true;
                break;
            case TEST_TASK_CMD_LOCK_WRITER:
                os_rw_lock_acquire_writer(&g_pTestClass->m_lock);
                p_testTask->res = true;
                break;
            case TEST_TASK_CMD_TRY_LOCK_WRITER:
                p_testTask->res = os_rw_lock_try_acquire_writer(&g_pTestClass->m_lock);
                break;
            case TEST_TASK_CMD_UNLOCK_WRITER:
                os_rw_lock_release_writer(&g_pTestClass->m_lock);
                break;
            case TEST_TASK_CMD_LOCK_READER:
                os_rw_lock_acquire_reader(&g_pTestClass->m_lock);
                p_testTask->res = true;
                break;
            case TEST_TASK_CMD_TRY_LOCK_READER:
                p_testTask->res = os_rw_lock_try_acquire_reader(&g_pTestClass->m_lock);
                break;
            case TEST_TASK_CMD_UNLOCK_READER:
                os_rw_lock_release_reader(&g_pTestClass->m_lock);
                break;
            default:
                printf("Error: Unknown cmd %d\n", (int)cmd);
                assert(0);
                break;
        }
        p_testTask->cmdQueue.notify_handled();
    }
}

/*** Unit-Tests
 * *******************************************************************************************************/

TEST_F(TestOsRwLock, test_lock_reader__lock_reader_success) // NOLINT
{
    this->m_testTask1.cmdQueue.push_and_wait(TEST_TASK_CMD_LOCK_READER);
    ASSERT_TRUE(this->m_testTask1.res);
    this->m_testTask2.cmdQueue.push_and_wait(TEST_TASK_CMD_LOCK_READER);
    ASSERT_TRUE(this->m_testTask2.res);
}

TEST_F(TestOsRwLock, test_lock_reader__try_lock_reader_success) // NOLINT
{
    this->m_testTask1.cmdQueue.push_and_wait(TEST_TASK_CMD_LOCK_READER);
    ASSERT_TRUE(this->m_testTask1.res);
    this->m_testTask2.cmdQueue.push_and_wait(TEST_TASK_CMD_TRY_LOCK_READER);
    ASSERT_TRUE(this->m_testTask2.res);
}

TEST_F(TestOsRwLock, test_try_lock_reader__lock_reader_success) // NOLINT
{
    this->m_testTask1.cmdQueue.push_and_wait(TEST_TASK_CMD_TRY_LOCK_READER);
    ASSERT_TRUE(this->m_testTask1.res);
    this->m_testTask2.cmdQueue.push_and_wait(TEST_TASK_CMD_LOCK_READER);
    ASSERT_TRUE(this->m_testTask2.res);
}

TEST_F(TestOsRwLock, test_try_lock_reader__try_lock_reader_success) // NOLINT
{
    this->m_testTask1.cmdQueue.push_and_wait(TEST_TASK_CMD_TRY_LOCK_READER);
    ASSERT_TRUE(this->m_testTask1.res);
    this->m_testTask2.cmdQueue.push_and_wait(TEST_TASK_CMD_TRY_LOCK_READER);
    ASSERT_TRUE(this->m_testTask2.res);
}

TEST_F(TestOsRwLock, test_try_lock_reader__try_lock_writer_fail) // NOLINT
{
    this->m_testTask1.cmdQueue.push_and_wait(TEST_TASK_CMD_TRY_LOCK_READER);
    ASSERT_TRUE(this->m_testTask1.res);
    this->m_testTask2.cmdQueue.push_and_wait(TEST_TASK_CMD_TRY_LOCK_WRITER);
    ASSERT_FALSE(this->m_testTask2.res);
}

TEST_F(TestOsRwLock, test_lock_writer__try_lock_reader_fail) // NOLINT
{
    this->m_testTask1.cmdQueue.push_and_wait(TEST_TASK_CMD_LOCK_WRITER);
    ASSERT_TRUE(this->m_testTask1.res);
    this->m_testTask2.cmdQueue.push_and_wait(TEST_TASK_CMD_TRY_LOCK_READER);
    ASSERT_FALSE(this->m_testTask2.res);
}

TEST_F(TestOsRwLock, test_try_lock_writer__try_lock_reader_fail) // NOLINT
{
    this->m_testTask1.cmdQueue.push_and_wait(TEST_TASK_CMD_TRY_LOCK_WRITER);
    ASSERT_TRUE(this->m_testTask1.res);
    this->m_testTask2.cmdQueue.push_and_wait(TEST_TASK_CMD_TRY_LOCK_READER);
    ASSERT_FALSE(this->m_testTask2.res);
}

TEST_F(TestOsRwLock, test_try_lock_writer__try_lock_writer_fail) // NOLINT
{
    this->m_testTask1.cmdQueue.push_and_wait(TEST_TASK_CMD_TRY_LOCK_WRITER);
    ASSERT_TRUE(this->m_testTask1.res);
    this->m_testTask2.cmdQueue.push_and_wait(TEST_TASK_CMD_TRY_LOCK_WRITER);
    ASSERT_FALSE(this->m_testTask2.res);
}

TEST_F(TestOsRwLock, test_lock_writer__try_lock_reader__unlock_writer__try_lock_reader) // NOLINT
{
    this->m_testTask1.cmdQueue.push_and_wait(TEST_TASK_CMD_LOCK_WRITER);
    ASSERT_TRUE(this->m_testTask1.res);
    this->m_testTask2.cmdQueue.push_and_wait(TEST_TASK_CMD_TRY_LOCK_READER);
    ASSERT_FALSE(this->m_testTask2.res);
    this->m_testTask1.cmdQueue.push_and_wait(TEST_TASK_CMD_UNLOCK_WRITER);
    this->m_testTask2.cmdQueue.push_and_wait(TEST_TASK_CMD_TRY_LOCK_READER);
    ASSERT_TRUE(this->m_testTask2.res);
}

TEST_F(TestOsRwLock, test_lock_reader__try_lock_writer__unlock_reader__try_lock_writer) // NOLINT
{
    this->m_testTask1.cmdQueue.push_and_wait(TEST_TASK_CMD_LOCK_READER);
    ASSERT_TRUE(this->m_testTask1.res);
    this->m_testTask2.cmdQueue.push_and_wait(TEST_TASK_CMD_TRY_LOCK_WRITER);
    ASSERT_FALSE(this->m_testTask2.res);
    this->m_testTask1.cmdQueue.push_and_wait(TEST_TASK_CMD_UNLOCK_READER);
    this->m_testTask2.cmdQueue.push_and_wait(TEST_TASK_CMD_TRY_LOCK_WRITER);
    ASSERT_TRUE(this->m_testTask2.res);
}

TEST_F(TestOsRwLock, test_sequence_try_lock_reader) // NOLINT
{
    static const std::array<ActionDesc, 3> actions = {{
        {
            .actionType = ACTION_TYPE_MUTEX_TRY_LOCK_READER,
            .param = { .tryLockReader = { .sleep_ms = 0, } },
        },
        {
            .actionType = ACTION_TYPE_SEMA_WAIT_IMMEDIATE_WRITER,
            .param = { .semaWaitImmediate = { .sleep_ms = 0, } },
        },
        {
            .actionType = ACTION_TYPE_MUTEX_UNLOCK_READER,
            .param = { },
        },
    }};
    this->p_arrOfActions = actions.data();
    this->numActions     = actions.size();
    this->m_testTask1.cmdQueue.push_and_wait(TEST_TASK_CMD_TRY_LOCK_READER);
    ASSERT_TRUE(this->m_testTask1.res);
}

TEST_F(TestOsRwLock, test_sequence_lock_writer__try_lock_reader_failed) // NOLINT
{
    static const std::array<ActionDesc, 4> actions = {{
        {
            .actionType = ACTION_TYPE_SEMA_WAIT_INFINITE_WRITER,
            .param = { },
        },
        {
            .actionType = ACTION_TYPE_MUTEX_TRY_LOCK_READER,
            .param = { .tryLockReader = { .sleep_ms = 0, } },
        },
        {
            .actionType = ACTION_TYPE_SEMA_WAIT_IMMEDIATE_WRITER,
            .param = { .semaWaitImmediate = { .sleep_ms = 0, } },
        },
        {
            .actionType = ACTION_TYPE_MUTEX_UNLOCK_READER,
            .param = { },
        },
    }};
    this->p_arrOfActions = actions.data();
    this->numActions     = actions.size();
    this->m_testTask1.cmdQueue.push_and_wait(TEST_TASK_CMD_LOCK_WRITER);
    this->m_testTask2.cmdQueue.push_and_wait(TEST_TASK_CMD_TRY_LOCK_READER);
    ASSERT_FALSE(this->m_testTask2.res);
}

TEST_F(TestOsRwLock, test_sequence_try_lock_reader__try_lock_reader) // NOLINT
{
    static const std::array<ActionDesc, 12> actions = {{
        {
            .actionType = ACTION_TYPE_MUTEX_TRY_LOCK_READER,
            .param = { .tryLockReader = { .sleep_ms = 0, } },
        },
        {
            .actionType = ACTION_TYPE_SEMA_WAIT_IMMEDIATE_WRITER,
            .param = { .semaWaitImmediate = { .sleep_ms = 200, } },
        },
        {
            .actionType = ACTION_TYPE_MUTEX_TRY_LOCK_READER,
            .param = { .tryLockReader = { .sleep_ms = 0, } },
        },

        // Cycle 1
        {
            .actionType = ACTION_TYPE_MUTEX_LOCK_READER_WITH_TIMEOUT,
            .param = { .lockReaderWithTimeout = { .ticksToWait = 1, .sleep_ms = 0, } },
        },
        {
            .actionType = ACTION_TYPE_SEMA_WAIT_IMMEDIATE_WRITER,
            .param = { .semaWaitImmediate = { .sleep_ms = 0, } },
        },
        {
            .actionType = ACTION_TYPE_SEMA_SIGNAL_WRITER,
            .param = { },
        },
        // Cycle 2
        {
            .actionType = ACTION_TYPE_MUTEX_LOCK_READER_WITH_TIMEOUT,
            .param = { .lockReaderWithTimeout = { .ticksToWait = 1, .sleep_ms = 0, } },
        },
        {
            .actionType = ACTION_TYPE_SEMA_WAIT_IMMEDIATE_WRITER,
            .param = { .semaWaitImmediate = { .sleep_ms = 0, } },
        },
        {
            .actionType = ACTION_TYPE_SEMA_SIGNAL_WRITER,
            .param = { },
        },
        // Cycle 3
        {
            .actionType = ACTION_TYPE_MUTEX_LOCK_READER_WITH_TIMEOUT,
            .param = { .lockReaderWithTimeout = { .ticksToWait = 1, .sleep_ms = 300, } },
        },

        {
            .actionType = ACTION_TYPE_MUTEX_UNLOCK_READER,
            .param = { },
        },
        {
            .actionType = ACTION_TYPE_MUTEX_UNLOCK_READER,
            .param = { },
        },
    }};
    this->p_arrOfActions = actions.data();
    this->numActions     = actions.size();
    this->m_testTask1.cmdQueue.push(TEST_TASK_CMD_TRY_LOCK_READER);
    this->m_testTask2.cmdQueue.push(TEST_TASK_CMD_TRY_LOCK_READER);
    this->m_testTask1.cmdQueue.wait_until_handled();
    this->m_testTask2.cmdQueue.wait_until_handled();
    ASSERT_TRUE(this->m_testTask1.res);
    ASSERT_TRUE(this->m_testTask2.res);
}

TEST_F(TestOsRwLock, test_sequence_lock_writer__try_lock_reader__try_lock_reader) // NOLINT
{
    static const std::array<ActionDesc, 7> actions = {{
        {
            .actionType = ACTION_TYPE_SEMA_WAIT_INFINITE_WRITER,
            .param = { },
        },
        {
            .actionType = ACTION_TYPE_MUTEX_TRY_LOCK_READER,
            .param = { .tryLockReader = { .sleep_ms = 0, } },
        },
        {
            .actionType = ACTION_TYPE_SEMA_WAIT_IMMEDIATE_WRITER,
            .param = { .semaWaitImmediate = { .sleep_ms = 200, } },
        },
        {
            .actionType = ACTION_TYPE_MUTEX_TRY_LOCK_READER,
            .param = { .tryLockReader = { .sleep_ms = 0, } },
        },

        {
            .actionType = ACTION_TYPE_MUTEX_LOCK_READER_WITH_TIMEOUT,
            .param = { .lockReaderWithTimeout = { .ticksToWait = 1, .sleep_ms = 0, } },
        },
        {
            .actionType = ACTION_TYPE_SEMA_WAIT_IMMEDIATE_WRITER,
            .param = { .semaWaitImmediate = { .sleep_ms = 0, } },
        },

        {
            .actionType = ACTION_TYPE_MUTEX_UNLOCK_READER,
            .param = { },
        },
    }};
    this->p_arrOfActions = actions.data();
    this->numActions     = actions.size();
    os_rw_lock_acquire_writer(&m_lock);
    this->m_testTask1.cmdQueue.push(TEST_TASK_CMD_TRY_LOCK_READER);
    this->m_testTask2.cmdQueue.push(TEST_TASK_CMD_TRY_LOCK_READER);
    this->m_testTask1.cmdQueue.wait_until_handled();
    this->m_testTask2.cmdQueue.wait_until_handled();
    ASSERT_FALSE(this->m_testTask1.res);
    ASSERT_FALSE(this->m_testTask2.res);
}
