#include "generate.hpp"
#include "generate_sat.hpp"
#include "mutate.hpp"
#include "coverage.hpp" 

// =======================================
// ======== GENERATION STRATEGIES ========
// ======================================= 

// Generates a random string between provided limits 
std::string generate_random_string(std::mt19937 generator, long min_length, long max_length)
{
    // Distribution for generating string length 
    std::uniform_int_distribution<long> d_string_length(min_length, max_length); 

    // Draw string length 
    long string_length = d_string_length(generator); 

    std::string file_out; 
    file_out.reserve(string_length);

    // define usable characters 
    static const char usable_characters[] =
        " "
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "\n"
        "!@#$%^&*()_+-=;,./<>?|{}:";

    // Distribution for choosing unsable character 
    std::uniform_int_distribution<int> d_char_choice(0, sizeof(usable_characters) - 1); 
    
    for (int i = 0; i < string_length; i++)
    {
        file_out += usable_characters[d_char_choice(generator)];
    }
    
    return file_out; 
}

// ======== GENERATION STRATEGY #1 ========
// Generates a short random string 
std::string generate_strategy_1_random_short(std::mt19937 generator, float aggresiveness)
{
    long min_length = 100 * aggresiveness; 
    long max_length = 1000 * aggresiveness; 

    return generate_random_string(generator, min_length, max_length); 
}

// ======== GENERATION STRATEGY #2 ========
// Generates a long random string 
std::string generate_strategy_2_random_long(std::mt19937 generator, float aggresiveness)
{
    long min_length = 100 * aggresiveness; 
    long max_length = 100000 * aggresiveness; 

    return generate_random_string(generator, min_length, max_length); 
}

// ======== GENERATION STRATEGY #3 ========
// Generates a string with a dummy p_line  
std::string generate_strategy_3_random_with_pline(std::mt19937 generator, float aggresiveness)
{
    long min_length = 100 * aggresiveness; 
    long max_length = 1000 * aggresiveness; 

    std::uniform_int_distribution<int> d_num_vars_clauses(2, 30); 

    std::string dummy_p_line = "p cnf " + std::to_string(d_num_vars_clauses(generator)) + " " + std::to_string(d_num_vars_clauses(generator)) + "\n";

    return dummy_p_line + generate_random_string(generator, min_length, max_length); 
}

// ======== GENERATION STRATEGY #4 ========
// Generates a short well-formed cnf file 
std::string generate_strategy_4_cnf_short(std::mt19937 generator, int seed, float aggresiveness)
{
    int num_vars    = 10 * aggresiveness; 
    int num_clauses = 20 * aggresiveness; 
    int max_clauses = 20 * aggresiveness; 

    // Distribution for drawing number of variables and clauses 
    std::uniform_int_distribution<int> d_num_vars(2, num_vars); 
    std::uniform_int_distribution<int> d_num_clauses(1, num_clauses); 
    std::uniform_int_distribution<int> d_max_clauses(1, max_clauses); 

    return generate_cnf(d_num_vars(generator), d_num_clauses(generator), d_max_clauses(generator), seed); 
}

// ======== GENERATION STRATEGY #5 ========
// Generates a long well-formed cnf file 
std::string generate_strategy_5_cnf_long(std::mt19937 generator, int seed, float aggresiveness)
{
    int num_vars    = 10000 * aggresiveness; 
    int num_clauses = 20000 * aggresiveness; 
    int max_clauses = 20000 * aggresiveness; 

    // Distribution for drawing number of variables and clauses 
    std::uniform_int_distribution<int> d_num_vars(2, num_vars); 
    std::uniform_int_distribution<int> d_num_clauses(1, num_clauses); 
    std::uniform_int_distribution<int> d_max_clauses(1, max_clauses); 

    return generate_cnf(d_num_vars(generator), d_num_clauses(generator), d_max_clauses(generator), seed); 
}

// ======== GENERATION STRATEGY #6 ========
// Generates a short well-formed cnf file that is guaranteed to be SAT 
std::string generate_strategy_6_sat_short(std::mt19937 generator, int seed, float aggresiveness)
{
    int num_vars    = 10 * aggresiveness; 
    int num_clauses = 20 * aggresiveness; 
    int max_clauses = 20 * aggresiveness; 

    // Distribution for drawing number of variables and clauses 
    std::uniform_int_distribution<int> d_num_vars(2, num_vars); 
    std::uniform_int_distribution<int> d_num_clauses(1, num_clauses); 
    std::uniform_int_distribution<int> d_max_clauses(1, max_clauses); 

    return generate_sat(d_num_vars(generator), d_num_clauses(generator), d_max_clauses(generator), seed); 
}

