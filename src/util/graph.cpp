#include <graph/graph.hpp>
#include <iostream>
#include <algorithm>
#include <net/net.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <exception>
#include <cstring>
#include <thread>
#include <comm/comm.hpp>

#define MAX_PACKET_BUFFER_SIZE 2048

namespace tcpip{

	char recv_buffer[MAX_PACKET_BUFFER_SIZE];

	void __network_start_pkt_receiver_thread(graph_t* topograph){

		fd_set active_sock_fd_set, backup_sock_fd_set;

		int max_value_fd = 0;
		unsigned int bytes_recvd = 0;
		FD_ZERO(&active_sock_fd_set);
		FD_ZERO(&backup_sock_fd_set);

		/* used to store senders IP address */
		struct sockaddr_in sender_addr;
		socklen_t sender_addr_size = sizeof(sender_addr);

		for(const auto& node_element: topograph->node_list){
			if(!node_element->udp_sock_fd){
				continue;
			}

			/* Find the max_value_fd */
			if(node_element->udp_sock_fd > max_value_fd){
				max_value_fd = node_element->udp_sock_fd;
			}

			/* Copy node_element->udp_sock_fd to the backup_fd_set  */
			FD_SET(node_element->udp_sock_fd, &backup_sock_fd_set);
		}

		while(1){
			/* copy backup_fd_set to active_fd_set */
			memcpy(&active_sock_fd_set, &backup_sock_fd_set, sizeof(fd_set));

			/*  Monitor active_sock_fd_set 
				select(FD_SETSIZE, &readfds, &writefds, NULL, &timeout);
			*/
			select(max_value_fd + 1, &active_sock_fd_set, NULL, NULL, NULL);

			/* 	select syscall while edit active_sock_fd_set and will keep
				fds which are ready for communication (READING only)
			*/
			for(const auto& node_element: topograph->node_list){

				/* check if the node_element->udp_sock_fd is present in active_sock_fd_set*/
				if(FD_ISSET(node_element->udp_sock_fd, &backup_sock_fd_set)){
					/* This node_element->udp_sock_fd is ready for READING */

					memset(recv_buffer, 0, MAX_PACKET_BUFFER_SIZE);

					/* ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                        struct sockaddr *src_addr, socklen_t *addrlen); */
					bytes_recvd = recvfrom(
						node_element->udp_sock_fd, 
						static_cast<char*>(recv_buffer),
						MAX_PACKET_BUFFER_SIZE,
						0,
						(struct sockaddr*) &sender_addr,
						&sender_addr_size
					);

					process_received_pkt(node_element.get(), recv_buffer, bytes_recvd);
				}
			}


		}
	}

	void network_start_pkt_receiver_thread(graph_t* topograph){
		/* 	We don't want to block our CLI, so we listen in fds in ONE separate thread from the main thread
			In that separate thread we will use select system call for monitoring on all the fds
		*/

		std::thread thread1(__network_start_pkt_receiver_thread, topograph);

		thread1.detach();
	}

	bool node_set_loopback_address(node_t* node, const std::array<unsigned char, 4>& ip_addr){

        node->node_nw_prop.loobback_addr.ip_addr = ip_addr;
        node->node_nw_prop.is_loopback_addr_config = true;
        return true;

    }

	unsigned int node_t::starting_udp_port_number = 4000;

	node_t::node_t(std::string nodename)
		:node_name(nodename), udp_port_number(++starting_udp_port_number)
	{
				
		udp_sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		struct sockaddr_in node_addr;
		node_addr.sin_family = AF_INET;
		node_addr.sin_port = udp_port_number;
		node_addr.sin_addr.s_addr = INADDR_ANY;

		if(bind(udp_sock_fd, (struct sockaddr*)&node_addr, sizeof(node_addr)) == -1){
			std::string error_string = "Error: socket bind failed for Node " + node_name;
			throw std::runtime_error(error_string);
		}
	}


