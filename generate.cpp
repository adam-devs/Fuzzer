#include "generate.hpp"
#include "generate_sat.hpp"

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
std::string generate_new_input(std::string seed, int action, bool verbose = false)
{
    if (verbose) {
      std::cout << "Generating input (action: " + std::to_string(action) + ", seed: " + seed << "): " << std::endl;
    }

    switch (action)
    {
    case 1:
        return generate_sat(10, 10, 10);
    case 2:
        return generate_sat(10000, 10000, 10000);
    case 3:
        return generate_sat(2, 10, 10);
    case 4:
        return generate_sat(1, 1, 1);
    case 5:
        return generate_unsat_pigionhole(10, 9);
    default:
        return generate_sat(1, 1, 1);
    }

}