// ======== GENERATION STRATEGY #7 ========
// Generates a long well-formed cnf file that is guaranteed to be SAT 
std::string generate_strategy_7_sat_long(std::mt19937 generator, int seed, float aggresiveness)
{
    int num_vars    = 10000 * aggresiveness; 
    int num_clauses = 20000 * aggresiveness; 
    int max_clauses = 20000 * aggresiveness; 

    // Distribution for drawing number of variables and clauses 
    std::uniform_int_distribution<int> d_num_vars(2, num_vars); 
    std::uniform_int_distribution<int> d_num_clauses(1, num_clauses); 
    std::uniform_int_distribution<int> d_max_clauses(1, max_clauses); 

    return generate_sat(d_num_vars(generator), d_num_clauses(generator), d_max_clauses(generator), seed); 
}

// ======== GENERATION STRATEGY #8 ========
// Generates a well-formed cnf file that omits variables in clauses 
std::string generate_strategy_8_cnf_omit_variable(std::mt19937 generator, int seed, float aggresiveness)
{
    int num_vars    = 1000 * aggresiveness; 
    int num_clauses = 200 * aggresiveness; 
    int max_clauses = 200 * aggresiveness; 

    // Distribution for drawing number of variables and clauses 
    std::uniform_int_distribution<int> d_num_vars(2, num_vars); 
    std::uniform_int_distribution<int> d_num_clauses(1, num_clauses); 
    std::uniform_int_distribution<int> d_max_clauses(1, max_clauses); 

    return generate_cnf(d_num_vars(generator), d_num_clauses(generator), d_max_clauses(generator), seed); 
}

// ======== GENERATION STRATEGY #9 ========
// Generates a short well-formed cnf file with combinations that is guaranteed UNSAT 
std::string generate_strategy_9_unsat_combination_short(std::mt19937 generator, float aggresiveness)
{
    int num_combination = 8 * aggresiveness; 

    // Distribution for drawing number of variables and clauses 
    std::uniform_int_distribution<int> d_num_combination(2, num_combination); 

    return generate_unsat_combination(d_num_combination(generator)); 
}

// ======== GENERATION STRATEGY #10 ========
// Generates a long well-formed cnf file with combinations that is guaranteed UNSAT 
std::string generate_strategy_10_unsat_combination_long(std::mt19937 generator, float aggresiveness)
{
    int num_combination = 18 * aggresiveness; 

    // Distribution for drawing number of variables and clauses 
    std::uniform_int_distribution<int> d_num_combination(8, num_combination); 

    return generate_unsat_combination(d_num_combination(generator)); 
}

// ======== GENERATION STRATEGY #11 ========
// Generates a short well-formed cnf file using pigeonhole that is guaranteed UNSAT 
std::string generate_strategy_11_unsat_pigeonhole_short(std::mt19937 generator, float aggresiveness)
{
    int num_pigeons = 12 * aggresiveness; 

    // Distribution for drawing number of variables and clauses 
    std::uniform_int_distribution<int> d_num_pigeons(2, num_pigeons); 
    num_pigeons = d_num_pigeons(generator); 

    return generate_unsat_pigeonhole(num_pigeons, num_pigeons - 1); 
}

// ======== GENERATION STRATEGY #12 ========
// Generates a long well-formed cnf file using pigeonhole that is guaranteed UNSAT 
std::string generate_strategy_12_unsat_pigeonhole_long(std::mt19937 generator, float aggresiveness)
{
    int num_pigeons = 40 * aggresiveness; 

    // Distribution for drawing number of variables and clauses 
    std::uniform_int_distribution<int> d_num_pigeons(10, num_pigeons); 
    num_pigeons = d_num_pigeons(generator); 

    return generate_unsat_pigeonhole(num_pigeons, num_pigeons - 1); 
}

// ======== GENERATION STRATEGY #13 ========
// Generates a long well-formed cnf file using pigeonhole that is guaranteed UNSAT
// Condition: num_pigeons is much greater than num_holes 
std::string generate_strategy_13_unsat_pigeon_much_more_than_hole(std::mt19937 generator, float aggresiveness)
{
    int num_pigeons = 30 * aggresiveness; 

    // Distribution for drawing number of variables and clauses 
    std::uniform_int_distribution<int> d_num_pigeons(5, num_pigeons); 
    num_pigeons = d_num_pigeons(generator); 

    return generate_unsat_pigeonhole(num_pigeons, num_pigeons/2); 
}

// ===================================== 
// ======== MUTATION STRATEGIES ======== 
// ===================================== 

// ======== MUTATION STRATEGY #1 ========
// Mutates nothing 
std::string mutate_strategy_1_nothing(std::string cnf_input)
{
    return cnf_input; 
}

