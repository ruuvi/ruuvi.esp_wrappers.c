/**
 * @file test_log_dump.cpp
 * @author TheSomeMan
 * @date 2021-02-07
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "gtest/gtest.h"
#include <string>
#include "esp_log_wrapper.hpp"

#define LOG_LOCAL_LEVEL LOG_LEVEL_VERBOSE
#include "log.h"

static const char *TAG = "test";

using namespace std;

/*** Google-test class implementation *********************************************************************************/

class TestLogDump;
static TestLogDump *g_pTestClass;

class TestLogDump : public ::testing::Test
{
private:
protected:
    void
    SetUp() override
    {
        esp_log_wrapper_init();
        g_pTestClass       = this;
        m_flag_malloc_fail = false;
    }

    void
    TearDown() override
    {
        esp_log_wrapper_deinit();
        g_pTestClass = nullptr;
    }

public:
    TestLogDump();

    ~TestLogDump() override;

    bool m_flag_malloc_fail;
};

TestLogDump::TestLogDump()
    : Test()
    , m_flag_malloc_fail(false)
{
}

TestLogDump::~TestLogDump() = default;

extern "C" {

void *
os_malloc(size_t size)
{
    if (g_pTestClass->m_flag_malloc_fail)
    {
        return nullptr;
    }
    void *p_buf = malloc(size);
    return p_buf;
}

void
os_free_internal(void *p_buf)
{
    free(p_buf);
}

const char *
os_task_get_name(void)
{
    static const char g_task_name[] = "thread_name";
    return const_cast<char *>(g_task_name);
}

os_task_priority_t
os_task_get_priority(void)
{
    return 0;
}

} // extern "C"

/*** Unit-Tests *******************************************************************************************************/

#define TEST_CHECK_LOG_RECORD(level_, msg_) ESP_LOG_WRAPPER_TEST_CHECK_LOG_RECORD("test", level_, msg_)

TEST_F(TestLogDump, test_null_buf) // NOLINT
{
    LOG_DUMP_INFO(nullptr, 0, "test log dump %d", 123);
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "test log dump 123:");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}

TEST_F(TestLogDump, test_buf_size_0) // NOLINT
{
    const std::array<uint8_t, 0> buf = {};
    LOG_DUMP_INFO(buf.data(), buf.size(), "test log dump %d", 123);
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "test log dump 123:");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}

TEST_F(TestLogDump, test_buf_size_1) // NOLINT
{
    const std::array<uint8_t, 1> buf = { '0' };
    LOG_DUMP_INFO(buf.data(), buf.size(), "test log dump %d", 123);
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "test log dump 123:");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0000: 30                                              | 0\n");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}

TEST_F(TestLogDump, test_buf_size_2) // NOLINT
{
    const std::array<uint8_t, 2> buf = { '0', '1' };
    LOG_DUMP_INFO(buf.data(), buf.size(), "test log dump %d", 123);
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "test log dump 123:");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0000: 30 31                                           | 01\n");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}

TEST_F(TestLogDump, test_buf_size_3) // NOLINT
{
    const std::array<uint8_t, 3> buf = { '0', '1', '2' };
    LOG_DUMP_INFO(buf.data(), buf.size(), "test log dump %d", 123);
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "test log dump 123:");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0000: 30 31 32                                        | 012\n");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}

TEST_F(TestLogDump, test_buf_size_4) // NOLINT
{
    const std::array<uint8_t, 4> buf = { '0', '1', '2', '3' };
    LOG_DUMP_INFO(buf.data(), buf.size(), "test log dump %d", 123);
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "test log dump 123:");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0000: 30 31 32 33                                     | 0123\n");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}

TEST_F(TestLogDump, test_buf_size_5) // NOLINT
{
    const std::array<uint8_t, 5> buf = { '0', '1', '2', '3', '4' };
    LOG_DUMP_INFO(buf.data(), buf.size(), "test log dump %d", 123);
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "test log dump 123:");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0000: 30 31 32 33 34                                  | 01234\n");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}

TEST_F(TestLogDump, test_buf_size_6) // NOLINT
{
    const std::array<uint8_t, 6> buf = { '0', '1', '2', '3', '4', '5' };
    LOG_DUMP_INFO(buf.data(), buf.size(), "test log dump %d", 123);
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "test log dump 123:");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0000: 30 31 32 33 34 35                               | 012345\n");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}

