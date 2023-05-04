/**
 * @file mac_addr.h
 * @author TheSomeMan
 * @date 2020-10-23
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#ifndef RUUVI_MAC_ADDR_H
#define RUUVI_MAC_ADDR_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAC_ADDRESS_NUM_BYTES (6)

typedef struct mac_address_bin_t
{
    uint8_t mac[MAC_ADDRESS_NUM_BYTES];
} mac_address_bin_t;

typedef struct mac_address_str_t
{
    char str_buf[(MAC_ADDRESS_NUM_BYTES * 2) + (MAC_ADDRESS_NUM_BYTES - 1) + 1]; // format: XX:XX:XX:XX:XX:XX
} mac_address_str_t;

#define MAC_ADDR_STR_BYTE_OFFSET(idx_) ((idx_)*3)

#define MAC_ADDR_INIT(mac1, mac2, mac3, mac4, mac5, mac6) \
    (mac_address_bin_t) \
    { \
        .mac = { \
            mac1, mac2, mac3, mac4, mac5, mac6, \
        }, \
    }

void
mac_address_bin_init(mac_address_bin_t* p_mac, const uint8_t mac[MAC_ADDRESS_NUM_BYTES]);

mac_address_str_t
mac_address_to_str(const mac_address_bin_t* p_mac);

/**
 * @brief Convert a string in format "AA:BB:CC:DD:EE:FF" to binary format
 * @param[IN] p_mac_addr_str - input string with MAC address
 * @param[OUT] p_mac_addr_bin  - pointer to output @def mac_address_bin_t where the converted value will be saved
 * @return true if input string contains valid MAC address
 */
bool
mac_addr_from_str(const char* const p_mac_addr_str, mac_address_bin_t* p_mac_addr_bin);

#ifdef __cplusplus
}
#endif

#endif // RUUVI_MAC_ADDR_H
