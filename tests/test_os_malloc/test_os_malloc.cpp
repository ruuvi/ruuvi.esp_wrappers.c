/**
 * @file test_os_malloc.cpp
 * @author TheSomeMan
 * @date 2020-10-02
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "os_malloc.h"
#include "gtest/gtest.h"
#include <string>

using namespace std;

/*** Google-test class implementation
 * *********************************************************************************/

class TestOsMalloc : public ::testing::Test
{
private:
protected:
    void
    SetUp() override
    {
    }

    void
    TearDown() override
    {
    }

public:
    TestOsMalloc();

    ~TestOsMalloc() override;
};

TestOsMalloc::TestOsMalloc()
    : Test()
{
}

TestOsMalloc::~TestOsMalloc() = default;

/*** Unit-Tests
 * *******************************************************************************************************/

TEST_F(TestOsMalloc, test_os_malloc_os_free) // NOLINT
{
    void *ptr = os_malloc(1000);
    ASSERT_NE(nullptr, ptr);
    os_free(ptr);
    ASSERT_EQ(nullptr, ptr);
}

TEST_F(TestOsMalloc, test_os_calloc) // NOLINT
{
    void *ptr = os_calloc(sizeof(uint32_t), 1000);
    ASSERT_NE(nullptr, ptr);
    os_free(ptr);
    ASSERT_EQ(nullptr, ptr);
}

TEST_F(TestOsMalloc, test_os_realloc) // NOLINT
{
    void *ptr = os_malloc(4);
    ASSERT_NE(nullptr, ptr);
    void *ptr2 = os_malloc(4);
    ASSERT_NE(nullptr, ptr2);
    const void *const saved_ptr = ptr;
    ASSERT_TRUE(os_realloc_safe((void **)&ptr, 1 * 1024));
    ASSERT_NE(nullptr, ptr);
    ASSERT_NE(saved_ptr, ptr);
    os_free(ptr);
    ASSERT_EQ(nullptr, ptr);
    os_free(ptr2);
    ASSERT_EQ(nullptr, ptr2);
}
