#include "fuzzer.hpp"
#include "coverage.hpp"
#include "generate.hpp"

#define FUZZER_TIMEOUT 1800
#define SUT_TIMEOUT 15

#define FIFO_SIZE 5

#define GEN_MAX 100.0
#define MUT_MAX 3.0

#define GEN_START 0.6
#define MUT_START 0.6


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


int interruptible_system(const char *cmd) {
  if (cmd == NULL){
    return 1;
  }

  pid_t pid = fork();

  if (pid == -1){
    // Could not fork.
    return -1;
  } else if (pid == 0){
    // This is the child. Run the command.
    int exec_res = execl("/bin/sh", "sh", "-c", cmd, (char*) NULL);
    if (exec_res){
      exit(1);
    } else {
      exit(0);
    }
  
  } else {
    // Wait for all children to finish.
    int child_res;
    int wait_res = waitpid(-1, &child_res, 0);
    if (wait_res){
      return wait_res;
    } else {
      return child_res;
    }
  }
}

void initialise_saved_inputs(Input *saved) { 
  for (int i = 0; i < 20; i++) {
    saved[i].type = placeholder;
    saved[i].priority = 0;
    saved[i].hash = get_hash("empty");
  }
}

bool evaluate_input(Input *saved, undefined_behaviour_t type, std::size_t hash) {
  bool new_type = true;
  bool new_hash = true;

  for (int i = 0; i < 20; i++) {
    if (saved[i].type == type) {
      new_type = false;
    }
    
    if (saved[i].hash == hash) {
      new_hash = false;
    }
  }

  int priority = 0;  

  // Calculate priority from seen/unseen type or address
  if (type == no_error || type == uncategorized) {
    priority = 0;
  } else if (new_type && new_hash) {
    priority = 4;
  } else if (new_type && !new_hash) {
    priority = 3;
  } else if (!new_type && new_hash) {
    priority = 2;
  } else if (type == error) {
    priority = 2;  
  } else {
    priority = 1;
  }

  int min_priority = 99;
  int min_index = -1;

  // Get lowest priority input in the saved list
  for (int i = 0; i < 20; i++) {
    if (saved[i].priority < min_priority) {
      min_priority = saved[i].priority;
      min_index = i;
    }
  }

  if (min_priority != 99 && priority > min_priority) {
    std::cout << "Replacing input " << std::to_string(min_index) << " with new input: Priority: " << std::to_string(priority) << ", Type: " << std::to_string(type) << std::endl;
    
    std::string mv = "mv " + FILENAME + " fuzzed-tests/saved" + std::to_string(min_index) + ".cnf";
    counter++;
    std::system(mv.c_str());

    // Remove lowest priority from the list, append current input 
    saved[min_index].priority = priority;
    saved[min_index].type = type;    
    saved[min_index].hash = hash;
    return true;      
  } else {
    if (verbose)
      std::cout << "Input did not yield interesting output. Priority: " << std::to_string(priority) << ", Type: " << std::to_string(type) << std::endl;
  }

  return false;
}

bool run_solver(std::string path_to_SUT, Input *saved, std::string input)
{
    if (verbose) std::cout << "-----------------------------------------------------------------" << std::endl;

    create_file(FILENAME, input);
    // Read input file
    std::ifstream input_stream(FILENAME);
    std::string input_content((std::istreambuf_iterator<char>(input_stream)),
                              std::istreambuf_iterator<char>());
    // if (verbose) print_file(input_content, "INPUT");

    std::string run_solver = path_to_SUT + "/runsat.sh " + FILENAME;

    // Redirect SUT output to file
    std::string output_file = "output.txt";
    std::string redirect_output = run_solver + " > " + output_file + " 2>&1";
    interruptible_system(redirect_output.c_str());

    // Read output file
    std::ifstream output_stream(output_file);
    std::string output_content((std::istreambuf_iterator<char>(output_stream)),
                               std::istreambuf_iterator<char>());
    if (verbose) print_file(output_content, "OUTPUT");
    
    undefined_behaviour_t error_type = process_output(output_content);
    std::size_t hash = get_hash(output_content);
        
    return evaluate_input(saved, error_type, hash);
}

