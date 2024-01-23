#include "fuzzer.hpp"

std::string FILENAME = "new_test.cnf";
int counter = 0;

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
    std::system("cat new_test.cnf \n echo \n");

    std::string run_solver = path_to_SUT + "/runsat.sh " + FILENAME;
    std::system(run_solver.c_str());

    std::string mv = "mv " + FILENAME + " fuzzed-tests/saved" + std::to_string(counter++) + ".cnf";
    std::system(mv.c_str());
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
        inputs.push_back(entry.path());

    auto start_time = std::chrono::steady_clock::now();
    auto end_time = start_time + std::chrono::seconds(10);

    while (std::chrono::steady_clock::now() < end_time)
    {
        std::cout << "----------------------------------------------------" << std::endl;
        run_solver(path_to_SUT, generate_new_input(seed));

        if (std::chrono::steady_clock::now() >= end_time)
            break;
    }

    return 0;
}
