/**
 * @file test_snprintf_with_esp_err_desc.cpp
 * @author TheSomeMan
 * @date 2023-04-09
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "gtest/gtest.h"
#include "snprintf_with_esp_err_desc.h"
#include <string>

using namespace std;

/*** Google-test class implementation *********************************************************************************/

class TestClass;
static TestClass* g_pTestClass;

class MemAllocTrace
{
    vector<void*> allocated_mem;

    std::vector<void*>::iterator
    find(void* p_mem)
    {
        for (auto iter = this->allocated_mem.begin(); iter != this->allocated_mem.end(); ++iter)
        {
            if (*iter == p_mem)
            {
                return iter;
            }
        }
        return this->allocated_mem.end();
    }

public:
    void
    add(void* p_mem)
    {
        auto iter = find(p_mem);
        assert(iter == this->allocated_mem.end()); // p_mem was found in the list of allocated memory blocks
        this->allocated_mem.push_back(p_mem);
    }
    void
    remove(void* p_mem)
    {
        auto iter = find(p_mem);
        assert(iter != this->allocated_mem.end()); // p_mem was not found in the list of allocated memory blocks
        this->allocated_mem.erase(iter);
    }
    bool
    is_empty()
    {
        return this->allocated_mem.empty();
    }
};

class TestClass : public ::testing::Test
{
private:
protected:
    void
    SetUp() override
    {
        g_pTestClass       = this;
        m_flag_malloc_fail = false;
        m_err_desc_extra   = nullptr;
    }

    void
    TearDown() override
    {
        g_pTestClass = nullptr;
    }

public:
    TestClass();

    ~TestClass() override;

    bool          m_flag_malloc_fail;
    const char*   m_err_desc_extra;
    MemAllocTrace m_mem_alloc_trace;
};

TestClass::TestClass()
    : Test()
    , m_flag_malloc_fail(false)
    , m_err_desc_extra(nullptr)
    , m_mem_alloc_trace()
{
}

TestClass::~TestClass() = default;

extern "C" {

void*
os_malloc(size_t size)
{
    if (g_pTestClass->m_flag_malloc_fail)
    {
        return nullptr;
    }
    void* p_buf = malloc(size);
    g_pTestClass->m_mem_alloc_trace.add(p_buf);
    return p_buf;
}

void
os_free_internal(void* p_buf)
{
    g_pTestClass->m_mem_alloc_trace.remove(p_buf);
    free(p_buf);
}

const char*
wrap_esp_err_to_name_r(const esp_err_t code, char* const p_buf, const size_t buf_len)
{
    (void)snprintf(
        p_buf,
        buf_len,
        "Error description for code %d%s",
        code,
        (nullptr != g_pTestClass->m_err_desc_extra) ? g_pTestClass->m_err_desc_extra : "");
    return p_buf;
}

} // extern "C"

/*** Unit-Tests *******************************************************************************************************/

TEST_F(TestClass, test_esp_err_to_name_with_alloc_str_buf__ok) // NOLINT
{
    const esp_err_t esp_err_code = ESP_ERR_INVALID_ARG;
    str_buf_t       str_buf      = esp_err_to_name_with_alloc_str_buf(esp_err_code);
    ASSERT_NE(nullptr, str_buf.buf);
    ASSERT_EQ(string("Error description for code 258"), str_buf.buf);
    ASSERT_EQ(30, str_buf.idx);
    str_buf_free_buf(&str_buf);
}

TEST_F(TestClass, test_esp_err_to_name_with_alloc_str_buf__fail) // NOLINT
{
    this->m_flag_malloc_fail     = true;
    const esp_err_t esp_err_code = ESP_ERR_INVALID_ARG;
    const str_buf_t str_buf      = esp_err_to_name_with_alloc_str_buf(esp_err_code);
    ASSERT_EQ(nullptr, str_buf.buf);
    ASSERT_EQ(0, str_buf.idx);
}

