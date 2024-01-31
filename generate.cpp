#include "generate.hpp"
#include "generate_sat.hpp"
#include "coverage.hpp" 

#define NUM "0123456789"

std::string random_number(int length)
{
    std::string characters = NUM;
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, characters.length() - 1);

    std::string randomString;
    for (int i = 0; i < length; ++i)
    {
        randomString += characters[distribution(generator)];
    }

    return randomString;
}

// TODO: Generate input based on seed, coverage, etc.
std::string generate_new_input(int seed, std::tuple<int,int,float> *strat, bool verbose = false)
{
    if (verbose) {
      std::cout << "S: " << std::get<0>(*strat) << " M: " << std::get<1>(*strat) << " I: " << std::get<2>(*strat) << std::endl;    
}
    
    int strategy = std::get<0>(*strat);
    int mutation = std::get<1>(*strat);
    int intensity = std::get<2>(*strat);

    switch (strategy)
    {
    case 0:
        return generate_sat(10, 10, 10, seed);
    case 1:
        return generate_unsat_pigeonhole(6, 5);
    case 2:
        return generate_cnf(10000, 10000, 10000, seed);
    case 3:
        return generate_sat(1000, 500, 500, seed);
    case 4:
        return generate_unsat_combination(12);
    default:
        return generate_sat(5, 10, 10, seed);
    }

}
