#include <string>

#include "process_output.hpp"



undefined_behaviour_t process_output(std::string output) {
	if (std::string::npos != output.find(OUTPUT_NULL_PTR)){
		return null_ptr;	
	} else if (std::string::npos != output.find(OUTPUT_SIGNED_INTEGER_OVERFLOW)){
		return signed_overflow;	
	} else if (std::string::npos != output.find(OUTPUT_VLA_BOUND)){
		return invalid_vla_bound;
	} else if (std::string::npos != output.find(OUTPUT_HEAP_BUFFER_OVERFLOW)){
		return heap_buffer_overflow;
	} else {
		return uncategorized;
	}	
}
