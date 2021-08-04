/**
 * @file test_os_mkgmtime.cpp
 * @author TheSomeMan
 * @date 2021-08-02
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "os_mkgmtime.h"
#include "gtest/gtest.h"
#include <string>

using namespace std;

/*** Google-test class implementation
 * *********************************************************************************/

class TestOsMkgmtime : public ::testing::Test
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
    TestOsMkgmtime();

    ~TestOsMkgmtime() override;
};

TestOsMkgmtime::TestOsMkgmtime()
    : Test()
{
}

TestOsMkgmtime::~TestOsMkgmtime() = default;

/*** Unit-Tests
 * *******************************************************************************************************/

static struct tm
init_tm(const int year, const int month, const int mday, const int hour, const int min, const int sec)
{
    struct tm tm_time
    {
    };
    tm_time.tm_year = year - 1900;
    tm_time.tm_mon  = month - 1;
    tm_time.tm_mday = mday;
    tm_time.tm_hour = hour;
    tm_time.tm_min  = min;
    tm_time.tm_sec  = sec;
    return tm_time;
}

TEST_F(TestOsMkgmtime, test_os_gmtime_1970_01_01_00_00_00) // NOLINT
{
    struct tm tm_time = init_tm(1970, 1, 1, 0, 0, 0);
    ASSERT_EQ(0, os_mkgmtime(&tm_time));
    ASSERT_EQ(70, tm_time.tm_year);
    ASSERT_EQ(1 - 1, tm_time.tm_mon);
    ASSERT_EQ(1, tm_time.tm_mday);
    ASSERT_EQ(0, tm_time.tm_hour);
    ASSERT_EQ(0, tm_time.tm_min);
    ASSERT_EQ(0, tm_time.tm_sec);
    ASSERT_EQ(1 - 1, tm_time.tm_yday);
    ASSERT_EQ(4, tm_time.tm_wday);
    ASSERT_EQ(0, tm_time.tm_isdst);
}

TEST_F(TestOsMkgmtime, test_os_gmtime_1970_01_02_12_13_14) // NOLINT
{
    struct tm tm_time = init_tm(1970, 1, 2, 12, 13, 14);
    ASSERT_EQ(130394, os_mkgmtime(&tm_time));
    ASSERT_EQ(70, tm_time.tm_year);
    ASSERT_EQ(1 - 1, tm_time.tm_mon);
    ASSERT_EQ(2, tm_time.tm_mday);
    ASSERT_EQ(12, tm_time.tm_hour);
    ASSERT_EQ(13, tm_time.tm_min);
    ASSERT_EQ(14, tm_time.tm_sec);
    ASSERT_EQ(2 - 1, tm_time.tm_yday);
    ASSERT_EQ(5, tm_time.tm_wday);
    ASSERT_EQ(0, tm_time.tm_isdst);
}

TEST_F(TestOsMkgmtime, test_os_gmtime_1999_12_31_23_59_59)
{
    struct tm tm_time = init_tm(1999, 12, 31, 23, 59, 59);
    ASSERT_EQ(946684799, os_mkgmtime(&tm_time));
    ASSERT_EQ(99, tm_time.tm_year);
    ASSERT_EQ(12 - 1, tm_time.tm_mon);
    ASSERT_EQ(31, tm_time.tm_mday);
    ASSERT_EQ(23, tm_time.tm_hour);
    ASSERT_EQ(59, tm_time.tm_min);
    ASSERT_EQ(59, tm_time.tm_sec);
    ASSERT_EQ(365 - 1, tm_time.tm_yday);
    ASSERT_EQ(5, tm_time.tm_wday);
    ASSERT_EQ(0, tm_time.tm_isdst);
}

TEST_F(TestOsMkgmtime, test_os_gmtime_2000_02_29_23_59_59)
{
    struct tm tm_time = init_tm(2000, 2, 29, 23, 59, 59);
    ASSERT_EQ(951868799, os_mkgmtime(&tm_time));
    ASSERT_EQ(100, tm_time.tm_year);
    ASSERT_EQ(2 - 1, tm_time.tm_mon);
    ASSERT_EQ(29, tm_time.tm_mday);
    ASSERT_EQ(23, tm_time.tm_hour);
    ASSERT_EQ(59, tm_time.tm_min);
    ASSERT_EQ(59, tm_time.tm_sec);
    ASSERT_EQ(60 - 1, tm_time.tm_yday);
    ASSERT_EQ(2, tm_time.tm_wday);
    ASSERT_EQ(0, tm_time.tm_isdst);
}

TEST_F(TestOsMkgmtime, test_os_gmtime_2000_03_01_23_59_59)
{
    struct tm tm_time = init_tm(2000, 3, 1, 23, 59, 59);
    ASSERT_EQ(951955199, os_mkgmtime(&tm_time));
    ASSERT_EQ(100, tm_time.tm_year);
    ASSERT_EQ(3 - 1, tm_time.tm_mon);
    ASSERT_EQ(1, tm_time.tm_mday);
    ASSERT_EQ(23, tm_time.tm_hour);
    ASSERT_EQ(59, tm_time.tm_min);
    ASSERT_EQ(59, tm_time.tm_sec);
    ASSERT_EQ(61 - 1, tm_time.tm_yday);
    ASSERT_EQ(3, tm_time.tm_wday);
    ASSERT_EQ(0, tm_time.tm_isdst);
}

TEST_F(TestOsMkgmtime, test_os_gmtime_2000_12_31_23_59_59)
{
    struct tm tm_time = init_tm(2000, 12, 31, 23, 59, 59);
    ASSERT_EQ(978307199, os_mkgmtime(&tm_time));
    ASSERT_EQ(100, tm_time.tm_year);
    ASSERT_EQ(12 - 1, tm_time.tm_mon);
    ASSERT_EQ(31, tm_time.tm_mday);
    ASSERT_EQ(23, tm_time.tm_hour);
    ASSERT_EQ(59, tm_time.tm_min);
    ASSERT_EQ(59, tm_time.tm_sec);
    ASSERT_EQ(366 - 1, tm_time.tm_yday);
    ASSERT_EQ(0, tm_time.tm_wday);
    ASSERT_EQ(0, tm_time.tm_isdst);
}

TEST_F(TestOsMkgmtime, test_os_gmtime_2038_01_19_03_14_07)
{
    struct tm tm_time = init_tm(2038, 1, 19, 3, 14, 7);
    ASSERT_EQ(0x7fffffff, os_mkgmtime(&tm_time));
    ASSERT_EQ(138, tm_time.tm_year);
    ASSERT_EQ(1 - 1, tm_time.tm_mon);
    ASSERT_EQ(19, tm_time.tm_mday);
    ASSERT_EQ(3, tm_time.tm_hour);
    ASSERT_EQ(14, tm_time.tm_min);
    ASSERT_EQ(7, tm_time.tm_sec);
    ASSERT_EQ(19 - 1, tm_time.tm_yday);
    ASSERT_EQ(2, tm_time.tm_wday);
    ASSERT_EQ(0, tm_time.tm_isdst);
}

TEST_F(TestOsMkgmtime, test_os_gmtime_2038_01_19_03_14_08)
{
    struct tm tm_time = init_tm(2038, 1, 19, 3, 14, 8);
    ASSERT_EQ(-1, os_mkgmtime(&tm_time));
}