TEST_F(TestLogDump, test_buf_size_7) // NOLINT
{
    const std::array<uint8_t, 7> buf = { '0', '1', '2', '3', '4', '5', '6' };
    LOG_DUMP_INFO(buf.data(), buf.size(), "test log dump %d", 123);
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "test log dump 123:");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0000: 30 31 32 33 34 35 36                            | 0123456\n");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}

TEST_F(TestLogDump, test_buf_size_8) // NOLINT
{
    const std::array<uint8_t, 8> buf = { '0', '1', '2', '3', '4', '5', '6', '7' };
    LOG_DUMP_INFO(buf.data(), buf.size(), "test log dump %d", 123);
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "test log dump 123:");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0000: 30 31 32 33 34 35 36 37                         | 01234567\n");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}

TEST_F(TestLogDump, test_buf_size_9) // NOLINT
{
    const std::array<uint8_t, 9> buf = { '0', '1', '2', '3', '4', '5', '6', '7', '8' };
    LOG_DUMP_INFO(buf.data(), buf.size(), "test log dump %d", 123);
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "test log dump 123:");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0000: 30 31 32 33 34 35 36 37 38                      | 012345678\n");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}

TEST_F(TestLogDump, test_buf_size_10) // NOLINT
{
    const std::array<uint8_t, 10> buf = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
    LOG_DUMP_INFO(buf.data(), buf.size(), "test log dump %d", 123);
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "test log dump 123:");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0000: 30 31 32 33 34 35 36 37 38 39                   | 0123456789\n");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}

TEST_F(TestLogDump, test_buf_size_11) // NOLINT
{
    const std::array<uint8_t, 11> buf = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A' };
    LOG_DUMP_INFO(buf.data(), buf.size(), "test log dump %d", 123);
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "test log dump 123:");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0000: 30 31 32 33 34 35 36 37 38 39 41                | 0123456789A\n");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}

TEST_F(TestLogDump, test_buf_size_12) // NOLINT
{
    const std::array<uint8_t, 12> buf = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B' };
    LOG_DUMP_INFO(buf.data(), buf.size(), "test log dump %d", 123);
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "test log dump 123:");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0000: 30 31 32 33 34 35 36 37 38 39 41 42             | 0123456789AB\n");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}

TEST_F(TestLogDump, test_buf_size_13) // NOLINT
{
    const std::array<uint8_t, 13> buf = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C' };
    LOG_DUMP_INFO(buf.data(), buf.size(), "test log dump %d", 123);
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "test log dump 123:");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0000: 30 31 32 33 34 35 36 37 38 39 41 42 43          | 0123456789ABC\n");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}

TEST_F(TestLogDump, test_buf_size_14) // NOLINT
{
    const std::array<uint8_t, 14> buf = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D' };
    LOG_DUMP_INFO(buf.data(), buf.size(), "test log dump %d", 123);
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "test log dump 123:");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0000: 30 31 32 33 34 35 36 37 38 39 41 42 43 44       | 0123456789ABCD\n");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}

TEST_F(TestLogDump, test_buf_size_15) // NOLINT
{
    const std::array<uint8_t, 15> buf = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E' };
    LOG_DUMP_INFO(buf.data(), buf.size(), "test log dump %d", 123);
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "test log dump 123:");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0000: 30 31 32 33 34 35 36 37 38 39 41 42 43 44 45    | 0123456789ABCDE\n");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}

TEST_F(TestLogDump, test_buf_size_16) // NOLINT
{
    const std::array<uint8_t, 16> buf = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
    };
    LOG_DUMP_INFO(buf.data(), buf.size(), "test log dump %d", 123);
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "test log dump 123:");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0000: 30 31 32 33 34 35 36 37 38 39 41 42 43 44 45 46 | 0123456789ABCDEF\n");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}

TEST_F(TestLogDump, test_buf_size_31) // NOLINT
{
    const std::array<uint8_t, 31> buf = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A',
                                          'B', 'C', 'D', 'E', 'F', 'a', 'b', 'c', 'd', 'e', 'f',
                                          'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o' };
    LOG_DUMP_INFO(buf.data(), buf.size(), "test log dump %d", 123);
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "test log dump 123:");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0000: 30 31 32 33 34 35 36 37 38 39 41 42 43 44 45 46 | 0123456789ABCDEF\n");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0010: 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F    | abcdefghijklmno\n");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}

