#include "generate_sat.hpp"

std::string generate_sat(int num_vars = 10, int num_clauses = 20, int max_clauses = 20)
{
    std::random_device rd;
    std::mt19937 generator(rd());
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
            std::cout << curr_var << std::endl;
            cnf_output += bool_vars[curr_var] ? std::to_string(curr_var + 1) : "-" + std::to_string(curr_var + 1); 
            cnf_output += " "; 

            // Break randomly to create random clause lengths
            if (d_max_clauses(generator) < j)
            {
                break; 
            }
        }

        cnf_output += " 0\n";
    }

    return cnf_output;
}

std::string generate_unsat_pigionhole(int pigions, int holes)
{
    // The idea behind this algorith to generate UNSATs is that there are more 
    // pigions than there are holes, we express that each pigion must be in a 
    // hole and that no two pigions should be in the same hole. 

    // Complexity: O(n^3) (Assuming that pigions = holes + 1)
    // Variables: pigions * holes 
    // Clauses: pigions + holes * pigion * (pigion - 1) / 2

    // Return standard UNSAT case if inputs are incorrect 
    if (pigions <= holes) { return "c error \n p cnf 1 2 \n -1 0\n 1 0"; }

    std::string cnf_output = ""; 
    int num_clauses = 0; // Accumulating to save time on division / multiplication

    // Express that each pigion should be in a hole 
    for (int pigion = 0; pigion < pigions; pigion++)
    {
        for (int hole = 0; hole < holes; hole++)
        {
            cnf_output += std::to_string((pigion) * holes + hole + 1); 
            cnf_output += " "; 
        }
        cnf_output += " 0\n";
        num_clauses++; 
    }

    // Express that two pigions cannot be in the same hole 
    for (int hole = 0; hole < holes; hole++)
    {
        for (int pigion1 = 0; pigion1 < pigions; pigion1++)
        {
            for (int pigion2 = pigion1 + 1; pigion2 < pigions; pigion2++)
            {
                cnf_output += "-" + std::to_string((pigion1) * holes + hole + 1); 
                cnf_output += " "; 
                cnf_output += "-" + std::to_string((pigion2) * holes + hole + 1); 
                cnf_output += "  0\n"; 
                num_clauses++; 
            }
        }
    }

    // Generate final cnf file 
    return "c pigionhole \np cnf " + std::to_string(pigions * holes) + " " + std::to_string(num_clauses) + "\n" + cnf_output; 

}


int main ()
{
    std::cout << generate_unsat_pigionhole(8, 3) << std::endl;   
}
