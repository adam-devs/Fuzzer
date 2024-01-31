#include <cstdint>
#include <optional>
#include <stdint.h>
#include <stdio.h>
#include <map>
#include <algorithm>
#include <filesystem>
namespace fs = std::filesystem;

#include "coverage.hpp"
#include "gcov.hpp"

std::optional<coverage> arc_coverage_all_files(std::string directory, bool debug){
	
	std::string notes_file_name;
	std::string count_file_name;

	coverage coverage = {};

	std::vector<std::filesystem::path> dir_files; 
	
	for (auto dir_entry : fs::directory_iterator(directory)) {		
		if (dir_entry.is_regular_file()){
			dir_files.push_back(dir_entry);
		}	
	}
	std::sort(dir_files.begin(), dir_files.end());

	
	for(auto dir_file : dir_files){
		// printf("Dir file: %s\n", dir_file.filename().c_str());
		
		if (dir_file.extension() == ".gcda") {
			count_file_name = dir_file;
		} 

		if (dir_file.extension() == ".gcno") {
			notes_file_name = dir_file;
		} 

		if(notes_file_name.empty() || count_file_name.empty()){
			continue;
		}

		if (debug)
			printf("\n------------- Reading contents of %s and %s -------------\n", notes_file_name.c_str(), count_file_name.c_str());
		
		std::vector<function_info_t*> functions;
		std::map<uint32_t, function_info_t *> ident_to_fn;

		if (debug)
			printf("Reading notes file... \n");
		if (read_notes_file(notes_file_name, &functions, &ident_to_fn) ) {
			return {};
		}

		if (debug)
			printf("\nReading count file... \n");
		if (read_count_file(count_file_name, &ident_to_fn)) {
			printf("Error when reading count file\n");
			return {};
		}

		for (auto it = functions.begin(); it != functions.end(); it++) {
			solve_flow_graph(*it, notes_file_name);
		}

		uint32_t function_arc_count = 0;
		uint32_t function_arcs_executed = 0;
		for (auto it = functions.begin(); it != functions.end(); it++) {
			function_info_t* func = *it;
			auto blocks = func->blocks;

			if (debug){
				printf("\nFunction name: %s\n", func->m_name.c_str());
				printf("Number of blocks: %lu\n", func->blocks.size());
			}

			for (auto bit = blocks.begin(); bit != blocks.end(); bit++) {
				block_info block = *bit;
				// printf("Block id: %u, counnt : %lu, is_return: %u, is_call: %u\n", block.id, block.count, block.is_call_return, block.is_call_site);
			}

			std::vector<int64_t> arc_counts = func->counts;
			uint32_t arcs_executed = 0;
			for(auto count : arc_counts){
				if (count > 0) {
					arcs_executed += 1;
					coverage.arc_coverage.push_back(true);
				} else {
					coverage.arc_coverage.push_back(false);
				}
			}

			coverage.functions += 1;
			if (arcs_executed > 1){
				coverage.functions_executed += 1;
				coverage.function_coverage.push_back(true);
			} else {
				coverage.function_coverage.push_back(false);
			}

			if (debug)
				printf("Arcs executed: %i/%lu\n", arcs_executed, arc_counts.size());

			function_arcs_executed += arcs_executed;
			function_arc_count += arc_counts.size();
		}

		if (debug)
			printf("Function arcs executed: %i/%i\n", function_arcs_executed, function_arc_count);
		
		coverage.arcs_executed += function_arcs_executed;
		coverage.arcs += function_arc_count;

		for (auto func : functions) {		
			for (auto arc : func->arcs){
				delete arc;
			}
			delete func;
		}

		notes_file_name.clear();
		count_file_name.clear();
	}

	return coverage;
}

std::optional<coverage_diff> calc_coverage_diff(coverage* prev, coverage* cur){
	
	if (prev->arc_coverage.size() != cur->arc_coverage.size()){
		return {};
	} else if (prev->function_coverage.size() != cur->function_coverage.size()) {
		return {};
	}
	
	coverage_diff diff = {};

	diff.arcs_executed_change = cur->arcs_executed - prev->arcs_executed;
	diff.functions_executed_change = cur->functions_executed - prev->functions_executed;
	
	for (size_t i = 0; i < prev->arc_coverage.size(); i++) {
    	if (prev->arc_coverage[i] == false && cur->arc_coverage[i] == true){
			diff.new_unique_arcs_executed++;
		} else if (prev->arc_coverage[i] == cur->arc_coverage[i]){
			diff.arcs_executed_overlap++;
		}
    }

	for (size_t i = 0; i < prev->function_coverage.size(); i++) {
    	if (prev->function_coverage[i] == false && cur->function_coverage[i] == true){
			diff.new_unique_funcs_executed++;
		} else if (prev->function_coverage[i] == cur->function_coverage[i]){
			diff.funcs_executed_overlap++;
		}
    }
	return diff;
}


std::optional<coverage*> aggregrate_coverage(coverage* aggregate, coverage* cur){
	
	if (aggregate->arc_coverage.size() != cur->arc_coverage.size() || aggregate->function_coverage.size() != cur->function_coverage.size()){
		printf("Aggregation failed: The current coverage and aggregate coverage do not have matching dimensions.");
		return {};
	}
		
	for (size_t i = 0; i < aggregate->arc_coverage.size(); i++) {
    	if (aggregate->arc_coverage[i] == false && cur->arc_coverage[i] == true){
			aggregate->arcs_executed++;
		}
		aggregate->arc_coverage[i] = cur->arc_coverage[i] | aggregate->arc_coverage[i];
	}

	for (size_t i = 0; i < aggregate->function_coverage.size(); i++) {
    	if (aggregate->function_coverage[i] == false && cur->function_coverage[i] == true){
			aggregate->functions_executed++;
		}
		aggregate->function_coverage[i] = cur->function_coverage[i] | aggregate->function_coverage[i];
    }

	return aggregate;
}


void print_coverage_info(coverage* coverage){
	printf("Covered %i/%i unique arcs.\n", coverage->arcs_executed, coverage->arcs);
	printf("Covered %i/%i unique functions.\n", coverage->functions_executed, coverage->functions);
}
