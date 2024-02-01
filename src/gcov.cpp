#include "gcov.hpp"

#include <cstdint>
#include <cstdlib>
#include <optional>
#include <stdint.h>
#include <stdio.h>
#include <map>

std::optional<void*> read_bytes(FILE* fd, void* buf, uint32_t count){
	uint32_t read = fread(buf, count, 1, fd);
	if (read != 1){
		return {};
	}
	
	return buf;
}

std::optional<uint32_t> read_uint32(FILE* fd) {
	uint32_t buf [1];
	std::optional<void*> res = read_bytes(fd, &buf, 4); 

	if (res.has_value()){
		return *(uint32_t*)res.value();
	} else {
		return {};
	}
}

std::optional<std::string> read_string(FILE* fd) {

	auto length = read_uint32(fd);
	// printf("Reading string of length: %i\n", length);
	
	if(!length.has_value()){
		return {};
	}
	
	std::string str;
	str.resize((*length) * 4);

	auto res = read_bytes(fd, &str[0], (*length) * 4);
	if (res.has_value()){
		
		// printf("Read string: %s\n", str.c_str());
		return str;
	} else {
		return {};
	}

}

int read_notes_file(std::string notes_file_name, std::vector<function_info_t*>* functions, std::map<uint32_t, function_info_t *>* ident_to_fn) {
	
	FILE* notes_fd = fopen(notes_file_name.c_str(), "rb"); 
	if (notes_fd == NULL){
		printf("Could not open data file: %s", notes_file_name.c_str());
	}

	// Gcov checks for endianness before parsing - since this will be built from source
	// I think we can avoid it.

	if(auto magic = read_uint32(notes_fd)) {
		if (*magic != GCOV_NOTE_MAGIC){
			printf("%s is note a gcov note file. Header is %x, expected %x\n", notes_file_name.c_str(), *magic, GCOV_NOTE_MAGIC);
			return 1;
		}
	} else {
		return 1;
	}

	auto version = read_uint32(notes_fd);
	auto stamp = read_uint32(notes_fd);
	// auto checksum = read_uint32(notes_fd);
	uint32_t current_tag = 0;

	if(!version.has_value() || !stamp.has_value() ){
	// if(!version.has_value() || !stamp.has_value() || !checksum.has_value()){
		printf("Could not read version, stamp or checksum\n");
		return 1;
	}

	if(auto work_dir = read_string(notes_fd)){
		;
		// printf("Working directory: %s\n", (*work_dir).c_str());
	} else {
		printf("Could not read working directory.\n");
		return 1;
	}
	
	// read_uint32(notes_fd);
	auto unexecuted_blocks __attribute__((unused))= read_uint32(notes_fd);
	// printf("Unexec blocks: %i\n", unexecuted_blocks);
	
	/*
	The basic block graph file contains the following records
	   	note: unit function-graph*
		unit: header int32:checksum string:source
		function-graph: announce_function basic_blocks {arcs | lines}*
		announce_function: header int32:ident int32:checksum
			string:name string:source int32:lineno
		basic_block: header int32:flags*
		arcs: header int32:block_no arc*
		arc:  int32:dest_block int32:flags
	        lines: header int32:block_no line*
	               int32:0 string:NULL
		line:  int32:line_no | int32:0 string:filename
	*/

	/*
		int32:  byte3 byte2 byte1 byte0 | byte0 byte1 byte2 byte3
		int64:  int32:low int32:high
		string: int32:0 | int32:length char* char:0 padding
		padding: | char:0 | char:0 char:0 | char:0 char:0 char:0
		item: int32 | int64 | string

		The basic format of the files is:

	   	file : int32:magic int32:version int32:stamp int32:checksum string:work_dir int32:unexec record*

		A record has a tag, length and variable amount of data:

		record: header data
		header: int32:tag int32:length
		data: item*
	*/
	
	// Go through every record in the file
	
	function_info_t *fn = NULL;
	while (auto tag = *read_uint32(notes_fd)){
		auto length = read_uint32(notes_fd);
		auto base_pos __attribute__((unused)) = ftell(notes_fd);

		// printf("Tag: %x\n", tag);
		
		if (feof(notes_fd)) {
			// printf("Reached EOF\n");
			break;
		}
		
		if(tag == GCOV_TAG_FUNCTION){
			auto identity = *read_uint32(notes_fd);
			auto lineno_checksum = read_uint32(notes_fd);
			
			auto cfg_checksum = read_uint32(notes_fd); 
			auto function_name = *read_string(notes_fd);

			// printf("Function name: %s\n", function_name.c_str());
			
			auto artificial = read_uint32(notes_fd);
			// TODO: find_source
			auto src_idx = read_string(notes_fd);
			auto start_line = read_uint32(notes_fd);
			auto start_column = read_uint32(notes_fd);
			auto end_line = read_uint32(notes_fd);
			auto end_column = read_uint32(notes_fd);

			fn = new function_info_t;
			functions->push_back(fn);
			(*ident_to_fn)[identity] = fn;


			fn->m_name = function_name;
			fn->ident = identity;
			fn->lineno_checksum = *lineno_checksum;
			fn->cfg_checksum = *cfg_checksum;
			// fn->src = (*src_idx).c_str();
			fn->start_line = *start_line;
			fn->start_column = *start_column;
			fn->end_line = *end_line;
			fn->end_column = *end_column;
			fn->artificial = *artificial;

			if (fn->artificial == true){
				printf("This function is artificial!");
//				return 1;
			}
	
			current_tag = tag;
		} else if(fn && tag == GCOV_TAG_BLOCKS) {
			if (fn->blocks.empty() == true) {
            	fn->blocks.resize(*read_uint32(notes_fd));
			} else {
				// TODO: Demangle function names
				printf("Already seen blocks for %s in file %s\n", fn->m_name.c_str(), notes_file_name.c_str());
				return 1;
			}
		} else if(fn && tag == GCOV_TAG_ARCS) {

			// Data entry describing arcs from a single basic block (src)
			auto src = *read_uint32(notes_fd);
			fn->blocks[src].id = src;
			uint32_t num_dets = GCOV_TAG_ARCS_NUM(*length);
			block_info* src_block = &fn->blocks[src];

			if (src >= fn->blocks.size() || src_block->succ){
				printf("Corrupted file.\n");
				return 1;
			}

			arc_info* arc;
			bool mark_catches = 0;

			// Add all the outgoing destinations
			for (;num_dets > 0; num_dets--){
				auto dest = *read_uint32(notes_fd);
				auto flags = *read_uint32(notes_fd);

				if (dest >= fn->blocks.size ()){
					printf("Dest > block size\n");
					return 1;				
				}

				// Allocate arc
				arc_info* arc = new arc_info;
				fn->arcs.push_back(arc);

				// Setup src and dst connections of arc
				arc->src = src_block;
				arc->dst = &fn->blocks[dest];
				// printf("Dest: %li\n", dest);
				arc->dst->id = dest;

				// Initialize counts

				arc->count = 0;
				arc->count_valid = 0;
				
				arc->on_tree = !!(flags & GCOV_ARC_ON_TREE);
				arc->fake = !!(flags & GCOV_ARC_FAKE);
				arc->fall_through = !!(flags & GCOV_ARC_FALLTHROUGH);

				arc->succ_next = src_block->succ;
				src_block->succ = arc;
				src_block->num_succ++;

				arc->pred_next = fn->blocks[dest].pred;
				fn->blocks[dest].pred = arc;
				fn->blocks[dest].num_pred++;

				if (arc->fake) {
					if (src) {
						/* Exceptional exit from this function, the
						source block must be a call.  */
						fn->blocks[src].is_call_site = 1;
						arc->is_call_non_return = 1;
						mark_catches = 1;
					} else {
						/* Non-local return from a callee of this
						function.  The destination block is a setjmp.  */
						arc->is_nonlocal_return = 1;
						fn->blocks[dest].is_nonlocal_return = 1;
					}
				}

				if (!arc->on_tree) {
					fn->counts.push_back(0);
				}
			}

			if (mark_catches) {
				/* We have a fake exit from this block.  The other
				non-fall through exits must be to catch handlers.
				Mark them as catch arcs.  */

				for (arc = src_block->succ; arc; arc = arc->succ_next){
					if (!arc->fake && !arc->fall_through) {
						arc->is_throw = 1;
						fn->has_catch = 1;
					}
				}
			}
	
		} else if (fn && tag == GCOV_TAG_LINES) {

			// We don't care about the source code, only the basic blocks!

			uint32_t blockno = *read_uint32(notes_fd);
			// block_info *block = &fn->blocks[blockno];
			if (blockno >= fn->blocks.size()) {
				printf("More blocks than expected.\n");
				return 1;
			}

			while (true) {
				auto lineno = *read_uint32(notes_fd);
				// printf("line no %i\n", lineno);

				if (!lineno){
					auto file_name = read_string(notes_fd);
					if (file_name.has_value() == false){
						break;
					} else {
						if (!((*file_name).c_str())) {
							break;
						}
						// printf("file name: %s\n", file_name.value().c_str());
					}
				}
			}

		} else if (current_tag && !GCOV_TAG_IS_SUBTAG(current_tag, tag)) {
			fn = NULL;
			current_tag = 0;
		} else {

			printf("Unrecognised tag!\n");
			return 0;
		}
		// fseek (notes_fd, base_pos, SEEK_SET);		
	}

	fclose(notes_fd);

	if (functions->empty()){
		printf("%s:no functions found\n", notes_file_name.c_str());
	}

	return 0;
}