    bool intf_set_ip_address(
        node_t* node,
        const std::string& node_local_if_name,
        const std::array<unsigned char, 4>& ip_addr, 
        unsigned char mask
    ){
        auto it = std::find_if(node->intf.begin(), node->intf.end(), 
            [&node_local_if_name](const std::shared_ptr<interface_t>& local_interface){
                if(local_interface->if_name == node_local_if_name){
                    return true;
                }
				return false;
        });

        if(it == node->intf.end()){
            return false;
        }

        (*it)->intf_nw_props.ip_addr.ip_addr = ip_addr;
        (*it)->intf_nw_props.is_ip_addr_config = true;
        (*it)->intf_nw_props.mask = mask;

        return true;

    }


	void graph_t::add_node(std::shared_ptr<node_t> node){
		node_list.push_front(node);
	}

	std::ostream& operator<<(std::ostream& co, const node_nw_prop_t& node_prop){

		// std::ios_base::fmtflags originalFlags = std::cout.flags();

		if(node_prop.is_loopback_addr_config == false){
			co << " NO LOOPBACK ADDRESS ";
		} else {
			
			for(auto i = 0;  i < node_prop.loobback_addr.ip_addr.size() ; ++i) {
				co << static_cast<int>(node_prop.loobback_addr.ip_addr[i]);
				if( i != node_prop.loobback_addr.ip_addr.size() - 1){
					co << ".";
				}
			}
			
		}

		// std::cout.flags(originalFlags);

		return co;
	}

	std::ostream& operator<<(std::ostream& co, const intf_nw_props_t& if_prop){
		
		// Save the original state of std::cout
		// std::ios_base::fmtflags originalFlags = std::cout.flags();

		if(if_prop.is_ip_addr_config == false){
			co << " NO IP ADDRESS ";
		} else {
			
			for(auto i = 0;  i < if_prop.ip_addr.ip_addr.size() ; ++i) {
				co << static_cast<int>(if_prop.ip_addr.ip_addr[i]);
				if( i != if_prop.ip_addr.ip_addr.size() - 1){
					co << ".";
				}
			}

			co << "\tMAC ADDR: ";

			for(auto i = 0;  i < if_prop.mac_addr.mac_addr.size() ; ++i) {
				co << static_cast<int>(if_prop.mac_addr.mac_addr[i]);
				if( i != if_prop.mac_addr.mac_addr.size() - 1){
					co << ".";
				}
			}
		}

		// Restore the original state of std::cout
        // std::cout.flags(originalFlags);

		return co;
	}

	interface_t* get_nbr_interface(interface_t* given_interface){
		link_t* w_link_connected_given_interface = given_interface->w_link.get();

		auto intf1_shared_ptr = w_link_connected_given_interface->intf1.lock();
		auto intf2_shared_ptr = w_link_connected_given_interface->intf2.lock();

		std::shared_ptr<node_t> nbr_if_to_return_shared_ptr{};

		if (intf1_shared_ptr != nullptr && intf2_shared_ptr != nullptr){

			interface_t* intf1_ptr = intf1_shared_ptr.get();
			interface_t* intf2_ptr = intf2_shared_ptr.get();

			if(intf1_ptr == given_interface){
				return intf2_ptr;
			}else {
				return intf1_ptr;
			}
		}

		return nullptr;
	}

