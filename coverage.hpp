#ifndef COVERAGE_HPP
#define COVERAGE_HPP

#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include <stdint.h>

typedef struct {
	uint32_t arcs;
	uint32_t arcs_executed;
	std::vector<bool> arc_coverage;

	uint32_t functions;
	uint32_t functions_executed;
	std::vector<bool> function_coverage;
} coverage;

typedef struct {
	int32_t arcs_executed_change;
	uint32_t arcs_executed_overlap;
	uint32_t new_unique_arcs_executed;
	
	int32_t functions_executed_change;
	uint32_t funcs_executed_overlap;
	uint32_t new_unique_funcs_executed;
} coverage_diff;


std::optional<coverage*> aggregrate_coverage(coverage* aggregate, coverage* cur);
std::optional<coverage_diff> calc_coverage_diff(coverage* prev, coverage* cur);
std::optional<coverage> arc_coverage_all_files(std::string directory, bool debug);
void print_coverage_info(coverage* coverage);

#endif
