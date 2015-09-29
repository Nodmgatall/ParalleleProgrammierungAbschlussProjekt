#include "visualizer.hpp"
#include <iostream>
#include <string.h>

const std::string usage_msg = "Usage: ./Visualizer.x <filename>\n"
                              "The Visualizer expects the name of "
                              "a file of simulation data as its only argument.";


int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << usage_msg << std::endl;
        return EXIT_FAILURE;
    }

    if ((strcmp(argv[1], "-h") || strcmp(argv[1], "--help")))
    {
            std::cout << usage_msg << std::endl;
            return EXIT_SUCCESS;
    }

    Visualizer visualizer(argv[1]);

    return EXIT_SUCCESS;
}

