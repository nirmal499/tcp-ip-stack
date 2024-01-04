#include <comm/comm.hpp>
#include <types/defined_types.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>  /* hostent */
#include <unistd.h> /* close */

#define MAX_PACKET_BUFFER_SIZE 2048

namespace tcpip{
	char send_buffer[MAX_PACKET_BUFFER_SIZE];

    interface_t* get_node_if_by_name(node_t *node, const std::string& given_if_name){
        for(auto const& element: node->intf){
            if(element->if_name == given_if_name){
                return element.get();
            }
        }

        return nullptr;
    }

    int process_extracted_pkt(node_t* receiving_node, interface_t* recv_if, char* pkt, unsigned int pkt_size){
        /*  Entry point into the data link layer from physical
            Ingress journey of the packet starts from here in the TCP/IP stack
        */

        std::cout << "msg recvd = " << pkt << " on node " << receiving_node->node_name << ", IIF = " << recv_if->if_name << "\n";

        return 0;
    }

    /* segregates the auxiliary data from pkt_with_auxiliary_data */
    int process_received_pkt(node_t* receiving_node, char* pkt_with_auxiliary_data, unsigned int pkt_size){
        
        Defined defined_data;
        std::size_t decoded_size = defined_data.decode_from_buffer(pkt_with_auxiliary_data);
        if(decoded_size == 0){
            std::cout << "Error decoding data\n";
            return -1;
        }

        interface_t *recv_if = get_node_if_by_name(receiving_node, defined_data.name);

        if(!recv_if){
            std::cout << "Error: Pkt recvd on unknown interface " 
                << defined_data.name << " on node " << receiving_node->node_name << "\n";

            return -1;
        }
        
        process_extracted_pkt(
            receiving_node, 
            recv_if, 
            pkt_with_auxiliary_data + decoded_size, /* Pass the part after auxiliary_data */
            pkt_size - decoded_size /* Decrease the pkt_size by the extracted auxiliary_data size */
        );

        return 0;
    }

    int __send_pkt_out(int sock_fd, char* pkt_with_auxiliary_data, unsigned int pkt_size, unsigned int dest_udp_port_number){
        int bytes_sent;
        struct sockaddr_in dest_addr;

        struct hostent* host = (struct hostent*) gethostbyname("127.0.0.1");
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = dest_udp_port_number;
        dest_addr.sin_addr = *((struct in_addr*)host->h_addr);

        /* ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
                      const struct sockaddr *dest_addr, socklen_t addrlen); */

        bytes_sent = sendto(sock_fd, pkt_with_auxiliary_data, pkt_size, 0,(struct sockaddr *)&dest_addr, sizeof(dest_addr));

        return bytes_sent;
    }

    int send_pkt_out(interface_t* sending_node_if, char* pkt, unsigned int pkt_size){
        node_t* sending_node = sending_node_if->att_node.lock().get();
        node_t* nbr_node = get_nbr_node(sending_node_if);

        if(!nbr_node){
            return -1;
        }

        unsigned int dst_udp_port_number = nbr_node->udp_port_number;

        int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(sock < 0){
            std::string error_string = "Error: socket creation failed";
			std::cout << error_string << "\n";

            return -1;
        }

        interface_t* nbr_interface = get_nbr_interface(sending_node_if);

        memset(send_buffer, 0, MAX_PACKET_BUFFER_SIZE);

        if(pkt_size + nbr_interface->if_name.size() > MAX_PACKET_BUFFER_SIZE){
            std::cout << "MAX_PACKET_BUFFER_SIZE is not enough\n";
            return -1;
        }

        Defined defined_data;
        defined_data.initValue(nbr_interface->if_name);
        std::size_t encoded_size = defined_data.encode_into_buffer(send_buffer, MAX_PACKET_BUFFER_SIZE);
        if(encoded_size == 0){
            std::cout << "Error encoding data\n";
            return -1;
        }

        memcpy(send_buffer + encoded_size, pkt, pkt_size);

        /*
            char* pkt_with_auxiliary_data = send_buffer;
            auto if_name_size = nbr_interface->if_name.size();
            strncpy(pkt_with_auxiliary_data, nbr_interface->if_name.c_str(), if_name_size); // char *strncpy(char *dest, const char *src, size_t n);
            pkt_with_auxiliary_data[if_name_size] = '\0';

            memcpy(pkt_with_auxiliary_data + if_name_size, pkt, pkt_size); // void *memcpy(void *dest, const void *src, size_t n);
        */

        int bytes_sent = __send_pkt_out(sock, send_buffer, encoded_size + pkt_size, dst_udp_port_number);
        if(bytes_sent < 0){
            return -1;
        }

        close(sock);

        return bytes_sent;

    }
}