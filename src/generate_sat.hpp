#include <random>
#include <iostream>
#include <vector> 
#include <string>
#include <functional>

std::string generate_cnf(int num_vars = 10, int num_clauses = 20, int max_clauses = 20, unsigned int seed = 123);

std::string generate_sat(int num_vars = 10, int num_clauses = 20, int max_clauses = 20, unsigned int seed = 123);

std::string generate_unsat_combination(int num_vars = 3);

std::string generate_unsat_pigeonhole(int pigeons = 4, int holes = 3); 
