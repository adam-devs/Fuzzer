#include <iostream>
#include <sstream>
#include <random>
#include <algorithm> 
#include <set> 

std::string random_mutate(
    std::string cnf_input          , 
    bool enable_num_vars_change    ,
    bool enable_num_clauses_change ,
    bool enable_correct_pline      ,
    bool enable_sign_flip          ,
    float prob_sign_flip           ,
    bool enable_EOL_deletion       ,
    float prob_EOL_deletion        ,
    bool enable_EOL_insertion      ,
    float prob_EOL_insertion       ,
    bool enable_variable_deletion  ,
    float prob_variable_deletion   ,
    bool enable_variable_insertion ,
    float prob_variable_insertion  ,
    bool enable_line_deletion      ,
    float prob_line_deletion       ,
    bool enable_line_insertion     ,
    float prob_line_insertion      ,
    bool enable_chunk_deletion     ,
    int chunk_deletion_times       ,
    bool enable_chunk_rearrange    ,
    int chunk_rearrange_times      , 
    unsigned int seed              ); 