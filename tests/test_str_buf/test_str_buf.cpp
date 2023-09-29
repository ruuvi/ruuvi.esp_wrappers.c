/**
 * @file test_str_buf.cpp
 * @author TheSomeMan
 * @date 2020-08-24
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "gtest/gtest.h"
#include "str_buf.h"
#include <string>

using namespace std;

/*** Google-test class implementation *********************************************************************************/

class TestStrBuf;
static TestStrBuf* g_pTestClass;

class TestStrBuf : public ::testing::Test
{
private:
protected:
    void
    SetUp() override
    {
        g_pTestClass       = this;
        m_flag_malloc_fail = false;
    }

    void
    TearDown() override
    {
        g_pTestClass = nullptr;
    }

public:
    TestStrBuf();

    ~TestStrBuf() override;

    bool m_flag_malloc_fail;
};

TestStrBuf::TestStrBuf()
    : Test()
    , m_flag_malloc_fail(false)
{
}

TestStrBuf::~TestStrBuf() = default;

extern "C" {

void*
os_malloc(size_t size)
{
    if (g_pTestClass->m_flag_malloc_fail)
    {
        return nullptr;
    }
    void* p_buf = malloc(size);
    return p_buf;
}

void
os_free_internal(void* p_buf)
{
    free(p_buf);
}

} // extern "C"

/*** Unit-Tests *******************************************************************************************************/

TEST_F(TestStrBuf, test_macro_str_buf_init) // NOLINT
{
    char tmp_buf[10] = "undef";

    str_buf_t str_buf = STR_BUF_INIT(tmp_buf, sizeof(tmp_buf));

    ASSERT_EQ(&tmp_buf[0], str_buf.buf);
    ASSERT_EQ(sizeof(tmp_buf), str_buf.size);
    ASSERT_EQ(0, str_buf.idx);
}

TEST_F(TestStrBuf, test_macro_str_buf_init_with_arr) // NOLINT
{
    char tmp_buf[10] = "undef";

    str_buf_t str_buf = STR_BUF_INIT_WITH_ARR(tmp_buf);

    ASSERT_EQ(&tmp_buf[0], str_buf.buf);
    ASSERT_EQ(sizeof(tmp_buf), str_buf.size);
    ASSERT_EQ(0, str_buf.idx);
}

TEST_F(TestStrBuf, test_macro_str_buf_init_null) // NOLINT
{
    str_buf_t str_buf = STR_BUF_INIT_NULL();

    ASSERT_EQ(nullptr, str_buf.buf);
    ASSERT_EQ(0, str_buf.size);
    ASSERT_EQ(0, str_buf.idx);
}

TEST_F(TestStrBuf, test_free_null_buffer) // NOLINT
{
    str_buf_t str_buf = STR_BUF_INIT_NULL();

    ASSERT_EQ(nullptr, str_buf.buf);
    ASSERT_EQ(0, str_buf.size);
    ASSERT_EQ(0, str_buf.idx);

    str_buf_free_buf(&str_buf);

    ASSERT_EQ(nullptr, str_buf.buf);
    ASSERT_EQ(0, str_buf.size);
    ASSERT_EQ(0, str_buf.idx);
}

TEST_F(TestStrBuf, test_str_buf_init) // NOLINT
{
    char tmp_buf[10] = "undef";

    str_buf_t str_buf  = STR_BUF_INIT_WITH_ARR(tmp_buf);
    str_buf_t str_buf2 = str_buf_init(tmp_buf, sizeof(tmp_buf));
    ASSERT_EQ(str_buf.buf, str_buf2.buf);
    ASSERT_EQ(str_buf.size, str_buf2.size);
    ASSERT_EQ(str_buf.idx, str_buf2.idx);

    ASSERT_EQ(&tmp_buf[0], str_buf.buf);
    ASSERT_EQ(sizeof(tmp_buf), str_buf.size);
    ASSERT_EQ(0, str_buf.idx);

    ASSERT_FALSE(str_buf_is_overflow(&str_buf));
}

