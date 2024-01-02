#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <string>
#include <forward_list>
#include <memory>
#include <vector>
#include <net/net.hpp>

#define MAX_NO_OF_INTERFACE 20

namespace tcpip{

	struct node_t;
	struct link_t;

	struct interface_t{
		std::string if_name{};

		/**
		 * Owning node: Node this particular interface is connected to*/
		std::weak_ptr<node_t> att_node;
		/** It does not take ownership */

		/**
		 * Every interface is a part of a wire (link) */
		std::shared_ptr<tcpip::link_t> w_link;

		/**
		 * att_node and link are both pointer eventhough it does not depend on dynamic values. WHY?
		 * 
		 * att_node and link are both of incomplete type that's why they are defined as pointers
		 * 
		 * */

		/* Interface network properties */
		intf_nw_props_t intf_nw_props;

	};

	struct link_t{

		std::weak_ptr<interface_t> intf1;
		std::weak_ptr<interface_t> intf2;
		unsigned int cost;

		~link_t();

	};

	struct node_t{
		std::string node_name;
		std::vector<std::shared_ptr<interface_t>> intf;

		static unsigned int starting_udp_port_number;

		unsigned int udp_port_number{};
		int udp_sock_fd{};

		node_t(std::string nodename);

		/* node network properties */
		node_nw_prop_t node_nw_prop;

	};

	struct graph_t{
		std::string topology_name;

		/**Singly Linked list*/
		std::forward_list<std::shared_ptr<node_t>> node_list;

		graph_t(std::string tp_name):
			topology_name{tp_name}
		{}

		void add_node(std::shared_ptr<node_t> node);

		void dump_graph();

	};

	/* When shared_ptr is taken as reference then reference count is not incremented */
	void insert_link_between_two_nodes(const std::shared_ptr<node_t>& node1, 
								  const std::shared_ptr<node_t>& node2, 
								  std::string from_if_name,
								  std::string to_if_name,
								  unsigned int cost);
	
	/* 	Here we are taking as a raw pointer instead of std::shared_ptr because
		inside the function we need not do any assignment of this "node" variable to other
		instead we just need to access its properties, which can be easily done using a 
		raw pointer
	 */
	bool node_set_loopback_address(node_t* node, const std::array<unsigned char, 4>& ip_addr);
	bool intf_set_ip_address(
        node_t* node,
        const std::string& node_local_if_name,
        const std::array<unsigned char, 4>& ip_addr, 
        unsigned char mask
    );

	void network_start_pkt_receiver_thread(graph_t* topograph);

	node_t* get_nbr_node(interface_t* given_interface);
	interface_t* get_nbr_interface(interface_t* given_interface);
};



#endif