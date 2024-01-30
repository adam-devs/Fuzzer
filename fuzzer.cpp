#include "fuzzer.hpp"
#include "coverage.hpp"
#include "generate.hpp"

std::string FILENAME = "current-test.cnf";
int counter = 0;
bool verbose = false;

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

void run_solver(std::string path_to_SUT, std::string input)
{
    if (verbose) std::cout << "-----------------------------------------------------------------" << std::endl;

    create_file(FILENAME, input);
    // Read input file
    std::ifstream input_stream(FILENAME);
    std::string input_content((std::istreambuf_iterator<char>(input_stream)),
                              std::istreambuf_iterator<char>());
    if (verbose) print_file(input_content, "INPUT");

    std::string run_solver = path_to_SUT + "/runsat.sh " + FILENAME;

    // Redirect SUT output to file
    std::string output_file = "output.txt";
    std::string redirect_output = run_solver + " > " + output_file + " 2>&1";
    std::system(redirect_output.c_str());

    // Read output file
    std::ifstream output_stream(output_file);
    std::string output_content((std::istreambuf_iterator<char>(output_stream)),
                               std::istreambuf_iterator<char>());
    if (verbose) print_file(output_content, "OUTPUT");

    // TODO: Selectively save interesting inputs
    if (counter < 20)
    {
        std::string mv = "mv " + FILENAME + " fuzzed-tests/saved" + std::to_string(counter++) + ".cnf";
        std::system(mv.c_str());
    }
}

void run_solver_with_timeout(std::string path_to_SUT, std::string input, std::chrono::seconds timeout)
{
    std::future<void> solver_future = std::async(std::launch::async, run_solver, path_to_SUT, input);

    if (solver_future.wait_for(timeout) == std::future_status::timeout)
    {
        // Timeout occurred, handle it accordingly
        std::cout << "Solver timed out!" << std::endl;
        std::string kill_solver = "killall runsat.sh";
        std::system(kill_solver.c_str());
    }
}

void check_coverage(std::string path_to_SUT, bool debug) {
  std::optional<coverage> arc_coverage = arc_coverage_all_files(path_to_SUT, debug);
  if (arc_coverage.has_value())
  { 
    coverage *progress = &arc_coverage.value();
    int exec = progress->arcs_executed;
    int total = progress->arcs;
    
    std::cout << "Overall Arc Coverage: " << exec << "/" << total
    << " (" << std::to_string(100*exec/total)<< "%)" << std::endl;
  }
  else
  {
    std::cout << "Overall Arc Coverage: 0/0" << std::endl;
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
    std::cout << std::to_string(argc) << std::endl;

    if (argc == 5)
    {
      std::cout << argv[4] << std::endl;
      std::string argument = argv[4];     
      verbose = "-verbose" == argument;
    }

    // Create directory for interesting inputs
    std::system("mkdir fuzzed-tests");

    // List of wellformed inputs
    std::list<std::string> inputs;

    for (const auto &entry : std::filesystem::directory_iterator(path_to_inputs))
        inputs.push_back(entry.path());

    auto start_time = std::chrono::steady_clock::now();
    auto end_time = start_time + std::chrono::seconds(30);

    int action = 0;
    int n = 5;

    // Main loop
    while (std::chrono::steady_clock::now() < end_time)
    {
        // Select action as an even split of the time available for n actions
        int next_action = n - std::chrono::duration_cast<std::chrono::seconds>(end_time - std::chrono::steady_clock::now()).count() / (n + 1);

        if (next_action != action) {
          action = next_action;
          std::cout << "-------------------- Performing action " << std::to_string(action) << " ----------------------" << std::endl;  
        }

        // Run the solver allowing for a timeout of 5 seconds
        run_solver_with_timeout(path_to_SUT, generate_new_input(seed, action, verbose), std::chrono::seconds(5));

        // Total time for fuzzing elapsed
        if (std::chrono::steady_clock::now() >= end_time)
            break;
    }

    // Once working will need to check coverage every loop
    // to make decisions on exploration vs exploitation
    check_coverage(path_to_SUT, verbose);

    return 0;
}