TEST_F(TestStrBuf, test_str_buf_init_null) // NOLINT
{
    str_buf_t str_buf = str_buf_init_null();
    ASSERT_EQ(nullptr, str_buf.buf);
    ASSERT_EQ(0, str_buf.size);
    ASSERT_EQ(0, str_buf.idx);
}

TEST_F(TestStrBuf, test_str_buf_init_with_alloc_and_free_buf) // NOLINT
{
    const size_t len_of_str = 10;
    str_buf_t    str_buf    = {
              .buf  = nullptr,
              .size = 0,
              .idx  = len_of_str,
    };
    ASSERT_TRUE(str_buf_init_with_alloc(&str_buf));
    ASSERT_NE(nullptr, str_buf.buf);
    ASSERT_EQ(len_of_str + 1, str_buf.size);
    ASSERT_EQ(0, str_buf.idx);

    str_buf_free_buf(&str_buf);
    ASSERT_EQ(nullptr, str_buf.buf);
    ASSERT_EQ(0, str_buf.size);
    ASSERT_EQ(0, str_buf.idx);
}

TEST_F(TestStrBuf, test_str_buf_init_with_alloc_failed) // NOLINT
{
    const size_t len_of_str = 10;
    str_buf_t    str_buf    = {
              .buf  = nullptr,
              .size = 0,
              .idx  = len_of_str,
    };
    this->m_flag_malloc_fail = true;
    ASSERT_FALSE(str_buf_init_with_alloc(&str_buf));
}

TEST_F(TestStrBuf, test_simple_print) // NOLINT
{
    char tmp_buf[10] = "undef";

    str_buf_t str_buf = STR_BUF_INIT_WITH_ARR(tmp_buf);
    ASSERT_TRUE(str_buf_printf(&str_buf, "abc"));
    ASSERT_EQ(string("abc"), string(tmp_buf));
    ASSERT_EQ(strlen("abc"), str_buf_get_len(&str_buf));
    ASSERT_FALSE(str_buf_is_overflow(&str_buf));
}

TEST_F(TestStrBuf, test_print_twice) // NOLINT
{
    char tmp_buf[10] = "undef";

    str_buf_t str_buf = STR_BUF_INIT_WITH_ARR(tmp_buf);
    ASSERT_TRUE(str_buf_printf(&str_buf, "abc"));
    ASSERT_TRUE(str_buf_printf(&str_buf, "def"));
    ASSERT_EQ(string("abcdef"), string(tmp_buf));
    ASSERT_EQ(strlen("abcdef"), str_buf_get_len(&str_buf));
    ASSERT_FALSE(str_buf_is_overflow(&str_buf));
}

TEST_F(TestStrBuf, test_printf) // NOLINT
{
    char tmp_buf[10] = "undef";

    str_buf_t str_buf = STR_BUF_INIT_WITH_ARR(tmp_buf);
    ASSERT_TRUE(str_buf_printf(&str_buf, "%s %d", "xyz", 123));
    ASSERT_EQ(string("xyz 123"), string(tmp_buf));
    ASSERT_EQ(strlen("xyz 123"), str_buf_get_len(&str_buf));
    ASSERT_FALSE(str_buf_is_overflow(&str_buf));
}

TEST_F(TestStrBuf, test_printf_full_buf) // NOLINT
{
    char tmp_buf[10] = "undef";

    str_buf_t str_buf = STR_BUF_INIT_WITH_ARR(tmp_buf);
    ASSERT_TRUE(str_buf_printf(&str_buf, "%s %d", "abcdef", 12));
    ASSERT_EQ(string("abcdef 12"), string(tmp_buf));
    ASSERT_EQ(9, str_buf_get_len(&str_buf));
    ASSERT_FALSE(str_buf_is_overflow(&str_buf));

    ASSERT_TRUE(str_buf_printf(&str_buf, "%s", ""));
    ASSERT_EQ(string("abcdef 12"), string(tmp_buf));
    ASSERT_EQ(9, str_buf_get_len(&str_buf));
    ASSERT_FALSE(str_buf_is_overflow(&str_buf));
}