// ======== MUTATION STRATEGY #2 ========
// Performs chunk deletion 
std::string mutate_strategy_2_chunk_deletion(std::string cnf_input, int seed, float aggresiveness)
{
    return random_mutate(
    cnf_input, 
    /* bool enable_num_vars_change     */ false, // Risk low level error
    /* bool enable_num_clauses_change  */ false, // Risk low level error
    /* bool enable_correct_pline       */ false, // Recommended, overrides enable_num_vars_change and enable_num_clauses_change
    /* bool enable_sign_flip           */ false, // No risk 
    /* float prob_sign_flip            */ 0.1, 
    /* bool enable_EOL_deletion        */ false, // Risk low level error
    /* float prob_EOL_deletion         */ 0.1, 
    /* bool enable_EOL_insertion       */ false, // Risk low level error
    /* float prob_EOL_insertion        */ 0.02, 
    /* bool enable_variable_deletion   */ false, // No risk with enable_correct_pline
    /* float prob_variable_deletion    */ 0.1, 
    /* bool enable_variable_insertion  */ false, // No risk 
    /* float prob_variable_insertion   */ 0.1,
    /* bool enable_line_deletion       */ false, // No risk with enable_correct_pline
    /* float prob_line_deletion        */ 0.2, 
    /* bool enable_line_insertion      */ false, // No risk with enable_correct_pline
    /* float prob_line_insertion       */ 0.2, 
    /* bool enable_chunk_deletion      */ true, // Risk low level error (Nuclear button)
    /* int chunk_deletion_times        */ 1 * aggresiveness, 
    /* bool enable_chunk_rearrange     */ false, // Risk low level error (Nuclear button)
    /* int chunk_rearrange_times       */ 1, 
    /* unsigned int seed               */ seed
    ); 
}

// ======== MUTATION STRATEGY #3 ========
// Performs chunk rearranging once * aggressiveness 
std::string mutate_strategy_3_chunk_rearrange_once(std::string cnf_input, int seed, float aggresiveness)
{
    return random_mutate(
    cnf_input, 
    /* bool enable_num_vars_change     */ false, // Risk low level error
    /* bool enable_num_clauses_change  */ false, // Risk low level error
    /* bool enable_correct_pline       */ false, // Recommended, overrides enable_num_vars_change and enable_num_clauses_change
    /* bool enable_sign_flip           */ false, // No risk 
    /* float prob_sign_flip            */ 0.1, 
    /* bool enable_EOL_deletion        */ false, // Risk low level error
    /* float prob_EOL_deletion         */ 0.1, 
    /* bool enable_EOL_insertion       */ false, // Risk low level error
    /* float prob_EOL_insertion        */ 0.02, 
    /* bool enable_variable_deletion   */ false, // No risk with enable_correct_pline
    /* float prob_variable_deletion    */ 0.1, 
    /* bool enable_variable_insertion  */ false, // No risk 
    /* float prob_variable_insertion   */ 0.1,
    /* bool enable_line_deletion       */ false, // No risk with enable_correct_pline
    /* float prob_line_deletion        */ 0.2, 
    /* bool enable_line_insertion      */ false, // No risk with enable_correct_pline
    /* float prob_line_insertion       */ 0.2, 
    /* bool enable_chunk_deletion      */ false, // Risk low level error (Nuclear button)
    /* int chunk_deletion_times        */ 1, 
    /* bool enable_chunk_rearrange     */ true, // Risk low level error (Nuclear button)
    /* int chunk_rearrange_times       */ 1 * aggresiveness, 
    /* unsigned int seed               */ seed
    ); 
}

// ======== MUTATION STRATEGY #4 ========
// Performs chunk rearranging multiple times 
std::string mutate_strategy_4_chunk_rearrange_multiple(std::string cnf_input, int seed, float aggresiveness)
{
    return random_mutate(
    cnf_input, 
    /* bool enable_num_vars_change     */ false, // Risk low level error
    /* bool enable_num_clauses_change  */ false, // Risk low level error
    /* bool enable_correct_pline       */ false, // Recommended, overrides enable_num_vars_change and enable_num_clauses_change
    /* bool enable_sign_flip           */ false, // No risk 
    /* float prob_sign_flip            */ 0.1, 
    /* bool enable_EOL_deletion        */ false, // Risk low level error
    /* float prob_EOL_deletion         */ 0.1, 
    /* bool enable_EOL_insertion       */ false, // Risk low level error
    /* float prob_EOL_insertion        */ 0.02, 
    /* bool enable_variable_deletion   */ false, // No risk with enable_correct_pline
    /* float prob_variable_deletion    */ 0.1, 
    /* bool enable_variable_insertion  */ false, // No risk 
    /* float prob_variable_insertion   */ 0.1,
    /* bool enable_line_deletion       */ false, // No risk with enable_correct_pline
    /* float prob_line_deletion        */ 0.2, 
    /* bool enable_line_insertion      */ false, // No risk with enable_correct_pline
    /* float prob_line_insertion       */ 0.2, 
    /* bool enable_chunk_deletion      */ false, // Risk low level error (Nuclear button)
    /* int chunk_deletion_times        */ 1, 
    /* bool enable_chunk_rearrange     */ true, // Risk low level error (Nuclear button)
    /* int chunk_rearrange_times       */ 3 * aggresiveness, 
    /* unsigned int seed               */ seed
    ); 
}

