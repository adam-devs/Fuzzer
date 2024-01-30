#include <random>
#include <iostream>
#include <vector> 
#include <string>
#include <functional>

std::string generate_sat(int num_vars, int num_clauses, int max_clauses); 

std::string generate_unsat_combination(int num_vars);

std::string generate_unsat_pigionhole(int pigions, int holes); 