TEST_F(TestStrBuf, test_printf_overflow_1) // NOLINT
{
    char tmp_buf[10] = "undef";

    str_buf_t str_buf = STR_BUF_INIT_WITH_ARR(tmp_buf);
    ASSERT_FALSE(str_buf_printf(&str_buf, "%s %d", "abcdef", 123));
    ASSERT_EQ(string("abcdef 12"), string(tmp_buf));
    ASSERT_EQ(10, str_buf_get_len(&str_buf));
    ASSERT_TRUE(str_buf_is_overflow(&str_buf));
}

TEST_F(TestStrBuf, test_printf_overflow_2) // NOLINT
{
    char tmp_buf[10] = "undef";

    str_buf_t str_buf = STR_BUF_INIT_WITH_ARR(tmp_buf);
    ASSERT_FALSE(str_buf_printf(&str_buf, "%s %d", "abcdef", 1234));
    ASSERT_EQ(string("abcdef 12"), string(tmp_buf));
    ASSERT_EQ(10, str_buf_get_len(&str_buf));
    ASSERT_TRUE(str_buf_is_overflow(&str_buf));

    ASSERT_FALSE(str_buf_printf(&str_buf, "Q"));
    ASSERT_EQ(string("abcdef 12"), string(tmp_buf));
    ASSERT_EQ(10, str_buf_get_len(&str_buf));
    ASSERT_TRUE(str_buf_is_overflow(&str_buf));
}

TEST_F(TestStrBuf, test_calc_length) // NOLINT
{
    str_buf_t str_buf = STR_BUF_INIT(nullptr, 0);
    ASSERT_TRUE(str_buf_printf(&str_buf, "abc"));
    ASSERT_EQ(strlen("abc"), str_buf_get_len(&str_buf));
    ASSERT_FALSE(str_buf_is_overflow(&str_buf));

    ASSERT_TRUE(str_buf_printf(&str_buf, "def"));
    ASSERT_EQ(strlen("abcdef"), str_buf_get_len(&str_buf));
    ASSERT_FALSE(str_buf_is_overflow(&str_buf));
}

TEST_F(TestStrBuf, test_null_str_buf) // NOLINT
{
    str_buf_t* p_str_buf = nullptr;
    ASSERT_FALSE(str_buf_printf(p_str_buf, "abc"));
}

TEST_F(TestStrBuf, test_incorrect_str_buf_null_buf) // NOLINT
{
    char tmp_buf[10] = "undef";

    str_buf_t str_buf = STR_BUF_INIT(nullptr, sizeof(tmp_buf));
    ASSERT_FALSE(str_buf_printf(&str_buf, "abc"));
    ASSERT_EQ(0, str_buf_get_len(&str_buf));
    ASSERT_FALSE(str_buf_is_overflow(&str_buf));
    ASSERT_EQ(string("undef"), string(tmp_buf));
}

TEST_F(TestStrBuf, test_incorrect_str_buf_zero_size) // NOLINT
{
    char tmp_buf[10] = "undef";

    str_buf_t str_buf = STR_BUF_INIT(tmp_buf, 0);
    ASSERT_FALSE(str_buf_printf(&str_buf, "abc"));
    ASSERT_EQ(0, str_buf_get_len(&str_buf));
    ASSERT_FALSE(str_buf_is_overflow(&str_buf));
    ASSERT_EQ(string("undef"), string(tmp_buf));
}

TEST_F(TestStrBuf, test_fmt_null) // NOLINT
{
    char tmp_buf[10] = "undef";

    str_buf_t   str_buf  = STR_BUF_INIT(tmp_buf, sizeof(tmp_buf));
    const char* fmt_null = nullptr;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
    ASSERT_FALSE(str_buf_printf(&str_buf, fmt_null));
#pragma GCC diagnostic pop
    ASSERT_EQ(0, str_buf_get_len(&str_buf));
    ASSERT_FALSE(str_buf_is_overflow(&str_buf));
    ASSERT_EQ(string("undef"), string(tmp_buf));
}

