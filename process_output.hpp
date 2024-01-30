// https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html
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