// ======== MUTATION STRATEGY #5 ========
// Mutate num_vars and num_clauses 
std::string mutate_strategy_5_num_vars_clauses(std::string cnf_input, int seed, float aggresiveness)
{
    return random_mutate(
    cnf_input, 
    /* bool enable_num_vars_change     */ true, // Risk low level error
    /* bool enable_num_clauses_change  */ true, // Risk low level error
    /* bool enable_correct_pline       */ false, // Recommended, overrides enable_num_vars_change and enable_num_clauses_change
    /* bool enable_sign_flip           */ false, // No risk 
    /* float prob_sign_flip            */ 0.1, 
    /* bool enable_EOL_deletion        */ false, // Risk low level error
    /* float prob_EOL_deletion         */ 0.1, 
    /* bool enable_EOL_insertion       */ false, // Risk low level error
    /* float prob_EOL_insertion        */ 0.02, 
    /* bool enable_variable_deletion   */ false, // No risk with enable_correct_pline
    /* float prob_variable_deletion    */ 0.1, 
    /* bool enable_variable_insertion  */ false, // No risk 
    /* float prob_variable_insertion   */ 0.1,
    /* bool enable_line_deletion       */ false, // No risk with enable_correct_pline
    /* float prob_line_deletion        */ 0.2, 
    /* bool enable_line_insertion      */ false, // No risk with enable_correct_pline
    /* float prob_line_insertion       */ 0.2, 
    /* bool enable_chunk_deletion      */ false, // Risk low level error (Nuclear button)
    /* int chunk_deletion_times        */ 1, 
    /* bool enable_chunk_rearrange     */ false, // Risk low level error (Nuclear button)
    /* int chunk_rearrange_times       */ 1, 
    /* unsigned int seed               */ seed
    ); 
}

// ======== MUTATION STRATEGY #6 ========
// Mutate lines with the same probability of insertion and deletion
std::string mutate_strategy_6_sign_flip(std::string cnf_input, int seed, float aggresiveness)
{
    return random_mutate(
    cnf_input, 
    /* bool enable_num_vars_change     */ false, // Risk low level error
    /* bool enable_num_clauses_change  */ false, // Risk low level error
    /* bool enable_correct_pline       */ false, // Recommended, overrides enable_num_vars_change and enable_num_clauses_change
    /* bool enable_sign_flip           */ true, // No risk 
    /* float prob_sign_flip            */ 0.1 * aggresiveness, 
    /* bool enable_EOL_deletion        */ false, // Risk low level error
    /* float prob_EOL_deletion         */ 0.1, 
    /* bool enable_EOL_insertion       */ false, // Risk low level error
    /* float prob_EOL_insertion        */ 0.02, 
    /* bool enable_variable_deletion   */ false, // No risk with enable_correct_pline
    /* float prob_variable_deletion    */ 0.1, 
    /* bool enable_variable_insertion  */ false, // No risk 
    /* float prob_variable_insertion   */ 0.1,
    /* bool enable_line_deletion       */ false , // No risk with enable_correct_pline
    /* float prob_line_deletion        */ 0.2, 
    /* bool enable_line_insertion      */ false, // No risk with enable_correct_pline
    /* float prob_line_insertion       */ 0.2, 
    /* bool enable_chunk_deletion      */ false, // Risk low level error (Nuclear button)
    /* int chunk_deletion_times        */ 1, 
    /* bool enable_chunk_rearrange     */ false, // Risk low level error (Nuclear button)
    /* int chunk_rearrange_times       */ 1, 
    /* unsigned int seed               */ seed
    ); 
}

// ======== MUTATION STRATEGY #7 ========
// Enable EOL deletion 
std::string mutate_strategy_7_eol_deletion(std::string cnf_input, int seed, float aggresiveness)
{
    return random_mutate(
    cnf_input, 
    /* bool enable_num_vars_change     */ false, // Risk low level error
    /* bool enable_num_clauses_change  */ false, // Risk low level error
    /* bool enable_correct_pline       */ false, // Recommended, overrides enable_num_vars_change and enable_num_clauses_change
    /* bool enable_sign_flip           */ false, // No risk 
    /* float prob_sign_flip            */ 0.1, 
    /* bool enable_EOL_deletion        */ true, // Risk low level error
    /* float prob_EOL_deletion         */ 0.1 * aggresiveness, 
    /* bool enable_EOL_insertion       */ false, // Risk low level error
    /* float prob_EOL_insertion        */ 0.02, 
    /* bool enable_variable_deletion   */ false, // No risk with enable_correct_pline
    /* float prob_variable_deletion    */ 0.1, 
    /* bool enable_variable_insertion  */ false, // No risk 
    /* float prob_variable_insertion   */ 0.1,
    /* bool enable_line_deletion       */ false, // No risk with enable_correct_pline
    /* float prob_line_deletion        */ 0.2, 
    /* bool enable_line_insertion      */ false, // No risk with enable_correct_pline
    /* float prob_line_insertion       */ 0.2, 
    /* bool enable_chunk_deletion      */ false, // Risk low level error (Nuclear button)
    /* int chunk_deletion_times        */ 1, 
    /* bool enable_chunk_rearrange     */ false, // Risk low level error (Nuclear button)
    /* int chunk_rearrange_times       */ 1, 
    /* unsigned int seed               */ seed
    ); 
}