TEST_F(TestStrBuf, test_negative_snprintf) // NOLINT
{
    char tmp_buf[20] = "undef";

    str_buf_t str_buf = STR_BUF_INIT(tmp_buf, sizeof(tmp_buf));
    ASSERT_FALSE(str_buf_printf(&str_buf, "TestABC %ls", L"\U10FFFFFF"));
    ASSERT_EQ(sizeof(tmp_buf), str_buf_get_len(&str_buf));
    ASSERT_TRUE(str_buf_is_overflow(&str_buf));
    ASSERT_EQ(string("TestABC "), string(tmp_buf));
}

TEST_F(TestStrBuf, test_printf_abc_123_qwe_456) // NOLINT
{
    str_buf_t str_buf = str_buf_printf_with_alloc("abc %d %s %d", 123, "qwe", 456);
    ASSERT_NE(nullptr, str_buf.buf);
    const string exp_str("abc 123 qwe 456");
    ASSERT_EQ(string(str_buf.buf), exp_str);
    ASSERT_EQ(str_buf.idx, exp_str.length());
    ASSERT_EQ(str_buf.size, exp_str.length() + 1);
    str_buf_free_buf(&str_buf);
    ASSERT_EQ(nullptr, str_buf.buf);
    ASSERT_EQ(0, str_buf.size);
    ASSERT_EQ(0, str_buf.idx);
}

TEST_F(TestStrBuf, test_printf_abc_123_malloc_failed) // NOLINT
{
    this->m_flag_malloc_fail = true;

    str_buf_t str_buf = str_buf_printf_with_alloc("abc %d", 123);
    ASSERT_EQ(nullptr, str_buf.buf);
}

TEST_F(TestStrBuf, test_printf_negative_snprintf) // NOLINT
{
    str_buf_t str_buf = str_buf_printf_with_alloc("TestABC %ls", L"\U10FFFFFF");
    ASSERT_EQ(nullptr, str_buf.buf);
}

TEST_F(TestStrBuf, test_bin_to_hex) // NOLINT
{
    std::array<char, 11> tmp_buf = { 'q', '\0' };

    std::array<uint8_t, 5> bin_buf = { 0x11U, 0xaaU, 0xffU, 0x00U, 0x80U };
    str_buf_t              str_buf = STR_BUF_INIT(tmp_buf.begin(), tmp_buf.size());
    ASSERT_TRUE(str_buf_bin_to_hex(&str_buf, bin_buf.cbegin(), bin_buf.size()));
    const string exp_str("11aaff0080");
    ASSERT_EQ(string(str_buf.buf), exp_str);
    ASSERT_EQ(str_buf.idx, exp_str.length());
}

TEST_F(TestStrBuf, test_bin_to_hex_insufficient_output_buf_1) // NOLINT
{
    std::array<char, 10> tmp_buf = { 'q', '\0' };

    std::array<uint8_t, 5> bin_buf = { 0x11U, 0xaaU, 0xffU, 0x00U, 0x80U };
    str_buf_t              str_buf = STR_BUF_INIT(tmp_buf.begin(), tmp_buf.size());
    ASSERT_FALSE(str_buf_bin_to_hex(&str_buf, bin_buf.cbegin(), bin_buf.size()));
    const string exp_str("11aaff00");
    ASSERT_EQ(string(str_buf.buf), exp_str);
    ASSERT_EQ(str_buf.idx, exp_str.length());
}

TEST_F(TestStrBuf, test_bin_to_hex_insufficient_output_buf_2) // NOLINT
{
    std::array<char, 9> tmp_buf = { 'q', '\0' };

    std::array<uint8_t, 5> bin_buf = { 0x11U, 0xaaU, 0xffU, 0x00U, 0x80U };
    str_buf_t              str_buf = STR_BUF_INIT(tmp_buf.begin(), tmp_buf.size());
    ASSERT_FALSE(str_buf_bin_to_hex(&str_buf, bin_buf.cbegin(), bin_buf.size()));
    const string exp_str("11aaff00");
    ASSERT_EQ(string(str_buf.buf), exp_str);
    ASSERT_EQ(str_buf.idx, exp_str.length());
}

