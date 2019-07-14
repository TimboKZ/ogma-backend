#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "Config.h"
#include "Server.h"

using namespace std;
using namespace Ogma;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int ac, char *av[]) {

    // Parse command line options
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("port", po::value<int>(), "port on which the web server will listen")
            ("frontend", po::value<string>(), "path to frontend build");
    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);
    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    // Prepare config
    Config config;
    if (vm.count("port")) {
        config.server_port = vm["port"].as<int>();
    } else {
        std::cout << "Server port was not specified. Using default." << std::endl;
        config.server_port = 10548;
    }
    if (vm.count("frontend")) {
        config.frontend_build_path = fs::canonical(vm["frontend"].as<string>());
    } else {
        std::cout << "Frontend build path was not specified. Aborting." << std::endl;
        return 2;
    }

    // Log effective config
    std::cout << "Effective config:" << std::endl;
    std::cout << "  - server port: " << config.server_port << std::endl;
    std::cout << "  - frontend build path: " << config.frontend_build_path << std::endl;
    std::cout << std::endl;

    Server server(config);
    server.start();
    return 0;
}

