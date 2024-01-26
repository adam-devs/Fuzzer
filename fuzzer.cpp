#include "fuzzer.hpp"
#include <fstream>

std::string FILENAME = "current-test.cnf";
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

std::string exec(const char *cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    return result;
}

void print_file(std::string content, std::string label)
{
    std::cout << "~~~~~~~~~~~~~~~~~~~ " << label << " ~~~~~~~~~~~~~~~~~~~\n"
              << content
              << "~~~~~~~~~~~~~~~~~~~ " << label << " ~~~~~~~~~~~~~~~~~~~"
              << std::endl;
}

void run_solver(std::string path_to_SUT, std::string input)
{
    create_file(FILENAME, input);
    // Read input file
    std::ifstream input_stream(FILENAME);
    std::string input_content((std::istreambuf_iterator<char>(input_stream)),
                              std::istreambuf_iterator<char>());
    print_file(input_content, "INPUT");

    std::string run_solver = path_to_SUT + "/runsat.sh " + FILENAME;

    // Redirect SUT output to file
    std::string output_file = "output.txt";
    std::string redirect_output = run_solver + " > " + output_file + " 2>&1";
    std::system(redirect_output.c_str());

    // Read output file
    std::ifstream output_stream(output_file);
    std::string output_content((std::istreambuf_iterator<char>(output_stream)),
                               std::istreambuf_iterator<char>());
    print_file(output_content, "OUTPUT");

    // TODO: Selectively save interesting inputs
    if (counter < 20)
    {
        std::string mv = "mv " + FILENAME + " fuzzed-tests/saved" + std::to_string(counter++) + ".cnf";
        std::system(mv.c_str());
    }
}

#include <future>

void run_solver_with_timeout(std::string path_to_SUT, std::string input, std::chrono::seconds timeout)
{
    std::future<void> solver_future = std::async(std::launch::async, run_solver, path_to_SUT, input);

    if (solver_future.wait_for(timeout) == std::future_status::timeout)
    {
        // Timeout occurred, handle it accordingly
        std::cout << "Solver timed out!" << std::endl;
    }
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
    std::string seed = argv[3];

    // Create directory for interesting inputs
    std::system("mkdir fuzzed-tests");

    // List of wellformed inputs
    std::list<std::string> inputs;

    for (const auto &entry : std::filesystem::directory_iterator(path_to_inputs))
        inputs.push_back(entry.path());

    auto start_time = std::chrono::steady_clock::now();
    auto end_time = start_time + std::chrono::seconds(10);

    // Main loop
    while (std::chrono::steady_clock::now() < end_time)
    {
        std::cout << "-----------------------------------------------------------------" << std::endl;

        // run_solver(path_to_SUT, generate_new_input(seed));
        run_solver_with_timeout(path_to_SUT, generate_new_input(seed), std::chrono::seconds(5));

        if (std::chrono::steady_clock::now() >= end_time)
            break;
    }

    return 0;
}