	node_t* get_nbr_node(interface_t* given_interface){
		/*
		link_t* w_link_connected_given_interface = given_interface->w_link.get();

		auto intf1_shared_ptr = w_link_connected_given_interface->intf1.lock();
		auto intf2_shared_ptr = w_link_connected_given_interface->intf2.lock();

		std::shared_ptr<node_t> att_node_to_return_shared_ptr{};

		if (intf1_shared_ptr != nullptr && intf2_shared_ptr != nullptr){

			interface_t* intf1_ptr = intf1_shared_ptr.get();
			interface_t* intf2_ptr = intf2_shared_ptr.get();

			if(intf1_ptr == given_interface){
				att_node_to_return_shared_ptr = intf2_ptr->att_node.lock();
			}else {
				att_node_to_return_shared_ptr = intf1_ptr->att_node.lock();
			}
		}
		return att_node_to_return_shared_ptr.get();
		*/

		interface_t* nbr_node_if = get_nbr_interface(given_interface);
		if(nbr_node_if){

			auto att_node_to_return_shared_ptr = nbr_node_if->att_node.lock();
			if(att_node_to_return_shared_ptr){
				return att_node_to_return_shared_ptr.get();
			}
			
		}

		return nullptr;
	}

	void graph_t::dump_graph(){
		std::cout << "Topology Name = " << topology_name << "\n";

		for(const auto& node_element: node_list){
			std::cout << "Node Name = " << node_element->node_name << " : " << "\n";
			std::cout << "\tnode flags: 2" << "\tlo addr: " << node_element->node_nw_prop << "\n";
			
			for(const auto& interface_element_shared_ptr: node_element->intf){
				std::cout << "Interface Name = " << interface_element_shared_ptr->if_name << " : " << "\n";
				
				auto att_node_shared_ptr = interface_element_shared_ptr->att_node.lock();
				std::string att_node_name{"NIL"};
				if (att_node_shared_ptr){
					// It did not returned nullptr
					att_node_name = (*att_node_shared_ptr).node_name;
				}

				node_t* nbr_node_shared_ptr = get_nbr_node(interface_element_shared_ptr.get());
				std::string nbr_node_name{"NIL"};
				if(nbr_node_shared_ptr){
					nbr_node_name = nbr_node_shared_ptr->node_name;
				}

				std::cout << "\tOwning Node: " << att_node_name << ", Nbr Node: " <<nbr_node_name << ", cost = " << interface_element_shared_ptr->w_link->cost << "\n";
				std::cout << "\tIP Addr = " << interface_element_shared_ptr->intf_nw_props << "\n";
			}
		} 
	}


	link_t::~link_t(){/* std::cout << "Link Destroyed\n"; */}

	void insert_link_between_two_nodes(const std::shared_ptr<node_t>& node1, 
								  const std::shared_ptr<node_t>& node2, 
								  std::string from_if_name,
								  std::string to_if_name,
								  unsigned int cost){

		std::shared_ptr<interface_t> if1 = std::make_shared<interface_t>();
		if1->if_name = from_if_name;
		if1->att_node = node1;
		if1->w_link = nullptr;

		std::shared_ptr<interface_t> if2 = std::make_shared<interface_t>();
		if2->if_name = to_if_name;
		if2->att_node = node2;
		if2->w_link = nullptr;

		std::shared_ptr<link_t> new_link = std::make_shared<link_t>();
		new_link->intf1 = if1;
		new_link->intf2 = if2;
		new_link->cost = cost;

		auto new_link__intf1__w_link = new_link->intf1.lock();
		if(new_link__intf1__w_link){
			new_link__intf1__w_link->w_link = new_link;
		}

		auto new_link__intf2__w_link = new_link->intf2.lock();
		if(new_link__intf2__w_link){
			new_link__intf2__w_link->w_link = new_link;
		}

		auto new_link_intf1 = new_link->intf1.lock();
		if(new_link_intf1){
			node1->intf.push_back(new_link_intf1);
		}else{
			std::cout << "DANGER1\n";
		}

		auto new_link_intf2 = new_link->intf2.lock();
		if(new_link_intf2){
			node2->intf.push_back(new_link_intf2);
		}else{
			std::cout << "DANGER2\n";	
		}

	}

	// INTERFACE OWNS LINK
	// LINK DOES NOT OWNS INTERFACE

	// NODE OWNS INTERFACE
	// INTERFACE DOES NOT OWNS NODE

};
