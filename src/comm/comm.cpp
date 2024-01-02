#include <comm/comm.hpp>

#define MAX_PACKET_BUFFER_SIZE 2048

namespace tcpip{
	char send_buffer[MAX_PACKET_BUFFER_SIZE];

    /* segregates the auxiliary data from pkt_with_auxiliary_data */
    void process_received_pkt(node_t* receiving_node, char* pkt_with_auxiliary_data, unsigned int pkt_size){
        char* receiver_if_name = pkt_with_auxiliary_data;
        /* structure your data e.g
            in first two byte store the size then ........
         */
    }

    int __send_pkt_out(int sock_fd, char* pkt_with_auxiliary_data, unsigned int pkt_size, unsigned int dest_udp_port_number){
        int bytes_sent;
        struct sockaddr_in dest_addr;

        struct hostent* host = (struct hostent*) gethostbyname("127.0.0.1");
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = dst_udp_port_number;
        dest_addr.sin_addr = *((struct in_addr*)host->h_addr);

        /* ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
                      const struct sockaddr *dest_addr, socklen_t addrlen); */

        bytes_sent = sendto(sock, pkt_with_auxiliary_data, pkt_size, 0,(struct sockaddr *)&dest_addr, sizeof(dest_addr));

        return bytes_sent;
    }

    int send_pkt_out(interface* sending_node_if, char* pkt, unsigned int pkt_size){
        node_t* sending_node = sending_node_if.lock()->att_node;
        node_t* nbr_node = get_nbr_node(sending_node_if);

        if(!nbr_node){
            return -1;
        }

        unsigned int dst_udp_port_number = nbr_node->udp_port_number;

        int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(sock < 0){
            std::string error_string = "Error: socket bind failed for Node " + node_name;
			std::cout << error_string << "\n";

            return -1;
        }

        interface_t* nbr_interface = get_nbr_interface(sending_node_if);

        memset(send_buffer, 0, MAX_PACKET_BUFFER_SIZE);

        if(pkt_size + nbr_interface->if_name.size() > MAX_PACKET_BUFFER_SIZE){
            return -1;
        }

        char* pkt_with_auxiliary_data = send_buffer;
        auto if_name_size = nbr_interface->if_name.size();
        /* char *strncpy(char *dest, const char *src, size_t n); */
        strncpy(pkt_with_auxiliary_data, nbr_interface->if_name.c_str(), if_name_size);
        pkt_with_auxiliary_data[if_name_size] = '\0';

        /* void *memcpy(void *dest, const void *src, size_t n); */
        memcpy(pkt_with_auxiliary_data + if_name_size, pkt, pkt_size);

        int bytes_sent = __send_pkt_out(sock, pkt_with_auxiliary_data, if_name_size + pkt_size, dst_udp_port_number);
        if(bytes_sent < 0){
            return -1;
        }

        close(sock);

        return bytes_sent;

    }
}