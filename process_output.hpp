#include <string>


// Error strings that UB sanitizer will output for the
// various types of UB
#define OUTPUT_VLA_BOUND "variable length array"

#define OUTPUT_NULL_PTR "null pointer"

#define OUTPUT_HEAP_BUFFER_OVERFLOW "heap-buffer-overflow"

// https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html
// Enumeration of all possible UBs detected by UB sanitizer
enum undefined_behaviour_t {
	ub_start,

	aligment, // Use of misaligned pointer
	bool_load, // Load of bool which is neither true or false.
	enum_load, // Load of enum value which is not representable in that enum type
	float_cast_overflow,
	function_ptr, // Indirect call of a function through a function pointer of wrong type.
	div_by_zero,
	null_ptr,
	pointer_overflow,
	shift, 
	signed_overflow,
	unreachable,
	invalid_vla_bound,
	heap_buffer_overflow,

	uncategorized,
};

// struct process_result {
// 	undefined_behaviour_t ub_type;
// 	std::string err_str;	
// };
