#include <iostream>
#include <graph/graph.hpp>
#include <fstream>
#include <cli/cli.h>
#include <cli/clifilesession.h>

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
	topo->dump_graph();

	/*
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

	rootMenu->Insert(std::move(showMenu));
	
	cli::Cli cli( std::move(rootMenu) );
    // global exit action
    cli.ExitAction([](auto& out){ out << "Goodbye and thanks for all the fish.\n"; });

    cli::CliFileSession input(cli);

    input.Start();
	*/

	return 0;
}