#include <string>
#include <iostream>
#include <filesystem>
#include <list>
#include <chrono>
#include <fstream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <fstream>
#include <future>

void create_file(std::string filename, std::string content)
{
    std::ofstream file(filename);
    if (file.is_open())
    {
        file << content;
        file.close();
    }
}

void print_file(std::string content, std::string label)
{
    std::cout << "~~~~~~~~~~~~~~~~~~~ " << label << " ~~~~~~~~~~~~~~~~~~~\n"
              << content
              << "~~~~~~~~~~~~~~~~~~~ " << label << " ~~~~~~~~~~~~~~~~~~~"
              << std::endl;
}