TEST_F(TestLogDump, test_non_ascii_symbols) // NOLINT
{
    std::array<uint8_t, UINT8_MAX + 1> buf = {};
    for (uint32_t i = 0; i < buf.size(); ++i)
    {
        buf[i] = (uint8_t)i;
    }
    LOG_DUMP_INFO(buf.data(), buf.size(), "test log dump %d", 123);
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "test log dump 123:");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0000: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F | ................\n");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0010: 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F | ................\n");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0020: 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F |  !\"#$%&'()*+,-./\n");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0030: 30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F | 0123456789:;<=>?\n");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0040: 40 41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F | @ABCDEFGHIJKLMNO\n");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0050: 50 51 52 53 54 55 56 57 58 59 5A 5B 5C 5D 5E 5F | PQRSTUVWXYZ[\\]^_\n");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0060: 60 61 62 63 64 65 66 67 68 69 6A 6B 6C 6D 6E 6F | `abcdefghijklmno\n");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0070: 70 71 72 73 74 75 76 77 78 79 7A 7B 7C 7D 7E 7F | pqrstuvwxyz{|}~.\n");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0080: 80 81 82 83 84 85 86 87 88 89 8A 8B 8C 8D 8E 8F | ................\n");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "0090: 90 91 92 93 94 95 96 97 98 99 9A 9B 9C 9D 9E 9F | ................\n");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "00A0: A0 A1 A2 A3 A4 A5 A6 A7 A8 A9 AA AB AC AD AE AF | ................\n");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "00B0: B0 B1 B2 B3 B4 B5 B6 B7 B8 B9 BA BB BC BD BE BF | ................\n");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "00C0: C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF | ................\n");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "00D0: D0 D1 D2 D3 D4 D5 D6 D7 D8 D9 DA DB DC DD DE DF | ................\n");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "00E0: E0 E1 E2 E3 E4 E5 E6 E7 E8 E9 EA EB EC ED EE EF | ................\n");
    TEST_CHECK_LOG_RECORD(ESP_LOG_INFO, "00F0: F0 F1 F2 F3 F4 F5 F6 F7 F8 F9 FA FB FC FD FE FF | ................\n");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}

TEST_F(TestLogDump, test_log_dump_err) // NOLINT
{
    const std::array<uint8_t, 1> buf = { '0' };
    LOG_DUMP_ERR(buf.data(), buf.size(), "test log dump %d", 123);
    TEST_CHECK_LOG_RECORD(ESP_LOG_ERROR, "test log dump 123:");
    TEST_CHECK_LOG_RECORD(ESP_LOG_ERROR, "0000: 30                                              | 0\n");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}

TEST_F(TestLogDump, test_log_dump_warn) // NOLINT
{
    const std::array<uint8_t, 1> buf = { '0' };
    LOG_DUMP_WARN(buf.data(), buf.size(), "test log dump %d", 123);
    TEST_CHECK_LOG_RECORD(ESP_LOG_WARN, "test log dump 123:");
    TEST_CHECK_LOG_RECORD(ESP_LOG_WARN, "0000: 30                                              | 0\n");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}

TEST_F(TestLogDump, test_log_dump_dbg) // NOLINT
{
    const std::array<uint8_t, 1> buf = { '0' };
    LOG_DUMP_DBG(buf.data(), buf.size(), "test log dump %d", 123);
    TEST_CHECK_LOG_RECORD(ESP_LOG_DEBUG, "test log dump 123:");
    TEST_CHECK_LOG_RECORD(ESP_LOG_DEBUG, "0000: 30                                              | 0\n");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}

TEST_F(TestLogDump, test_log_dump_verbose) // NOLINT
{
    const std::array<uint8_t, 1> buf = { '0' };
    LOG_DUMP_VERBOSE(buf.data(), buf.size(), "test log dump %d", 123);
    TEST_CHECK_LOG_RECORD(ESP_LOG_VERBOSE, "test log dump 123:");
    TEST_CHECK_LOG_RECORD(ESP_LOG_VERBOSE, "0000: 30                                              | 0\n");
    ASSERT_TRUE(esp_log_wrapper_is_empty());
}
