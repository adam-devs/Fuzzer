#include "fuzzer.hpp"

#define FILENAME "new_test.cnf"

void create_file(std::string filename, std::string content)
{
    std::ofstream file(filename);
    if (file.is_open())
    {
        file << content;
        file.close();
    }
}

void run_solver(std::string path_to_SUT, std::string input)
{
    create_file(FILENAME, input);

    std::string run_solver = path_to_SUT + "/runsat.sh " + FILENAME;
    std::system(run_solver.c_str());
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        std::cout << "Usage: " << argv[0] << " /path/to/SUT /path/to/inputs seed" << std::endl;
        return 1;
    }

    std::string path_to_SUT = argv[1];
    std::string path_to_inputs = argv[2];
    int seed = std::stoi(argv[3]);

    std::system("mkdir fuzzed-tests");

    std::list<std::string> inputs;

    for (const auto &entry : std::filesystem::directory_iterator(path_to_inputs))
    {
        // std::cout << entry.path() << std::endl;
        inputs.push_back(entry.path());
    }

    auto start_time = std::chrono::steady_clock::now();
    auto end_time = start_time + std::chrono::seconds(10);

    while (std::chrono::steady_clock::now() < end_time)
    {
        run_solver(path_to_SUT, new_input(seed));

        if (std::chrono::steady_clock::now() >= end_time)
        {
            break;
        }
    }

    return 0;
}