// ======== MUTATION STRATEGY #8 ========
// Enable EOL insertion
std::string mutate_strategy_8_eol_insertoin(std::string cnf_input, int seed, float aggresiveness)
{
    return random_mutate(
    cnf_input, 
    /* bool enable_num_vars_change     */ false, // Risk low level error
    /* bool enable_num_clauses_change  */ false, // Risk low level error
    /* bool enable_correct_pline       */ false, // Recommended, overrides enable_num_vars_change and enable_num_clauses_change
    /* bool enable_sign_flip           */ false, // No risk 
    /* float prob_sign_flip            */ 0.1, 
    /* bool enable_EOL_deletion        */ false, // Risk low level error
    /* float prob_EOL_deletion         */ 0.1, 
    /* bool enable_EOL_insertion       */ true, // Risk low level error
    /* float prob_EOL_insertion        */ 0.05 * aggresiveness, 
    /* bool enable_variable_deletion   */ false, // No risk with enable_correct_pline
    /* float prob_variable_deletion    */ 0.1, 
    /* bool enable_variable_insertion  */ false, // No risk 
    /* float prob_variable_insertion   */ 0.1,
    /* bool enable_line_deletion       */ false, // No risk with enable_correct_pline
    /* float prob_line_deletion        */ 0.2, 
    /* bool enable_line_insertion      */ false, // No risk with enable_correct_pline
    /* float prob_line_insertion       */ 0.2, 
    /* bool enable_chunk_deletion      */ false, // Risk low level error (Nuclear button)
    /* int chunk_deletion_times        */ 1, 
    /* bool enable_chunk_rearrange     */ false, // Risk low level error (Nuclear button)
    /* int chunk_rearrange_times       */ 1, 
    /* unsigned int seed               */ seed
    ); 
}

// ======== MUTATION STRATEGY #9 ========
// Enable variable deletion
std::string mutate_strategy_9_variable_deletion(std::string cnf_input, int seed, float aggresiveness)
{
    return random_mutate(
    cnf_input, 
    /* bool enable_num_vars_change     */ false, // Risk low level error
    /* bool enable_num_clauses_change  */ false, // Risk low level error
    /* bool enable_correct_pline       */ true, // Recommended, overrides enable_num_vars_change and enable_num_clauses_change
    /* bool enable_sign_flip           */ false, // No risk 
    /* float prob_sign_flip            */ 0.1, 
    /* bool enable_EOL_deletion        */ false, // Risk low level error
    /* float prob_EOL_deletion         */ 0.1, 
    /* bool enable_EOL_insertion       */ false, // Risk low level error
    /* float prob_EOL_insertion        */ 0.02, 
    /* bool enable_variable_deletion   */ true, // No risk with enable_correct_pline
    /* float prob_variable_deletion    */ 0.1 * aggresiveness, 
    /* bool enable_variable_insertion  */ false, // No risk 
    /* float prob_variable_insertion   */ 0.1,
    /* bool enable_line_deletion       */ false, // No risk with enable_correct_pline
    /* float prob_line_deletion        */ 0.2, 
    /* bool enable_line_insertion      */ false, // No risk with enable_correct_pline
    /* float prob_line_insertion       */ 0.2, 
    /* bool enable_chunk_deletion      */ false, // Risk low level error (Nuclear button)
    /* int chunk_deletion_times        */ 1, 
    /* bool enable_chunk_rearrange     */ false, // Risk low level error (Nuclear button)
    /* int chunk_rearrange_times       */ 1, 
    /* unsigned int seed               */ seed
    ); 
}

// ======== MUTATION STRATEGY #10 ========
// Enable variable insertion
std::string mutate_strategy_10_variable_insertion(std::string cnf_input, int seed, float aggresiveness)
{
    return random_mutate(
    cnf_input, 
    /* bool enable_num_vars_change     */ false, // Risk low level error
    /* bool enable_num_clauses_change  */ false, // Risk low level error
    /* bool enable_correct_pline       */ true, // Recommended, overrides enable_num_vars_change and enable_num_clauses_change
    /* bool enable_sign_flip           */ false, // No risk 
    /* float prob_sign_flip            */ 0.1, 
    /* bool enable_EOL_deletion        */ false, // Risk low level error
    /* float prob_EOL_deletion         */ 0.1, 
    /* bool enable_EOL_insertion       */ false, // Risk low level error
    /* float prob_EOL_insertion        */ 0.02, 
    /* bool enable_variable_deletion   */ false, // No risk with enable_correct_pline
    /* float prob_variable_deletion    */ 0.1, 
    /* bool enable_variable_insertion  */ true, // No risk 
    /* float prob_variable_insertion   */ 0.1 * aggresiveness,
    /* bool enable_line_deletion       */ false, // No risk with enable_correct_pline
    /* float prob_line_deletion        */ 0.2, 
    /* bool enable_line_insertion      */ false, // No risk with enable_correct_pline
    /* float prob_line_insertion       */ 0.2, 
    /* bool enable_chunk_deletion      */ false, // Risk low level error (Nuclear button)
    /* int chunk_deletion_times        */ 1, 
    /* bool enable_chunk_rearrange     */ false, // Risk low level error (Nuclear button)
    /* int chunk_rearrange_times       */ 1, 
    /* unsigned int seed               */ seed
    ); 
}