/* Solve the flow graph. Propagate counts from the instrumented arcs
   to the blocks and the uninstrumented arcs.  */

void solve_flow_graph(function_info_t *fn, std::string notes_file_name) {
	uint32_t ix;
	arc_info *arc;
	int64_t *count_ptr = &fn->counts.front();
	block_info *blk;
	block_info *valid_blocks = NULL;   /* valid, but unpropagated blocks.  */
	block_info *invalid_blocks = NULL; /* invalid, but inferable blocks.  */

	/* The arcs were built in reverse order.  Fix that now.  */
	for (ix = fn->blocks.size(); ix--;) {
		arc_info *arc_p, *arc_n;

		for (arc_p = NULL, arc = fn->blocks[ix].succ; arc; arc_p = arc, arc = arc_n) {
			arc_n = arc->succ_next;
			arc->succ_next = arc_p;
		}
		fn->blocks[ix].succ = arc_p;

		for (arc_p = NULL, arc = fn->blocks[ix].pred; arc; arc_p = arc, arc = arc_n) {
			arc_n = arc->pred_next;
			arc->pred_next = arc_p;
		}
		fn->blocks[ix].pred = arc_p;
	}

	if (fn->blocks.size() < 2){
		printf("%s:'%s' lacks entry and/or exit blocks\n", notes_file_name.c_str(), fn->m_name.c_str());
	} else {
		if (fn->blocks[ENTRY_BLOCK].num_pred)
			printf("%s:'%s' has arcs to entry block\n", notes_file_name.c_str(), fn->m_name.c_str());
		else
			/* We can't deduce the entry block counts from the lack of
			predecessors.  */
			fn->blocks[ENTRY_BLOCK].num_pred = ~(uint32_t)0;

		if (fn->blocks[EXIT_BLOCK].num_succ)
			printf("%s:'%s' has arcs from exit block\n", notes_file_name.c_str(), fn->m_name.c_str());
		else
		/* Likewise, we can't deduce exit block counts from the lack
		of its successors.  */
		fn->blocks[EXIT_BLOCK].num_succ = ~(uint32_t)0;
	}

	/* Propagate the measured counts, this must be done in the same
	order as the code in profile.cc  */
	for (uint32_t i = 0; i < fn->blocks.size(); i++) {
	blk = &fn->blocks[i];
	block_info const *prev_dst = NULL;
	int out_of_order = 0;
	int non_fake_succ = 0;

	for (arc = blk->succ; arc; arc = arc->succ_next) {
		if (!arc->fake)
			non_fake_succ++;

		if (!arc->on_tree) {
			if (count_ptr)
				arc->count = *count_ptr++;
			arc->count_valid = 1;
			blk->num_succ--;
			arc->dst->num_pred--;
		}
		if (prev_dst && prev_dst > arc->dst)
			out_of_order = 1;
		prev_dst = arc->dst;
	}

	if (non_fake_succ == 1) {
		/* If there is only one non-fake exit, it is an
		unconditional branch.  */
		for (arc = blk->succ; arc; arc = arc->succ_next) {
			if (!arc->fake) {
				arc->is_unconditional = 1;
				/* If this block is instrumenting a call, it might be
				an artificial block. It is not artificial if it has
				a non-fallthrough exit, or the destination of this
				arc has more than one entry.  Mark the destination
				block as a return site, if none of those conditions
				hold.  */
				if (blk->is_call_site && arc->fall_through && arc->dst->pred == arc && !arc->pred_next)
					arc->dst->is_call_return = 1;
			}
		}
	}

	/* Sort the successor arcs into ascending dst order. profile.cc
	normally produces arcs in the right order, but sometimes with
	one or two out of order.  We're not using a particularly
	smart sort.  */
	if (out_of_order) {
		arc_info *start = blk->succ;
		uint32_t changes = 1;

		while (changes) {
			arc_info *arc, *arc_p, *arc_n;

			changes = 0;
			for (arc_p = NULL, arc = start; (arc_n = arc->succ_next);) {
				if (arc->dst > arc_n->dst) {
					changes = 1;
					if (arc_p)
						arc_p->succ_next = arc_n;
					else
						start = arc_n;

					arc->succ_next = arc_n->succ_next;
					arc_n->succ_next = arc;
					arc_p = arc_n;
				} else {
					arc_p = arc;
					arc = arc_n;
				}
			}
		}
		blk->succ = start;
	}

	/* Place it on the invalid chain, it will be ignored if that's
	wrong.  */
	blk->invalid_chain = 1;
	blk->chain = invalid_blocks;
	invalid_blocks = blk;
	}

	while (invalid_blocks || valid_blocks) {
		while ((blk = invalid_blocks)) {
			int64_t total = 0;
			const arc_info *arc;

			invalid_blocks = blk->chain;
			blk->invalid_chain = 0;

			if (!blk->num_succ)
				for (arc = blk->succ; arc; arc = arc->succ_next)
					total += arc->count;
			else if (!blk->num_pred)
				for (arc = blk->pred; arc; arc = arc->pred_next)
					total += arc->count;
			else
				continue;

			blk->count = total;
			blk->count_valid = 1;
			blk->chain = valid_blocks;
			blk->valid_chain = 1;
			valid_blocks = blk;
		}
		while ((blk = valid_blocks)) {
			int64_t total;
			arc_info *arc, *inv_arc;

			valid_blocks = blk->chain;
			blk->valid_chain = 0;
			if (blk->num_succ == 1) {
					block_info *dst;

				total = blk->count;
				inv_arc = NULL;
				for (arc = blk->succ; arc; arc = arc->succ_next) {
					total -= arc->count;
				if (!arc->count_valid)
					inv_arc = arc;
				}
				dst = inv_arc->dst;
				inv_arc->count_valid = 1;
				inv_arc->count = total;
				blk->num_succ--;
				dst->num_pred--;
				if (dst->count_valid) {
					if (dst->num_pred == 1 && !dst->valid_chain) {
						dst->chain = valid_blocks;
						dst->valid_chain = 1;
						valid_blocks = dst;
					}
				} else {
					if (!dst->num_pred && !dst->invalid_chain) {
						dst->chain = invalid_blocks;
						dst->invalid_chain = 1;
						invalid_blocks = dst;
					}
				}
			}

			if (blk->num_pred == 1) {
				block_info *src;

				total = blk->count;
				inv_arc = NULL;

				for (arc = blk->pred; arc; arc = arc->pred_next) {
					total -= arc->count;
					if (!arc->count_valid)
						inv_arc = arc;
				}
				src = inv_arc->src;
				inv_arc->count_valid = 1;
				inv_arc->count = total;
				blk->num_pred--;
				src->num_succ--;
				if (src->count_valid) {
					if (src->num_succ == 1 && !src->valid_chain) {
					src->chain = valid_blocks;
					src->valid_chain = 1;
					valid_blocks = src;
					}
				} else {
					if (!src->num_succ && !src->invalid_chain) {
					src->chain = invalid_blocks;
					src->invalid_chain = 1;
					invalid_blocks = src;
					}
				}
			}
		}
	}

	/* If the graph has been correctly solved, every block will have a
	valid count.  */
	for (uint32_t i = 0; ix < fn->blocks.size(); i++)
	if (!fn->blocks[i].count_valid) {
		printf("%s:graph is unsolvable\n", fn->m_name.c_str());
		break;
	}
}



