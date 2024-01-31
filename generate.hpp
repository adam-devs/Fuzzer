#ifndef GENERATE_HPP
#define GENERATE_HPP

#include <iostream>
#include <random>
#include <string>
#include <tuple>

// Enumeration of generation strategies 
enum generation_strategy_t
{
    choose_generate_strategy_1_random,
    choose_generate_strategy_2_random_with_pline,
    choose_generate_strategy_3_cnf,
    choose_generate_strategy_4_sat,
    choose_generate_strategy_5_cnf_omit_variable,
    choose_generate_strategy_6_unsat_combination,
    choose_generate_strategy_7_unsat_pigeonhole, 
    choose_generate_strategy_8_unsat_pigeon_much_more_than_hole,

    choose_generate_strategy_end,
}; 

// Enumeration of mutation strategies 
enum mutation_strategy_t
{
    choose_mutate_strategy_1_nothing,
    choose_mutate_strategy_2_chunk_deletion,
    choose_mutate_strategy_3_chunk_rearrange_once,
    choose_mutate_strategy_4_chunk_rearrange_multiple,
    choose_mutate_strategy_5_num_vars_clauses,
    choose_mutate_strategy_6_sign_flip,
    choose_mutate_strategy_7_eol_deletion,
    choose_mutate_strategy_8_eol_insertoin,
    choose_mutate_strategy_9_variable_deletion,
    choose_mutate_strategy_10_variable_insertion,
    choose_mutate_strategy_11_variable_shuffle,
    choose_mutate_strategy_12_line_deletion,
    choose_mutate_strategy_13_line_insertion,
    choose_mutate_strategy_14_line_shuffle,
    choose_mutate_strategy_15_controlled_chaos,

    choose_mutate_strategy_end,
}; 

typedef struct
{
  generation_strategy_t gen_strat; 
  mutation_strategy_t mut_strat;
  float aggresiveness;
} Strategy;

std::string generate_new_input(int seed, Strategy *strat, bool verbose);

#endif
