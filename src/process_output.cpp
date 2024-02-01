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
	} else if (std::string::npos != output.find(OUTPUT_WILD_POINTER)){
	  return wild_pointer;
  } else if (std::string::npos != output.find(MISALIGNED_ADDRESS)) {
	  return alignment;
 	} else if (std::string::npos != output.find(SEG_FAULT)){
    return seg_fault;
  } else if (std::string::npos != output.find(SANITIZER)){
    return error;
  } else if (std::string::npos != output.find(SAT)){
	  return no_error;
  } else if (std::string::npos != output.find(UNSAT)){
	  return no_error;  
  } else {
		return uncategorized;
	}	
}
