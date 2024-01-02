#ifndef __NET_HPP
#define __NET_HPP

#include <array>

/* Accessors */
#define IF_MAC_ADDR(intf_ptr) ((intf_ptr)->intf_nw_props_t.mac_addr.mac_addr)
#define IF_IP_ADDR(intf_ptr) ((intf_ptr)->intf_nw_props_t.ip_addr.ip_addr)
#define NODE_LOOPBACK_ADDR(node_ptr) ((node_ptr)->node_nw_prop_t.loobback_addr.ip_addr)

namespace tcpip{

    struct ip_add_t {
        /*  IPv4 : 32 bits 
            4 bytes: 32 bits
        */
        std::array<unsigned char, 4> ip_addr;
        /* [0-255].[0-255].[0-255].[0-255] */
        /* 192.168.0.1 */
    };

    struct mac_add_t{
        /* 6 bytes :- 48 bits */
        std::array<unsigned char, 6> mac_addr;/* It's size is 48 bits */
        /* [hex-hex].[hex-hex].[hex-hex].[hex-hex].[hex-hex].[hex-hex] */
        /* 00:1A:2B:3C:4D:5E */
    };

    struct node_nw_prop_t{
        /* L3 properties */

        /* Indicate whether loobback_addr is present or not */
        bool is_loopback_addr_config;

        /* Loopback address of node */
        ip_add_t loobback_addr;

        /*  We do not have mask property because in case of loopback address we know
            it is always 32
        */

        node_nw_prop_t();

    };


    struct intf_nw_props_t {
        /* L2 properties */
        mac_add_t mac_addr; /* MAC are hard burnt in interface NIC */

        /* L3 properties */
        bool is_ip_addr_config;
        ip_add_t ip_addr;
        unsigned char mask;

        intf_nw_props_t();

    };


}


#endif