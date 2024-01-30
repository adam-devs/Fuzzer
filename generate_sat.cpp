#include "generate_sat.hpp"

// Generate a cnf string (seeded random) that has no guarantee on SAT
std::string generate_cnf(int num_vars = 10, int num_clauses = 20, int max_clauses = 20, unsigned int seed = 123456)
{
    std::random_device rd;
    std::mt19937 generator(rd());
    generator.seed(seed); 

    // Create base cnf prefix
    std::string cnf_output = "p cnf " + std::to_string(num_vars) + " " + std::to_string(num_clauses) + "\n"; 

    // Create distribution for choosing a random boolean values
    std::bernoulli_distribution d_bool(0.5);

    // Create distribution for choosing variables 
    std::uniform_int_distribution<int> d_vars(0, num_vars - 1);

    // Create distribution for breaking the for loop 
    std::uniform_int_distribution<int> d_max_clauses(0, max_clauses - 1);

    for (int i = 0; i < num_clauses; i++)
    {
        for (int j = 0; j < max_clauses; j++)
        {
            int curr_var = d_vars(generator); 
            cnf_output += d_bool(generator) ? std::to_string(curr_var + 1) : "-" + std::to_string(curr_var + 1); 
            cnf_output += " "; 

            // Break randomly to create random clause lengths
            if (d_max_clauses(generator) < j)
            {
                break; 
            }
        }

        cnf_output += "0\n";
    }

    return cnf_output;
}

// Generate a cnf string (seeded random) that guarenteed SAT
std::string generate_sat(int num_vars = 10, int num_clauses = 20, int max_clauses = 20, unsigned int seed = 123456)
{
    std::random_device rd;
    std::mt19937 generator(rd());
    generator.seed(seed); 

    // Initialise vector for generating variables 
    std::vector<bool> bool_vars(num_vars); 

    // Create base cnf prefix
    std::string cnf_output = "p cnf " + std::to_string(num_vars) + " " + std::to_string(num_clauses) + "\n"; 

    // Create distribution for choosing a random boolean values
    std::bernoulli_distribution d_bool(0.5);

    for (int i = 0; i < num_vars; i++)
    {
        bool_vars[i] = d_bool(generator); 
    }

    // Create distribution for choosing variables 
    std::uniform_int_distribution<int> d_vars(0, num_vars - 1);

    // Create distribution for breaking the for loop 
    std::uniform_int_distribution<int> d_max_clauses(0, max_clauses - 1);

    for (int i = 0; i < num_clauses; i++)
    {
        for (int j = 0; j < max_clauses; j++)
        {
            int curr_var = d_vars(generator); 
            cnf_output += bool_vars[curr_var] ? std::to_string(curr_var + 1) : "-" + std::to_string(curr_var + 1); 
            cnf_output += " "; 

            // Break randomly to create random clause lengths
            if (d_max_clauses(generator) < j)
            {
                break; 
            }
        }

        cnf_output += "0\n";
    }

    return cnf_output;
}

// Generate a cnf string (deterministic) that is guarenteed UNSAT by enumerating all possible conditions
std::string generate_unsat_combination(int num_vars = 3)
{   
    // The idea behind this algorithm is to generate every combination of negation 
    // pattern for the number of variables. 

    // Complexity: O(2^n)
    // Variables: num_vars
    // Clauses: 2^n 

    // Return immediately with 1 variable
    if (num_vars == 1) 
    {
        return "c 1 variable combination\np cnf 1 2\n1 0\n-1 0\n"; 
    }

    int num_clauses = std::pow(2, num_vars);
    std::string cnf_output = "c " + std::to_string(num_vars) + " variable combination\n"; 
    cnf_output += "p cnf " + std::to_string(num_vars) + " " + std::to_string(num_clauses) + "\n"; 

    for (int i = 0; i < num_clauses; i++) 
    {
        for (int j = 0; j < num_vars; j++) 
        {

            // Check if jth bit of i is set
            if ((i >> j) & 1) 
            {
                cnf_output += std::to_string(j + 1) + " "; // True for set bits 
            } 
            else 
            {
                cnf_output += "-" + std::to_string(j + 1) + " "; // False for non-set bits 
            }
        }
        cnf_output += "0\n"; 
    }

    return cnf_output; 
}

// Generate a cnf string (deterministic) that is guarenteed UNSAT by expressing the pigeon hole problem 
std::string generate_unsat_pigeonhole(int pigeons, int holes)
{
    // The idea behind this algorith to generate UNSATs is that there are more 
    // pigeons than there are holes, we express that each pigeon must be in a 
    // hole and that no two pigeons should be in the same hole. 

    // Complexity: O(n^3) (Assuming that pigeons = holes + 1)
    // Variables: pigeons * holes 
    // Clauses: pigeons + holes * pigeon * (pigeon - 1) / 2

    // Return standard UNSAT case if inputs are incorrect 
    if (pigeons <= holes) { return "c error \n p cnf 1 2 \n -1 0\n 1 0"; }

    std::string cnf_output = ""; 
    int num_clauses = 0; // Accumulating to save time on division / multiplication

    // Express that each pigeon should be in a hole 
    for (int pigeon = 0; pigeon < pigeons; pigeon++)
    {
        for (int hole = 0; hole < holes; hole++)
        {
            cnf_output += std::to_string((pigeon) * holes + hole + 1); 
            cnf_output += " "; 
        }
        cnf_output += " 0\n";
        num_clauses++; 
    }

    // Express that two pigeons cannot be in the same hole 
    for (int hole = 0; hole < holes; hole++)
    {
        for (int pigeon1 = 0; pigeon1 < pigeons; pigeon1++)
        {
            for (int pigeon2 = pigeon1 + 1; pigeon2 < pigeons; pigeon2++)
            {
                cnf_output += "-" + std::to_string((pigeon1) * holes + hole + 1); 
                cnf_output += " "; 
                cnf_output += "-" + std::to_string((pigeon2) * holes + hole + 1); 
                cnf_output += "  0\n"; 
                num_clauses++; 
            }
        }
    }

    // Generate final cnf file 
    return "c pigeonhole \np cnf " + std::to_string(pigeons * holes) + " " + std::to_string(num_clauses) + "\n" + cnf_output; 

}