// ======== MUTATION STRATEGY #11 ========
// Mutate variables with the same probability of insertion and deletion
std::string mutate_strategy_11_variable_shuffle(std::string cnf_input, int seed, float aggresiveness)
{
    return random_mutate(
    cnf_input, 
    /* bool enable_num_vars_change     */ false, // Risk low level error
    /* bool enable_num_clauses_change  */ false, // Risk low level error
    /* bool enable_correct_pline       */ true, // Recommended, overrides enable_num_vars_change and enable_num_clauses_change
    /* bool enable_sign_flip           */ false, // No risk 
    /* float prob_sign_flip            */ 0.1, 
    /* bool enable_EOL_deletion        */ false, // Risk low level error
    /* float prob_EOL_deletion         */ 0.1, 
    /* bool enable_EOL_insertion       */ false, // Risk low level error
    /* float prob_EOL_insertion        */ 0.02, 
    /* bool enable_variable_deletion   */ true, // No risk with enable_correct_pline
    /* float prob_variable_deletion    */ 0.1 * aggresiveness,
    /* bool enable_variable_insertion  */ true, // No risk 
    /* float prob_variable_insertion   */ 0.1 * aggresiveness,
    /* bool enable_line_deletion       */ false, // No risk with enable_correct_pline
    /* float prob_line_deletion        */ 0.2, 
    /* bool enable_line_insertion      */ false, // No risk with enable_correct_pline
    /* float prob_line_insertion       */ 0.2, 
    /* bool enable_chunk_deletion      */ false, // Risk low level error (Nuclear button)
    /* int chunk_deletion_times        */ 1, 
    /* bool enable_chunk_rearrange     */ false, // Risk low level error (Nuclear button)
    /* int chunk_rearrange_times       */ 1, 
    /* unsigned int seed               */ seed
    ); 
}

// ======== MUTATION STRATEGY #12 ========
// Enable line deletion
std::string mutate_strategy_12_line_deletion(std::string cnf_input, int seed, float aggresiveness)
{
    return random_mutate(
    cnf_input, 
    /* bool enable_num_vars_change     */ false, // Risk low level error
    /* bool enable_num_clauses_change  */ false, // Risk low level error
    /* bool enable_correct_pline       */ true, // Recommended, overrides enable_num_vars_change and enable_num_clauses_change
    /* bool enable_sign_flip           */ false, // No risk 
    /* float prob_sign_flip            */ 0.1, 
    /* bool enable_EOL_deletion        */ false, // Risk low level error
    /* float prob_EOL_deletion         */ 0.1, 
    /* bool enable_EOL_insertion       */ false, // Risk low level error
    /* float prob_EOL_insertion        */ 0.02, 
    /* bool enable_variable_deletion   */ false, // No risk with enable_correct_pline
    /* float prob_variable_deletion    */ 0.1, 
    /* bool enable_variable_insertion  */ false, // No risk 
    /* float prob_variable_insertion   */ 0.1,
    /* bool enable_line_deletion       */ true, // No risk with enable_correct_pline
    /* float prob_line_deletion        */ 0.2 * aggresiveness, 
    /* bool enable_line_insertion      */ false, // No risk with enable_correct_pline
    /* float prob_line_insertion       */ 0.2, 
    /* bool enable_chunk_deletion      */ false, // Risk low level error (Nuclear button)
    /* int chunk_deletion_times        */ 1, 
    /* bool enable_chunk_rearrange     */ false, // Risk low level error (Nuclear button)
    /* int chunk_rearrange_times       */ 1, 
    /* unsigned int seed               */ seed
    ); 
}

// ======== MUTATION STRATEGY #13 ========
// Enable line insertion
std::string mutate_strategy_13_line_insertion(std::string cnf_input, int seed, float aggresiveness)
{
    return random_mutate(
    cnf_input, 
    /* bool enable_num_vars_change     */ false, // Risk low level error
    /* bool enable_num_clauses_change  */ false, // Risk low level error
    /* bool enable_correct_pline       */ true, // Recommended, overrides enable_num_vars_change and enable_num_clauses_change
    /* bool enable_sign_flip           */ false, // No risk 
    /* float prob_sign_flip            */ 0.1, 
    /* bool enable_EOL_deletion        */ false, // Risk low level error
    /* float prob_EOL_deletion         */ 0.1, 
    /* bool enable_EOL_insertion       */ false, // Risk low level error
    /* float prob_EOL_insertion        */ 0.02, 
    /* bool enable_variable_deletion   */ false, // No risk with enable_correct_pline
    /* float prob_variable_deletion    */ 0.1, 
    /* bool enable_variable_insertion  */ false, // No risk 
    /* float prob_variable_insertion   */ 0.1,
    /* bool enable_line_deletion       */ false, // No risk with enable_correct_pline
    /* float prob_line_deletion        */ 0.2, 
    /* bool enable_line_insertion      */ true, // No risk with enable_correct_pline
    /* float prob_line_insertion       */ 0.2 * aggresiveness, 
    /* bool enable_chunk_deletion      */ false, // Risk low level error (Nuclear button)
    /* int chunk_deletion_times        */ 1, 
    /* bool enable_chunk_rearrange     */ false, // Risk low level error (Nuclear button)
    /* int chunk_rearrange_times       */ 1, 
    /* unsigned int seed               */ seed
    ); 
}

