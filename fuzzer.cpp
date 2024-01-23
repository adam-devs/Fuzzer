#include <string>
#include <iostream>
#include <filesystem>
#include <list>

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        std::cout << "Usage: " << argv[0] << " /path/to/SUT /path/to/inputs seed" << std::endl;
        return 1;
    }

    std::string path_to_SUT = argv[1];
    std::string path_to_inputs = argv[2];
    std::string seed = argv[3];

    std::system("mkdir fuzzed-tests");

    std::list<std::string> inputs;

    for (const auto &entry : std::filesystem::directory_iterator(path_to_inputs))
    {
        std::cout << entry.path() << std::endl;
        inputs.push_back(entry.path());
    }

    std::string run_solver = "." + path_to_SUT + "/runsat.sh " + inputs.front();
    std::system(run_solver.c_str());

    return 0;
}
