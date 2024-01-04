#include <iostream>
#include <graph/graph.hpp>
#include <comm/comm.hpp>
#include <fstream>
#include <cli/cli.h>
#include <cli/clifilesession.h>
#include <thread>
#include <chrono>

extern std::unique_ptr<tcpip::graph_t> build_first_topo();

int main(void)
{
	/*
	// Save the current buffer for cout
    // std::streambuf* coutBuffer = std::cout.rdbuf();

	// Specify the name of the file to redirect cout
    // std::ofstream file("output.txt");

	// Redirect cout to the file
    // std::cout.rdbuf(file.rdbuf());

	std::unique_ptr<tcpip::graph_t> topo = build_first_topo();

    // topo->dump_graph();

	// Restore the original cout buffer
    // std::cout.rdbuf(coutBuffer);
	*/

	std::unique_ptr<tcpip::graph_t> topo = build_first_topo();
	// topo->dump_graph();

	// Sleep for 2 seconds iorder for the other thread to start
    std::chrono::seconds duration(2);
    std::this_thread::sleep_for(duration);

	// tcpip::node_t* snode = topo->get_node_by_node_name("R0_re");
	// if(!snode){
	// 	std::cout << "TESTING FAILED1\n";
	// } else {
	// 	tcpip::interface_t* oif = snode->get_node_if_by_name("eth0/0");

	// 	if(!oif){
	// 		std::cout << "TESTING FAILED2\n";
	// 	} else {
	// 		std::string msg = "Hello, how are you";
	// 		int bytes_sent = tcpip::send_pkt_out(oif, msg.data(), msg.size());

	// 		/*  Make the main thread wait enough so that the detached thread completes all its works
	// 			If main thread exits before the detached thread then nothing will be printed and segmentation fault
	// 			If you make thread make join instead of detach then the main thread will keep waiting for the child thread
	// 			to finish in line main.cpp:32 -> graph.cpp:170. And the child thread will be stuck in select system call
	// 			because it is a blocking syscall as well as recvfrom is also a blocking call
	// 		 */
	// 		std::chrono::seconds duration(10);
    // 		std::this_thread::sleep_for(duration);
	// 	}
	// }

	
	auto rootMenu = std::make_unique< cli::Menu >( "cli" );
	auto showMenu = std::make_unique< cli::Menu >( "show" );

	showMenu->Insert(
        "topology",
        [&topo](std::ostream& out){
			topo->dump_graph();
			out << "PRINTING DONE\n";
		},
        "Print hello world"
	);

	showMenu->Insert(
        "test",
        [&topo](std::ostream& out){
			tcpip::node_t* snode = topo->get_node_by_node_name("R0_re");
			if(!snode){
				out << "TESTING FAILED1\n";
			} else {
				tcpip::interface_t* oif = snode->get_node_if_by_name("eth0/0");

				if(!oif){
					out << "TESTING FAILED2\n";
				} else {
					std::string msg = "Hello, how are you";
					int bytes_sent = tcpip::send_pkt_out(oif, msg.data(), msg.size());

					/*  Make the main thread wait enough so that the detached thread completes all its works
						If main thread exits before the detached thread then nothing will be printed and segmentation fault
						If you make thread make join instead of detach then the main thread will keep waiting for the child thread
						to finish in line main.cpp:32 -> graph.cpp:170. And the child thread will be stuck in select system call
						because it is a blocking syscall as well as recvfrom is also a blocking call
					*/
					std::chrono::seconds duration(5);
					std::this_thread::sleep_for(duration);
				}
			}
		},
        "Testing Command"
	);

	rootMenu->Insert(std::move(showMenu));
	
	cli::Cli cli( std::move(rootMenu) );
    // global exit action
    cli.ExitAction([](auto& out){ out << "Goodbye and thanks for all the fish.\n"; });

    cli::CliFileSession input(cli);

    input.Start();
	

	return 0;
}