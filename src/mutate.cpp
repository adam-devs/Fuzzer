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

std::string chunk_deletion(std::mt19937 generator, std::string &input_string)
{
    // Get deletion size 
    std::uniform_int_distribution<size_t> d_chunk_size(0, input_string.size() - 2); 
    size_t chunk_size = d_chunk_size(generator); 

    // Get deletion site
    std::uniform_int_distribution<size_t> d_deletion_site(0, input_string.size() - chunk_size - 1); 
    size_t deletion_site = d_deletion_site(generator); 

    std::string remaining_string = input_string.substr(deletion_site, chunk_size); 

    input_string.erase(deletion_site, chunk_size); 

    return remaining_string; 
}

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
    unsigned int seed              )
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
    std::bernoulli_distribution d_EOL_insertion(prob_EOL_insertion); 
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
            // Detecting junk
            if (line == "" || line == " ")
            {
                continue; 
            }

            static const char junk_characters[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "!@#$%^&*()_+-=;,./<>?|{}:";
            
            // At this stage, our generated code has fooled our fuzzer into 
            // believing that it is a real cnf file... 
            size_t found = line.find_first_of(junk_characters); 

            if (found != std::string::npos) 
            {
                // Give back the junk that we are fed without parsing it 
                return cnf_input; 
            }
            
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
            for (long unsigned int i = 0; i < line_variables.size(); i++)
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
            reassembled_line.erase(reassembled_line.size(), 1); 

            main_body_string += reassembled_line + "\n"; 
        }
        else 
        {
            prefix_string += line + "\n"; 
        }
    }

    if (enable_correct_pline)
    {
        // Find the largest integer in variable_list 

        int largest_var = 1; 
        for (const auto& str : variable_list) {
            try 
            {
                // Convert string to integer 
                int variable = std::stoi(str); 
                // Find Maximum
                largest_var = std::max(largest_var, variable); 
            } 
            // Ignore errors and continue 
            catch (const std::invalid_argument& ia) 
            {
                continue;
            } 
            catch (const std::out_of_range& oor) 
            {
                continue;
            }
        }
        auto num_lines = std::count(main_body_string.begin(), main_body_string.end(), '\n');
        num_lines += (!main_body_string.empty() && main_body_string.back() != '\n');
        p_line_string = "p cnf " + std::to_string(largest_var) + " " + std::to_string(num_lines) + "\n"; 
    }

    // Distribution for chunk manipulation 
    std::string *operating_string; 
    if (main_body_string.size() == 0)
    {
        operating_string = &prefix_string; 
    }
    else
    {
        operating_string = &main_body_string; 
    }

    // Precedence of deletion / rearrange is debatable 
    // Delete an arbitrary chunk of the cnf main body if enabled and triggered 
    if (enable_chunk_deletion)
    {
        for (int i = 0; i < chunk_deletion_times; i++)
        {
            chunk_deletion(generator, *operating_string); 
        }
    }

    // Rearrange an arbitrary chunk of the cnf main body if enabled and triggered 
    if (enable_chunk_rearrange)
    {
        for (int i = 0; i < chunk_rearrange_times; i++)
        {
            std::string remaining_string = chunk_deletion(generator, *operating_string); 
            
            std::uniform_int_distribution<size_t> d_injection_site(0, operating_string->size() - 1); 
            size_t injection_site = d_injection_site(generator); 

            operating_string->insert(injection_site, remaining_string); 
        }
    }

    return prefix_string + p_line_string + main_body_string; 
}