bool run_solver_with_timeout(std::string path_to_SUT, Input *saved, std::string input, std::chrono::seconds timeout)
{
    std::future<bool> solver_future = std::async(std::launch::async, run_solver, path_to_SUT, saved, input);

    if (solver_future.wait_for(timeout) == std::future_status::timeout)
    {
        // Timeout occurred, handle it accordingly
        std::cout << "Solver timed out!" << std::endl;
        std::string kill_solver = "killall runsat.sh";
        std::system(kill_solver.c_str());
        return false;
    } else {
        return solver_future.get();
    }
}

float check_coverage(std::string path_to_SUT, bool debug) {
  std::optional<coverage> arc_coverage = arc_coverage_all_files(path_to_SUT, debug);
  if (arc_coverage.has_value())
  { 
    coverage *progress = &arc_coverage.value();
    int exec = progress->arcs_executed;
    int total = progress->arcs;
    
    std::cout << "Overall Arc Coverage: " << exec << "/" << total
    << " (" << std::to_string(100*exec/total)<< "%)" << std::endl;
    
    return (100.0*exec/total);
  }
  else
  {
    std::cout << "Overall Arc Coverage: 0/0" << std::endl;
    return 0.0;
  }
}

void update_strategy(Strategy *strat) {
  
  strat->mut_strat = (mutation_strategy_t)((int)strat->mut_strat + 1);
 
  if (strat->mut_strat >= choose_mutate_strategy_end){
    strat->gen_strat = (generation_strategy_t)( ((int)strat->gen_strat + 1) % (int)choose_generate_strategy_end);
    strat->mut_strat = (mutation_strategy_t)( ((int)strat->mut_strat) % (int)choose_mutate_strategy_end);
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
    std::string seed_input = argv[3];
    int seed = std::stoi(seed_input);
    std::cout << std::to_string(argc) << std::endl;

    if (argc == 5)
    {
      std::cout << argv[4] << std::endl;
      std::string argument = argv[4];     
      verbose = "-verbose" == argument;
    }

    // Create directory for interesting inputs
    std::system("rm -rf fuzzed-tests");
    std::system("mkdir fuzzed-tests");

    // List of wellformed inputs
    std::list<std::string> inputs;

    for (const auto &entry : std::filesystem::directory_iterator(path_to_inputs))
        inputs.push_back(entry.path());

    Input saved_inputs[20];
    initialise_saved_inputs(saved_inputs);

    auto start_time = std::chrono::steady_clock::now();
    auto end_time = start_time + std::chrono::seconds(FUZZER_TIMEOUT);

    Strategy strategy = {
      .gen_strat = choose_generate_strategy_1_random,
      .mut_strat = choose_mutate_strategy_1_nothing, 
      .gen_aggresiveness = 0.6f, 
      .mut_aggresiveness = 0.6f, 
    };

    std::optional<coverage> aggregrate_coverage = {};
    std::string coverage_dir = std::string(path_to_SUT);
    // TODO: Remove this condition for release/submission
    if (path_to_SUT == "solvers/minisat/"){
      coverage_dir = std::string("solvers/minisat/core");
    }

    // Main loop
    while (std::chrono::steady_clock::now() < end_time)
    {

        // Number of iterations left for this strategy
        int strat_iterations_left = 10;

        // MAX number of iterations left for this strategy. The counter above
        // can be changed, but this acts as a backstop ensuring other strategies 
        // are also tried
        int max_strat_iterations_left = 50;

    
        std::deque<int> new_coverage_fifo = {};
    
        while (strat_iterations_left > 0 && max_strat_iterations_left > 0){

          if(strategy.gen_aggresiveness >= GEN_MAX){
            strategy.gen_aggresiveness = GEN_MAX;
          }

          // For the most complicated mutation strategy, reduce the generation
          // aggresive for faster iteration.  
          if(strategy.mut_strat == choose_mutate_strategy_15_controlled_chaos && strategy.gen_aggresiveness >= GEN_MAX / 2){
            strategy.gen_aggresiveness = GEN_MAX / 2;
          }

          if( (strategy.gen_strat == choose_generate_strategy_7_unsat_pigeonhole || strategy.gen_strat == choose_generate_strategy_8_unsat_pigeon_much_more_than_hole) && strategy.gen_aggresiveness >= GEN_MAX / 8){
            strategy.gen_aggresiveness = GEN_MAX / 8;
          }

          if(strategy.mut_aggresiveness >= MUT_MAX){
            strategy.mut_aggresiveness = MUT_MAX;
          }

      // // std::string generate_new_input(int seed, const Strategy *strat, bool verbose);
      //     std::packaged_task<std::string(int, const Strategy*, bool)> task(generate_new_input);         
      //     seed++;

      //     auto future = task.get_future();
      //     std::thread gen_thread(std::move(task), seed, &strategy, verbose);

      //     std::string input;
      //     if(future.wait_for(std::chrono::seconds(5)) != std::future_status::timeout){
      //       gen_thread.join();
      //       input = future.get();
      //     } else {

      //       std::cout << " Input generation timed out " << std::endl;
      //       int err = pthread_kill(gen_thread.native_handle(), 1);

      //       std::cout << " Managed to kill thread: " << err << std::endl;
      //       break;
      //     }
      
          bool found_new_bug = run_solver_with_timeout(path_to_SUT, saved_inputs, generate_new_input(seed++, &strategy, verbose) , std::chrono::seconds(SUT_TIMEOUT));

          // We found a new bug with the current strategy, try for longer:
          if (found_new_bug){
            strat_iterations_left += 10;
          }


          if(aggregrate_coverage.has_value() == false){
            aggregrate_coverage = arc_coverage_all_files(coverage_dir, false);
          }

          coverage cur_coverage = *arc_coverage_all_files(coverage_dir, false);
          coverage_diff coverage_diff = *calc_coverage_diff(&aggregrate_coverage.value(), &cur_coverage);
          calc_aggregrate_coverage(&aggregrate_coverage.value(), &cur_coverage);
          uint32_t new_arcs_discovered = coverage_diff.new_unique_arcs_executed;
          
          if (new_arcs_discovered > 0 && verbose){
            std::cout << "Discovered " << new_arcs_discovered << " new arcs." << std::endl;
          }

          print_coverage_info(&cur_coverage);
          
          new_coverage_fifo.push_front(new_arcs_discovered);

          int new_coverage_recently = 0;
          if (new_coverage_fifo.size() > FIFO_SIZE) {
            new_coverage_fifo.pop_back();
            for(size_t c = 0; c < new_coverage_fifo.size(); c++){
              new_coverage_recently += new_coverage_fifo[c];
            }

            if(new_coverage_recently < 2){
              strategy.gen_aggresiveness *= 1.4;
              strategy.mut_aggresiveness *= 1.06;
            } else {
              // We're finding new coverage, keep going!
              strat_iterations_left += 10;
            }        
          } else {
            // Dont do anything initially untill we have at least FIFO_SIZE
            // entries
          } 

          // std::cout << "New coverage size: " << new_coverage_fifo.size() << std::endl;
          // std::cout << "New coverage recently: " << new_coverage_recently << std::endl;

          strat_iterations_left--;
          max_strat_iterations_left--;
        }

    
        strategy.gen_aggresiveness = GEN_START;
        strategy.mut_aggresiveness = MUT_START;
        


        // max_coverage = std::max(curr, max_coverage);

        // Update strategy when coverage becomes stagnant
        update_strategy(&strategy);

        // Total time for fuzzing elapsed
        if (std::chrono::steady_clock::now() >= end_time)
            break;
    }

    // Once working will need to check coverage every loop
    // to make decisions on exploration vs exploitation   
    std::cout << "Aggregrate Coverage: " << std::endl;
    print_coverage_info(&aggregrate_coverage.value());
  
    //Print info about saved inputs  
    export_inputs_info(saved_inputs);
  
    return 0;
}