int read_count_file(std::string count_file_name, std::map<uint32_t, function_info_t *>* ident_to_fn) {
	uint32_t ix;

	FILE* count_fd = fopen(count_file_name.c_str(), "rb");
	if (!count_fd) {
		printf("%s:cannot open data file, assuming not executed\n",count_file_name.c_str());
		return 0;
	}

	if(auto magic = read_uint32(count_fd)) {
		if (*magic != GCOV_DATA_MAGIC){
			printf("%s is note a gcov note file. Header is %x, expected %x\n", count_file_name.c_str(), *magic, GCOV_NOTE_MAGIC);
			return 1;
		}
	} else {
		return 1;
	}

	auto version __attribute__((unused)) = *read_uint32(count_fd);
	auto tag = *read_uint32(count_fd);
	// if (tag != bbg_stamp) {
	// 	fnotice(stderr, "%s:stamp mismatch with notes file\n", count_file_name);
	// 	return_code = 5;
	// 	goto cleanup;
	// }

	/* Read checksum.  */
	// read_uint32(count_fd);

	function_info_t *fn = NULL;
	std::map<uint32_t, function_info_t *>::iterator it;

	while ((tag = *read_uint32(count_fd))) {
		auto length = *read_uint32(count_fd);
		int read_length = (int)length;
		auto base __attribute__((unused)) = ftell(count_fd);

		// printf("Tag: %x\n", tag);

		if (feof(count_fd)) {
			// printf("Reached EOF\n");
			break;
		}

		if (tag == GCOV_TAG_OBJECT_SUMMARY) {
			gcov_summary summary __attribute__((unused));
			summary.runs = *read_uint32(count_fd);
			summary.sum_max = *read_uint32(count_fd);
			// object_runs = summary.runs;
		} else if (tag == GCOV_TAG_FUNCTION && !length){
			// printf("Function tag no length\n");
			; /* placeholder  */
		}
		else if (tag == GCOV_TAG_FUNCTION && length == GCOV_TAG_FUNCTION_LENGTH) {
			uint32_t ident = *read_uint32(count_fd);
			fn = NULL;
			it = ident_to_fn->find(ident);
			if (it != ident_to_fn->end())
				fn = it->second;

			if (!fn)
				;
			else if (*read_uint32(count_fd) != fn->lineno_checksum || *read_uint32(count_fd) != fn->cfg_checksum) {
				printf("%s:profile mismatch for '%s'\n", count_file_name.c_str(), fn->m_name.c_str());
				return 1;
			}
		} else if (tag == GCOV_TAG_FOR_COUNTER(GCOV_COUNTER_ARCS) && fn) {
			length = abs(read_length);
			
			if (length != GCOV_TAG_COUNTER_LENGTH(fn->counts.size())){
				printf("Length does not match\n");
				return 1;
			}

			if (read_length > 0){
				for (ix = 0; ix != fn->counts.size(); ix++){

					int64_t value;
					uint32_t allocated_buffer[2];
					uint32_t *buffer = (uint32_t*)*read_bytes(count_fd, &allocated_buffer, 2 * 4);

					if (!buffer)
						value = 0;

					value = buffer[0];
					// if (sizeof (value) > sizeof (gcov_uint32_t_t))
					value |= ((int64_t) buffer[1]) << 32;
					// else if (buffer[1])
						// gcov_var.error = GCOV_FILE_COUNTER_OVERFLOW;


					fn->counts[ix] += value;
					// printf("Read counts %lu \n", fn->counts[ix]);
				}
			}

			if (read_length < 0)
				read_length = 0;
			
			// gcov_sync(base, read_length);
			// if ((error = gcov_is_error())) {
			// 	fnotice(stderr, error < 0 ? N_("%s:overflowed\n") : N_("%s:corrupted\n"), count_file_name);
			// 	return_code = 4;
			// 	goto cleanup;
			// }
		} else {
			printf("Unrecognised tag!\n");
			return 1;
		}
	}

	fclose(count_fd);
	return 0;
}