TEST_F(TestClass, test_esp_err_to_name_with_alloc_str_buf__err_desc_too_big) // NOLINT
{
    this->m_err_desc_extra
        = " 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567A";
    const esp_err_t esp_err_code = ESP_ERR_INVALID_ARG;
    str_buf_t       str_buf      = esp_err_to_name_with_alloc_str_buf(esp_err_code);
    ASSERT_NE(nullptr, str_buf.buf);
    ASSERT_EQ(
        string("Error description for code 258 "
               "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567"),
        str_buf.buf);
    ASSERT_EQ(119, str_buf.idx);
    str_buf_free_buf(&str_buf);
}

TEST_F(TestClass, test_snprintf_with_esp_err_desc__ok) // NOLINT
{
    const esp_err_t esp_err_code = ESP_ERR_INVALID_ARG;
    char            buf[128];

    const int res = snprintf_with_esp_err_desc(esp_err_code, buf, sizeof(buf), "Message %d", 123);
    ASSERT_EQ(55, res);
    ASSERT_EQ(string("Message 123, error 258 (Error description for code 258)"), buf);
    ASSERT_TRUE(this->m_mem_alloc_trace.is_empty());
}

TEST_F(TestClass, test_snprintf_with_esp_err_desc__calc_size_without_printing) // NOLINT
{
    const esp_err_t esp_err_code = ESP_ERR_INVALID_ARG;

    const int res = snprintf_with_esp_err_desc(esp_err_code, nullptr, 0, "Message %d", 123);
    ASSERT_EQ(55, res);
    ASSERT_TRUE(this->m_mem_alloc_trace.is_empty());
}

TEST_F(TestClass, test_snprintf_with_esp_err_desc__ok_with_null_fmt) // NOLINT
{
    const esp_err_t esp_err_code = ESP_ERR_INVALID_ARG;
    char            buf[128];

    const int res = snprintf_with_esp_err_desc(esp_err_code, buf, sizeof(buf), nullptr);
    ASSERT_EQ(42, res);
    ASSERT_EQ(string("error 258 (Error description for code 258)"), buf);
    ASSERT_TRUE(this->m_mem_alloc_trace.is_empty());
}

TEST_F(TestClass, test_snprintf_with_esp_err_desc__ok_with_empty_message) // NOLINT
{
    const esp_err_t esp_err_code = ESP_ERR_INVALID_ARG;
    char            buf[128];

    const int res = snprintf_with_esp_err_desc(esp_err_code, buf, sizeof(buf), "%s", "");
    ASSERT_EQ(42, res);
    ASSERT_EQ(string("error 258 (Error description for code 258)"), buf);
    ASSERT_TRUE(this->m_mem_alloc_trace.is_empty());
}

TEST_F(TestClass, test_snprintf_with_esp_err_desc__malloc_failed) // NOLINT
{
    this->m_flag_malloc_fail     = true;
    const esp_err_t esp_err_code = ESP_ERR_INVALID_ARG;
    char            buf[128];

    const int res = snprintf_with_esp_err_desc(esp_err_code, buf, sizeof(buf), "Message %d", 123);
    ASSERT_EQ(22, res);
    ASSERT_EQ(string("Message 123, error 258"), buf);
    ASSERT_TRUE(this->m_mem_alloc_trace.is_empty());
}

TEST_F(TestClass, test_snprintf_with_esp_err_desc__insufficient_buffer) // NOLINT
{
    const esp_err_t esp_err_code = ESP_ERR_INVALID_ARG;
    char            buf[55];

    const int res = snprintf_with_esp_err_desc(esp_err_code, buf, sizeof(buf), "Message %d", 123);
    ASSERT_EQ(55, res);
    ASSERT_EQ(string("Message 123, error 258 (Error description for code 258"), buf);
    ASSERT_TRUE(this->m_mem_alloc_trace.is_empty());
}

TEST_F(TestClass, test_snprintf_with_esp_err_desc__too_big_err_desc) // NOLINT
{
    this->m_err_desc_extra
        = " 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567A";
    const esp_err_t esp_err_code = ESP_ERR_INVALID_ARG;
    char            buf[256];

    const int res = snprintf_with_esp_err_desc(esp_err_code, buf, sizeof(buf), "Message %d", 123);
    ASSERT_EQ(144, res);
    ASSERT_EQ(
        string("Message 123, error 258 (Error description for code 258 "
               "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567)"),
        buf);
    ASSERT_TRUE(this->m_mem_alloc_trace.is_empty());
}
