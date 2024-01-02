#ifndef __COMM_HPP
#define __COMM_HP


#include <graph/graph.hpp>

namespace tcpip{

    /* Defined types must only have   */
    struct Defined1{
        int x;
        double y;
        std::string 
    };

    void process_received_pkt(node_t* receiving_node, char* pkt_with_auxiliary_data, unsigned int pkt_size);
    /*  From the interface we can get the owning node using property "att_node".
        So, no need to pass node_t* sending_node explicitly
     */
    void send_pkt_out(interface* sending_node_interface, char* pkt_with_auxiliary_data, unsigned int pkt_size);

    int __send_pkt_out(int sock_fd, char* pkt_with_auxiliary_data, unsigned int pkt_size, unsigned int dest_udp_port_number);

}

#endif