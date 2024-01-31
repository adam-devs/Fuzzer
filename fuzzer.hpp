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
#include <algorithm>
#include <tuple>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "process_output.hpp"

typedef struct
{ 
  int priority;
  undefined_behaviour_t type;
  std::size_t hash;
} Input;

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

std::size_t get_hash(std::string output) {
  std::hash<std::string> hasher; 
  return hasher(output); 
}

