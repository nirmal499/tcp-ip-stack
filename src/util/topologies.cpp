#include <graph/graph.hpp>

std::unique_ptr<tcpip::graph_t> build_first_topo() {
    std::unique_ptr<tcpip::graph_t> topo = std::make_unique<tcpip::graph_t>("Hello World Generic Graph");

    std::shared_ptr<tcpip::node_t> R0_re = std::make_shared<tcpip::node_t>("R0_re");
	std::shared_ptr<tcpip::node_t> R1_re = std::make_shared<tcpip::node_t>("R1_re");
	std::shared_ptr<tcpip::node_t> R2_re = std::make_shared<tcpip::node_t>("R2_re");

    topo->add_node(R0_re);
	topo->add_node(R1_re);
	topo->add_node(R2_re);

    tcpip::insert_link_between_two_nodes(R0_re, R1_re, "eth0/0", "eth0/1", 1);
    tcpip::insert_link_between_two_nodes(R1_re, R2_re, "eth0/2", "eth0/3", 1);
    tcpip::insert_link_between_two_nodes(R0_re, R2_re, "eth0/4", "eth0/5", 1);

    node_set_loopback_address(R0_re.get(), {122, 1, 1, 0});
    intf_set_ip_address(R0_re.get(), "eth0/4", {40,1,1,1}, 24);
    intf_set_ip_address(R0_re.get(), "eth0/0", {20,1,1,1}, 24);

    node_set_loopback_address(R1_re.get(), {122, 1, 1, 1});
    intf_set_ip_address(R1_re.get(), "eth0/1", {20,1,1,2}, 24);
    intf_set_ip_address(R1_re.get(), "eth0/2", {30,1,1,1}, 24);

    node_set_loopback_address(R2_re.get(), {122, 1, 1, 2});
    intf_set_ip_address(R2_re.get(), "eth0/3", {30,1,1,2}, 24);
    intf_set_ip_address(R2_re.get(), "eth0/5", {40,1,1,2}, 24);

    /*  In this api we are going to iterate over udp socket fds of each
        node of the topology and we will listen on them
    */
    // network_start_pkt_receiver_thread(topo.get());

    return topo;
}