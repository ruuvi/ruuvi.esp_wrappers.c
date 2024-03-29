/**
 * @file test_mac_addr.cpp
 * @author TheSomeMan
 * @date 2020-10-23
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "mac_addr.h"
#include "gtest/gtest.h"
#include <string>

using namespace std;

/*** Google-test class implementation
 * *********************************************************************************/

class TestMacAddr : public ::testing::Test
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
    TestMacAddr();

    ~TestMacAddr() override;
};

TestMacAddr::TestMacAddr()
    : Test()
{
}

TestMacAddr::~TestMacAddr() = default;

/*** Unit-Tests
 * *******************************************************************************************************/

TEST_F(TestMacAddr, test_mac_address_bin_init) // NOLINT
{
    const std::array<uint8_t, MAC_ADDRESS_NUM_BYTES> mac     = { 0x11U, 0x22U, 0x33U, 0xAAU, 0xBBU, 0xCCU };
    mac_address_bin_t                                mac_bin = { 0 };
    mac_address_bin_init(&mac_bin, mac.data());
    ASSERT_EQ(mac_bin.mac[0], mac[0]);
    ASSERT_EQ(mac_bin.mac[1], mac[1]);
    ASSERT_EQ(mac_bin.mac[2], mac[2]);
    ASSERT_EQ(mac_bin.mac[3], mac[3]);
    ASSERT_EQ(mac_bin.mac[4], mac[4]);
    ASSERT_EQ(mac_bin.mac[5], mac[5]);
}

TEST_F(TestMacAddr, test_mac_address_init_macro) // NOLINT
{
    const mac_address_bin_t mac_bin = MAC_ADDR_INIT(0x11U, 0x22U, 0x33U, 0xAAU, 0xBBU, 0xCCU);
    ASSERT_EQ(mac_bin.mac[0], 0x11);
    ASSERT_EQ(mac_bin.mac[1], 0x22);
    ASSERT_EQ(mac_bin.mac[2], 0x33);
    ASSERT_EQ(mac_bin.mac[3], 0xAA);
    ASSERT_EQ(mac_bin.mac[4], 0xBB);
    ASSERT_EQ(mac_bin.mac[5], 0xCC);
}

TEST_F(TestMacAddr, test_mac_address_to_str) // NOLINT
{
    const mac_address_bin_t mac_bin = MAC_ADDR_INIT(0x11U, 0x22U, 0x33U, 0xAAU, 0xBBU, 0xCCU);
    const mac_address_str_t mac_str = mac_address_to_str(&mac_bin);
    ASSERT_EQ(string("11:22:33:AA:BB:CC"), string(mac_str.str_buf));
}

TEST_F(TestMacAddr, test_mac_addr_from_str) // NOLINT
{
    mac_address_bin_t mac_addr_bin = { 0 };
    ASSERT_TRUE(mac_addr_from_str("11:22:33:AA:BB:CC", &mac_addr_bin));
    ASSERT_EQ(0x11U, mac_addr_bin.mac[0]);
    ASSERT_EQ(0x22U, mac_addr_bin.mac[1]);
    ASSERT_EQ(0x33U, mac_addr_bin.mac[2]);
    ASSERT_EQ(0xAAU, mac_addr_bin.mac[3]);
    ASSERT_EQ(0xBBU, mac_addr_bin.mac[4]);
    ASSERT_EQ(0xCCU, mac_addr_bin.mac[5]);

    ASSERT_FALSE(mac_addr_from_str("11:22:33:AA:BB:CC", nullptr));
    ASSERT_FALSE(mac_addr_from_str(nullptr, &mac_addr_bin));
    ASSERT_FALSE(mac_addr_from_str("11:22:33:AA:BB:C", &mac_addr_bin));
    ASSERT_FALSE(mac_addr_from_str("", &mac_addr_bin));
}
