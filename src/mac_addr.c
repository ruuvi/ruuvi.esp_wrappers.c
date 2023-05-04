/**
 * @file mac_addr.c
 * @author TheSomeMan
 * @date 2020-10-23
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

#include "mac_addr.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "str_buf.h"

void
mac_address_bin_init(mac_address_bin_t* p_mac, const uint8_t mac[MAC_ADDRESS_NUM_BYTES])
{
    _Static_assert(
        MAC_ADDRESS_NUM_BYTES == sizeof(p_mac->mac),
        "Size of mac_address_bin_t must be equal to MAC_ADDRESS_NUM_BYTES");
    memcpy(p_mac->mac, mac, sizeof(p_mac->mac));
}

mac_address_str_t
mac_address_to_str(const mac_address_bin_t* p_mac)
{
    mac_address_str_t mac_str = { 0 };
    str_buf_t         str_buf = {
                .buf  = mac_str.str_buf,
                .size = sizeof(mac_str.str_buf),
                .idx  = 0,
    };
    for (size_t i = 0; i < sizeof(p_mac->mac); ++i)
    {
        if (0 != i)
        {
            str_buf_printf(&str_buf, ":");
        }
        str_buf_printf(&str_buf, "%02X", p_mac->mac[i]);
    }
    return mac_str;
}

bool
mac_addr_from_str(const char* const p_mac_addr_str, mac_address_bin_t* p_mac_addr_bin)
{
    if ((NULL == p_mac_addr_str) || (NULL == p_mac_addr_bin))
    {
        return false;
    }

    mac_address_str_t mac_addr_str;
    if (strlen(p_mac_addr_str) >= sizeof(mac_addr_str))
    {
        return false;
    }
    strcpy(mac_addr_str.str_buf, p_mac_addr_str);

    const char* const p_delim    = ":";
    char*             p_save_ptr = NULL;
    char*             p_token    = strtok_r(mac_addr_str.str_buf, p_delim, &p_save_ptr);

    if (NULL == p_token)
    {
        return false;
    }

    for (size_t i = 0; i < MAC_ADDRESS_NUM_BYTES && p_token != NULL; ++i)
    {
        if (strlen(p_token) != 2 || !isxdigit(p_token[0]) || !isxdigit(p_token[1]))
        {
            return false;
        }
        p_mac_addr_bin->mac[i] = (uint8_t)strtol(p_token, NULL, 16);
        p_token                = strtok_r(NULL, p_delim, &p_save_ptr);
    }

    if (p_token != NULL)
    {
        return false;
    }

    return true;
}