TEST_F(TestStrBuf, test_bin_to_hex_with_prefix_and_suffix) // NOLINT
{
    std::array<char, 19> tmp_buf = { 'q', '\0' };

    std::array<uint8_t, 5> bin_buf = { 0x11U, 0xaaU, 0xffU, 0x00U, 0x80U };
    str_buf_t              str_buf = STR_BUF_INIT(tmp_buf.begin(), tmp_buf.size());
    ASSERT_TRUE(str_buf_printf(&str_buf, "pre:"));
    ASSERT_TRUE(str_buf_bin_to_hex(&str_buf, bin_buf.cbegin(), bin_buf.size()));
    ASSERT_TRUE(str_buf_printf(&str_buf, ":sfx"));
    const string exp_str("pre:11aaff0080:sfx");
    ASSERT_EQ(string(str_buf.buf), exp_str);
    ASSERT_EQ(str_buf.idx, exp_str.length());
}

TEST_F(TestStrBuf, test_bin_to_hex_null_buf_nonzero_size) // NOLINT
{
    std::array<char, 11> tmp_buf = { 'q', '\0' };

    std::array<uint8_t, 5> bin_buf = { 0x11U, 0xaaU, 0xffU, 0x00U, 0x80U };
    str_buf_t              str_buf = STR_BUF_INIT(nullptr, tmp_buf.size());
    ASSERT_FALSE(str_buf_bin_to_hex(&str_buf, bin_buf.cbegin(), bin_buf.size()));
    ASSERT_EQ(nullptr, str_buf.buf);
    ASSERT_EQ(0, str_buf.idx);
}

TEST_F(TestStrBuf, test_bin_to_hex_nonnull_buf_zero_size) // NOLINT
{
    std::array<char, 11> tmp_buf = { 'q', '\0' };

    std::array<uint8_t, 5> bin_buf = { 0x11U, 0xaaU, 0xffU, 0x00U, 0x80U };
    str_buf_t              str_buf = STR_BUF_INIT(tmp_buf.begin(), 0);
    ASSERT_FALSE(str_buf_bin_to_hex(&str_buf, bin_buf.cbegin(), bin_buf.size()));
    ASSERT_EQ(0, str_buf.idx);
}

TEST_F(TestStrBuf, test_bin_to_hex_overflow) // NOLINT
{
    std::array<char, 11> tmp_buf = { 'q', '\0' };

    std::array<uint8_t, 5> bin_buf = { 0x11U, 0xaaU, 0xffU, 0x00U, 0x80U };
    str_buf_t              str_buf = STR_BUF_INIT(tmp_buf.begin(), tmp_buf.size());
    ASSERT_FALSE(str_buf_printf(&str_buf, "12345678901"));
    const string exp_str("1234567890");
    ASSERT_EQ(exp_str, string(str_buf.buf));
    ASSERT_EQ(exp_str.length() + 1, str_buf.idx);

    ASSERT_FALSE(str_buf_bin_to_hex(&str_buf, bin_buf.cbegin(), bin_buf.size()));
    ASSERT_EQ(exp_str, string(str_buf.buf));
    ASSERT_EQ(exp_str.length() + 1, str_buf.idx);
}

TEST_F(TestStrBuf, test_bin_to_hex_with_alloc) // NOLINT
{
    std::array<uint8_t, 5> bin_buf = { 0x11U, 0xaaU, 0xffU, 0x00U, 0x80U };
    str_buf_t              str_buf = str_buf_bin_to_hex_with_alloc(bin_buf.begin(), bin_buf.size());
    ASSERT_NE(nullptr, str_buf.buf);
    const string exp_str("11aaff0080");
    ASSERT_EQ(string(str_buf.buf), exp_str);
    ASSERT_EQ(str_buf.idx, exp_str.length());
    ASSERT_EQ(str_buf.size, exp_str.length() + 1);
    str_buf_free_buf(&str_buf);
    ASSERT_EQ(nullptr, str_buf.buf);
    ASSERT_EQ(0, str_buf.size);
    ASSERT_EQ(0, str_buf.idx);
}