// ======== MUTATION STRATEGY #14 ========
// Mutate lines with the same probability of insertion and deletion
std::string mutate_strategy_14_line_shuffle(std::string cnf_input, int seed, float aggresiveness)
{
    return random_mutate(
    cnf_input, 
    /* bool enable_num_vars_change     */ false, // Risk low level error
    /* bool enable_num_clauses_change  */ false, // Risk low level error
    /* bool enable_correct_pline       */ true, // Recommended, overrides enable_num_vars_change and enable_num_clauses_change
    /* bool enable_sign_flip           */ false, // No risk 
    /* float prob_sign_flip            */ 0.1, 
    /* bool enable_EOL_deletion        */ false, // Risk low level error
    /* float prob_EOL_deletion         */ 0.1, 
    /* bool enable_EOL_insertion       */ false, // Risk low level error
    /* float prob_EOL_insertion        */ 0.02, 
    /* bool enable_variable_deletion   */ false, // No risk with enable_correct_pline
    /* float prob_variable_deletion    */ 0.1, 
    /* bool enable_variable_insertion  */ false, // No risk 
    /* float prob_variable_insertion   */ 0.1,
    /* bool enable_line_deletion       */ true , // No risk with enable_correct_pline
    /* float prob_line_deletion        */ 0.2 * aggresiveness, 
    /* bool enable_line_insertion      */ true, // No risk with enable_correct_pline
    /* float prob_line_insertion       */ 0.2 * aggresiveness, 
    /* bool enable_chunk_deletion      */ false, // Risk low level error (Nuclear button)
    /* int chunk_deletion_times        */ 1, 
    /* bool enable_chunk_rearrange     */ false, // Risk low level error (Nuclear button)
    /* int chunk_rearrange_times       */ 1, 
    /* unsigned int seed               */ seed
    ); 
}

// ======== MUTATION STRATEGY #15 ========
// Enable all toggles that would still result in a well-formed cnf file 
std::string mutate_strategy_15_controlled_chaos(std::string cnf_input, int seed, float aggresiveness)
{
    return random_mutate(
    cnf_input, 
    /* bool enable_num_vars_change     */ false, // Risk low level error
    /* bool enable_num_clauses_change  */ false, // Risk low level error
    /* bool enable_correct_pline       */ true, // Recommended, overrides enable_num_vars_change and enable_num_clauses_change
    /* bool enable_sign_flip           */ true, // No risk 
    /* float prob_sign_flip            */ 0.1 * aggresiveness, 
    /* bool enable_EOL_deletion        */ false, // Risk low level error
    /* float prob_EOL_deletion         */ 0.1, 
    /* bool enable_EOL_insertion       */ false, // Risk low level error
    /* float prob_EOL_insertion        */ 0.02, 
    /* bool enable_variable_deletion   */ true, // No risk with enable_correct_pline
    /* float prob_variable_deletion    */ 0.1 * aggresiveness, 
    /* bool enable_variable_insertion  */ true, // No risk 
    /* float prob_variable_insertion   */ 0.1 * aggresiveness,
    /* bool enable_line_deletion       */ true , // No risk with enable_correct_pline
    /* float prob_line_deletion        */ 0.2 * aggresiveness, 
    /* bool enable_line_insertion      */ true, // No risk with enable_correct_pline
    /* float prob_line_insertion       */ 0.2 * aggresiveness, 
    /* bool enable_chunk_deletion      */ false, // Risk low level error (Nuclear button)
    /* int chunk_deletion_times        */ 1, 
    /* bool enable_chunk_rearrange     */ false, // Risk low level error (Nuclear button)
    /* int chunk_rearrange_times       */ 1, 
    /* unsigned int seed               */ seed
    ); 
}


