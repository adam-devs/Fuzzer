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
#include <deque>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "generate.hpp"
#include "process_output.hpp"

#ifndef FUZZER_HPP
#define FUZZER_HPP

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

void export_inputs_info(Input *saved) {
  std::string out = "";

  for (int i = 0; i < 20; i++) {
    if (saved[i].type != placeholder)
      out = out + std::to_string(i) + " Type: " + std::to_string(saved[i].type) + "\n";
  }

  std::cout << out << std::endl;
}

std::size_t get_hash(std::string output) {
  std::hash<std::string> hasher; 
  return hasher(output); 
}

#endif
