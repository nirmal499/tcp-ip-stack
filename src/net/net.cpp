#include <random>
#include <net/net.hpp>

// Seed the random number generator
std::random_device rd;
std::mt19937 generator(rd());

std::array<unsigned char, 6> generate_MAC_addr() {
    std::array<unsigned char, 6> mac_addr;

    // Generate a random number between 0 and 255
    std::uniform_int_distribution<int> distribution(0, 255);
    int random_number = distribution(generator);

    // Set a locally administered bit and unicast bit
    const unsigned char localAdminBit = 0x02;
    const unsigned char unicastBit = 0x00;

    // Generate the first byte with the locally administered and unicast bits
    unsigned char firstByte = localAdminBit | unicastBit;
    mac_addr[0] = firstByte;

    // Generate the remaining 5 bytes randomly
    for (int i = 1; i < 6; ++i) {
        mac_addr[i] = static_cast<unsigned char>(random_number);
    }

    return mac_addr;
}

namespace tcpip{

    node_nw_prop_t::node_nw_prop_t():
            is_loopback_addr_config(false), loobback_addr{ .ip_addr = 0 }
    {}

    intf_nw_props_t::intf_nw_props_t():
            is_ip_addr_config(false),ip_addr{ .ip_addr = 0 }, mask{0}
    {
        mac_addr.mac_addr = generate_MAC_addr();
    }
}

