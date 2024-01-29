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
std::string generate_new_input(std::string seed)
{
    std::cout << "Generating input (with seed " + seed << "): " << std::endl;

    return generate_sat(10, 20, 20);
    // return "p cnf " + random_number(2) + " " + random_number(2) + "\n";
    //    return "p cnf 1 1 \n1 0\n";
}
