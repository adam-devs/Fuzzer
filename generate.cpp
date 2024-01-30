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
std::string generate_new_input(int seed, int action, std::string sut_path, bool verbose = false)
{
    if (verbose) {
      std::cout << "Generating input (action: " + std::to_string(action) + ", seed: " + std::to_string(seed) << "): " << std::endl;
    }

    switch (action)
    {
    case 1:
        return generate_sat(10, 10, 10, seed);
    case 2:
        return generate_unsat_pigeonhole(6, 5);
    case 3:
        return generate_cnf(10000, 10000, 10000, seed);
    case 4:
        return generate_sat(1000, 500, 500, seed);
    case 5:
        return generate_unsat_combination(12);
    default:
        return generate_sat(5, 10, 10, seed);
    }

}
