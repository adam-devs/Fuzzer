#include <iostream>
#include <sstream>
#include <random>
#include <algorithm> 
#include <set> 

std::string random_mutate(
    std::string cnf_input, 
    bool enable_num_vars_change     = false, // Risk low level error
    bool enable_num_clauses_change  = false, // Risk low level error
    bool enable_correct_pline       = false, // Recommended, overrides enable_num_vars_change and enable_num_clauses_change
    bool enable_sign_flip           = false, // No risk 
    float prob_sign_flip            = 0.1, 
    bool enable_EOL_deletion        = false, // Risk low level error
    float prob_EOL_deletion         = 0.1, 
    bool enable_EOL_insertion       = false, // Risk low level error
    float prob_EOL_insertion        = 0.02, 
    bool enable_variable_deletion   = false, // No risk with enable_correct_pline
    float prob_variable_deletion    = 0.1, 
    bool enable_variable_insertion  = false, // No risk 
    float prob_variable_insertion   = 0.1,
    bool enable_line_deletion       = false, // No risk with enable_correct_pline
    float prob_line_deletion        = 0.2, 
    bool enable_line_insertion      = false, // No risk with enable_correct_pline
    float prob_line_insertion       = 0.2, 
    bool enable_chunk_deletion      = false, // Risk low level error (Nuclear button)
    int chunk_deletion_times        = 1, 
    bool enable_chunk_rearrange     = false, // Risk low level error (Nuclear button)
    int chunk_rearrange_times       = 1, 
    unsigned int seed               = 123
    ); 