#include "mutate.hpp"

/*
Ideas: 

Mutate variables into utf-8 
Mutate very big variables 
Mutate non-existing variables 
Merge two CNF files 

*/

std::string generate_variable(std::set<std::string> variable_list, std::mt19937 generator)
{
    std::uniform_int_distribution<int> d_sel_var(0, variable_list.size() - 1);
    std::bernoulli_distribution d_bool(0.5); 
    // Get random element in 
    int set_index = d_sel_var(generator); 
    auto selected_variable = next(variable_list.begin(), set_index);
    std::string selected_sign = d_bool(generator) ? "" : "-"; 
    return selected_sign + *selected_variable; 
}

// Requires function generate_var 
std::string generate_line(std::set<std::string> variable_list, int max_length, std::mt19937 generator)
{   
    std::string generated_line; 
    std::uniform_int_distribution<int> d_max_length(0, max_length - 1);

    for (int i = 0; i < max_length; i++)
    {
        generated_line += generate_variable(variable_list, generator) + " "; 

        // Break randomly to create random clause lengths
        if (d_max_length(generator) < i)
        {
            break; 
        }
    }

    return generated_line + "0";
}

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
    int seed                        = 1234
    )
{
    std::istringstream  isstream(cnf_input); 
    std::string         line; 

    std::random_device rd;
    std::mt19937 generator(rd());
    generator.seed(seed); 

    bool in_main_body = false; 

    // Record main body and prefix for chunk manipulation 
    std::string prefix_string; 
    std::string p_line_string; 
    std::string main_body_string; 
    
    // Record average line length for line insertion generation
    int avg_line_length = 10; 

    // Global list of variables 
    std::set<std::string> variable_list; 

    // Distributions for boolean mutation choices 
    std::bernoulli_distribution d_sign_flip(prob_sign_flip);
    std::bernoulli_distribution d_EOL_deletion(prob_EOL_deletion); 
    std::bernoulli_distribution d_EOL_insertion(prob_EOL_deletion); 
    std::bernoulli_distribution d_variable_deletion(prob_variable_deletion);
    std::bernoulli_distribution d_variable_insertion(prob_variable_insertion);
    std::bernoulli_distribution d_line_deletion(prob_line_deletion);
    std::bernoulli_distribution d_line_insertion(prob_line_insertion);
    
    while (std::getline(isstream, line))
    {
        if (line.find("p cnf") == 0)
        {
            // The line starts with p cnf, therefore we are in the main body of the cnf file 
            in_main_body = true; 

            std::istringstream p_line_stream(line); 
            std::string p_line_token;

            int num_vars; 
            int num_clauses; 
            std::string remainder = ""; 

            p_line_stream >> p_line_token >> p_line_token >> num_vars >> num_clauses;

            //TODO: consider the case of there being things after p cnf * *r

            int new_num_vars = num_vars; 
            int new_num_clauses = num_clauses;          
            
            if (enable_num_vars_change)
            {
                // Distribution with numvars +- numvars/2 to keep the new number approximate
                std::uniform_int_distribution<int> d_num_vars(num_vars - num_vars/2, num_vars + num_vars/2);
                new_num_vars = d_num_vars(generator);
            }

            if (enable_num_clauses_change)
            {
                // Distribution with numclauses +- numclauses/2 to keep the new number approximate
                std::uniform_int_distribution<int> d_num_clauses(num_clauses - num_clauses/2, num_clauses + num_clauses/2);
                new_num_clauses = d_num_clauses(generator); 
            }

            p_line_string = "p cnf " + std::to_string(new_num_vars) + " " + std::to_string(new_num_clauses) + "\n"; 
            continue; 
        }

        if (in_main_body)
        {
            // Record variables in line 
            std::istringstream  line_stream(line);
            std::string         line_token;  

            std::vector<std::string> line_variables;
            std::vector<std::string> line_signs;  

            while(line_stream >> line_token)
            {   
                // Delete EOF if enabled and triggered 
                if (line_token == "0" && enable_EOL_deletion)
                {
                    if (d_EOL_deletion(generator)) {continue; }
                }

                if (line_token.find("-") == 0)
                {
                    line_token.erase(0,1); 
                    line_signs.push_back("-"); 
                }
                else 
                {
                    line_signs.push_back(""); 
                }
                
                line_variables.push_back(line_token); 
                if (line_token != "0") {variable_list.insert(line_token); }
            }

            // Calculate average line length 
            avg_line_length = (avg_line_length + line_variables.size()) / 2; 

            // Skip line if line deletion triggers
            if (enable_line_deletion)
            {
                if (d_line_deletion(generator)) {continue; }
            }

            // Insert line if line insertion triggers 
            if (enable_line_insertion)
            {
                if (d_line_insertion(generator))
                {
                    main_body_string += generate_line(variable_list, avg_line_length, generator) + "\n"; 
                }
            }

            std::string reassembled_line; 

            // Reassemble line with variable insertion / deletion 
            for (int i = 0; i < line_variables.size(); i++)
            {   
                // Skip variable if enabled and triggered
                if (enable_variable_deletion && line_variables[i] != "0" && line_variables.size() > 4)
                {
                    if (d_variable_deletion(generator)) {continue; }
                }

                // Flip sign if enabled and triggered (skipping zeros)
                if (enable_sign_flip && line_variables[i] != "0")
                {
                    if (d_sign_flip(generator))
                    {
                        reassembled_line += line_signs[i] == "-" ? "" : "-"; 
                    }
                    else 
                    {
                        reassembled_line += line_signs[i]; 
                    }
                } 
                else 
                {
                    reassembled_line += line_signs[i]; 
                }

                reassembled_line += line_variables[i] + " "; 

                // Insert a recorded variable if enabled and triggered (skipping zeros)
                if (enable_variable_insertion && i < line_variables.size() - 1)
                {
                    if (d_variable_insertion(generator))
                    {
                        reassembled_line += generate_variable(variable_list, generator) + " "; 
                    }
                }

                if (enable_EOL_insertion)
                {
                    if (d_EOL_insertion(generator))
                    {
                        reassembled_line += "0 "; 
                    }
                }
            }
            // Remove trailing space for parsing 
            reassembled_line.erase(reassembled_line.size()-1, reassembled_line.size()); 

            main_body_string += reassembled_line + "\n"; 
        }
        else 
        {
            prefix_string += line + "\n"; 
        }
    }

    if (enable_correct_pline)
    {
        auto num_lines = std::count(main_body_string.begin(), main_body_string.end(), '\n');
        num_lines += (!main_body_string.empty() && main_body_string.back() != '\n');
        p_line_string = "p cnf " + std::to_string(variable_list.size()) + " " + std::to_string(num_lines) + "\n"; 
    }

    // Distribution for chunk manipulation 
    std::uniform_int_distribution<long> d_rand_location_1(0, main_body_string.size() - 2);

    // Precedence of deletion / rearrange is debatable 
    // Delete an arbitrary chunk of the cnf main body if enabled and triggered 
    if (enable_chunk_deletion)
    {
        for (int i = 0; i < chunk_deletion_times; i++)
        {
            long location_1 = d_rand_location_1(generator);
            std::uniform_int_distribution<long> d_rand_location_2(location_1, main_body_string.size() - 1);
            long location_2 = d_rand_location_2(generator); 
            main_body_string.erase(location_1, location_2 - location_1); 
        }
    }

    // Rearrange an arbitrary chunk of the cnf main body if enabled and triggered 
    if (enable_chunk_rearrange)
    {
        for (int i = 0; i < chunk_rearrange_times; i++)
        {
            long location_1 = d_rand_location_1(generator); 
            std::uniform_int_distribution<long> d_rand_location_2(location_1, main_body_string.size() - 1);
            long location_2 = d_rand_location_2(generator); 
            std::string selected_chunk = std::string(&main_body_string[location_1], &main_body_string[location_2]); 
            main_body_string.erase(location_1, location_2 - location_1); 
            std::uniform_int_distribution<long> d_rand_location(0, main_body_string.size() - 1); 
            long insertion_site = d_rand_location(generator); 
            main_body_string.insert(insertion_site, selected_chunk); 
        }
    }

    return prefix_string + p_line_string + main_body_string; 
}