std::string generate_new_input(int seed, std::tuple<generation_strategy_t,mutation_strategy_t,float> *strat, bool verbose = false)
{   

    // Get command from top level 
    generation_strategy_t generation_strategy = std::get<0>(*strat);
    mutation_strategy_t   mutation_strategy   = std::get<1>(*strat);

    float aggresiveness = std::get<2>(*strat); 

    if (verbose) 
    {
      std::cout << "Gen_Strat: " << generation_strategy << " Mutate_Strat: " << mutation_strategy << " Scaling: " << aggresiveness << std::endl;    
    }

    // Initate generator for parameter generation 
    std::random_device rd;
    std::mt19937 generator(rd());
    generator.seed(seed); 

    std::string cnf_file; 
    
    // Choose generation strategy
    switch (generation_strategy) 
    {
        case choose_generate_strategy_1_random_short:
            cnf_file = generate_strategy_1_random_short(generator, aggresiveness);
        case choose_generate_strategy_2_random_long:
            cnf_file = generate_strategy_2_random_long(generator, aggresiveness);
        case choose_generate_strategy_3_random_with_pline:
            cnf_file = generate_strategy_3_random_with_pline(generator, aggresiveness); 
        case choose_generate_strategy_4_cnf_short:
            cnf_file = generate_strategy_4_cnf_short(generator, seed, aggresiveness); 
        case choose_generate_strategy_5_cnf_long:
            cnf_file = generate_strategy_5_cnf_long(generator, seed, aggresiveness); 
        case choose_generate_strategy_6_sat_short: 
            cnf_file = generate_strategy_6_sat_short(generator, seed, aggresiveness); 
        case choose_generate_strategy_7_sat_long: 
            cnf_file = generate_strategy_7_sat_long(generator, seed, aggresiveness); 
        case choose_generate_strategy_8_cnf_omit_variable: 
            cnf_file = generate_strategy_8_cnf_omit_variable(generator, seed, aggresiveness); 
        case choose_generate_strategy_9_unsat_combination_short:  
            cnf_file = generate_strategy_9_unsat_combination_short(generator, aggresiveness); 
        case choose_generate_strategy_10_unsat_combination_long: 
            cnf_file = generate_strategy_10_unsat_combination_long(generator, aggresiveness);
        case choose_generate_strategy_11_unsat_pigeonhole_short: 
            cnf_file = generate_strategy_11_unsat_pigeonhole_short(generator, aggresiveness); 
        case choose_generate_strategy_12_unsat_pigeonhole_long: 
            cnf_file = generate_strategy_12_unsat_pigeonhole_long(generator, aggresiveness); 
        case choose_generate_strategy_13_unsat_pigeon_much_more_than_hole: 
            cnf_file = generate_strategy_13_unsat_pigeon_much_more_than_hole(generator, aggresiveness); 
        default:
            cnf_file = generate_strategy_4_cnf_short(generator, seed, aggresiveness); 
    }

    // Choose mutation strategy 
    switch (mutation_strategy)
    {
        case choose_mutate_strategy_1_nothing: 
            cnf_file = mutate_strategy_1_nothing(cnf_file); 
        case choose_mutate_strategy_2_chunk_deletion: 
            cnf_file = mutate_strategy_2_chunk_deletion(cnf_file, seed, aggresiveness); 
        case choose_mutate_strategy_3_chunk_rearrange_once: 
            cnf_file = mutate_strategy_3_chunk_rearrange_once(cnf_file, seed, aggresiveness); 
        case choose_mutate_strategy_4_chunk_rearrange_multiple: 
            cnf_file = mutate_strategy_4_chunk_rearrange_multiple(cnf_file, seed, aggresiveness); 
        case choose_mutate_strategy_5_num_vars_clauses: 
            cnf_file = mutate_strategy_5_num_vars_clauses(cnf_file, seed, aggresiveness); 
        case choose_mutate_strategy_6_sign_flip: 
            cnf_file = mutate_strategy_6_sign_flip(cnf_file, seed, aggresiveness); 
        case choose_mutate_strategy_7_eol_deletion: 
            cnf_file = mutate_strategy_7_eol_deletion(cnf_file, seed, aggresiveness); 
        case choose_mutate_strategy_8_eol_insertoin: 
            cnf_file = mutate_strategy_8_eol_insertoin(cnf_file, seed, aggresiveness); 
        case choose_mutate_strategy_9_variable_deletion: 
            cnf_file = mutate_strategy_9_variable_deletion(cnf_file, seed, aggresiveness); 
        case choose_mutate_strategy_10_variable_insertion: 
            cnf_file = mutate_strategy_10_variable_insertion(cnf_file, seed, aggresiveness); 
        case choose_mutate_strategy_11_variable_shuffle: 
            cnf_file = mutate_strategy_11_variable_shuffle(cnf_file, seed, aggresiveness); 
        case choose_mutate_strategy_12_line_deletion:
            cnf_file = mutate_strategy_12_line_deletion(cnf_file, seed, aggresiveness); 
        case choose_mutate_strategy_13_line_insertion: 
            cnf_file = mutate_strategy_13_line_insertion(cnf_file, seed, aggresiveness); 
        case choose_mutate_strategy_14_line_shuffle: 
            cnf_file = mutate_strategy_14_line_shuffle(cnf_file, seed, aggresiveness); 
        case choose_mutate_strategy_15_controlled_chaos: 
            cnf_file = mutate_strategy_15_controlled_chaos(cnf_file, seed, aggresiveness); 
        default: 
            cnf_file = mutate_strategy_1_nothing(cnf_file); 
    }
    
    return cnf_file; 
}
