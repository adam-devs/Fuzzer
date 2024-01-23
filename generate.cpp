#include "generate.hpp"

#define ALPHANUM "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
#define NUM "0123456789"

std::string generateRandomString(int length)
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

std::string new_input(int i)
{
    std::cout << "Generating with seed" + std::to_string(i) << std::endl;

    return "p cnf 10 " + generateRandomString(2);
}
