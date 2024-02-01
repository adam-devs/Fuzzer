#include <string>


// Error strings that UB sanitizer will output for the
// various types of UB

#define OUTPUT_NULL_PTR "null pointer"

#define OUTPUT_SIGNED_INTEGER_OVERFLOW "signed integer overflow"

#define OUTPUT_CANNOT_REPRESENT "cannot be represented in type"
#define OUTPUT_STACK_OVERFLOW "stack-overflow"
#define OUTPUT_VLA_BOUND "variable length array"
#define OUTPUT_HEAP_BUFFER_OVERFLOW "heap-buffer-overflow"
#define OUTPUT_WILD_POINTER "wild pointer"
#define MISALIGNED_ADDRESS "misaligned address"
#define SEG_FAULT "SEGV on unknown address"
#define SANITIZER "Sanitizer"

#define SAT "SAT"
#define UNSAT "UNSAT"

// https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html
// Enumeration of all possible UBs detected by UB sanitizer
enum undefined_behaviour_t {
	ub_start,

	cannot_represent,
	stack_overflow, // Use of misaligned pointer
	alignment, // Use of misaligned pointer
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
	wild_pointer,

	seg_fault,
	error,
	uncategorized,
	no_error,
	placeholder,

	ub_end,
};

// struct process_result {
// 	undefined_behaviour_t ub_type;
// 	std::string err_str;	
// };

undefined_behaviour_t process_output(std::string output);

