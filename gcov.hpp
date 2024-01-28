
#ifndef GCOV_HPP
#define GCOV_HPP

/*
	This header file and its associated implementation are taken mostly from
	the source code of the gcov command-line tool, but with a few modifications
	to make it work within the confines of our codebase.

	We decided this approach was favourable to simply using the gcov tool,
	as we are able to introspect the arcs/edges taken, rather than just
	the coverage of the basic blocks.
*/


#include <string>
#include <cstdint>
#include <stdint.h>
#include <map>
#include <vector>

#define GCOV_NOTE_MAGIC 0x67636e6f // note/graph
#define GCOV_DATA_MAGIC 0x67636461 // data/count files
#define GCOV_WORD_SIZE 4

// Taken from the gcov source
#define GCOV_TAG_FUNCTION	 (0x01000000)
#define GCOV_TAG_FUNCTION_LENGTH (3 * GCOV_WORD_SIZE)
#define GCOV_TAG_BLOCKS		 (0x01410000)
#define GCOV_TAG_BLOCKS_LENGTH(NUM) (NUM)
#define GCOV_TAG_ARCS		 (0x01430000)
#define GCOV_TAG_ARCS_LENGTH(NUM)  (1 + (NUM) * 2 * GCOV_WORD_SIZE)
#define GCOV_TAG_ARCS_NUM(LENGTH)  (((LENGTH / GCOV_WORD_SIZE) - 1) / 2)
#define GCOV_TAG_LINES		 (0x01450000)
#define GCOV_TAG_COUNTER_BASE 	 (0x01a10000)
#define GCOV_TAG_COUNTER_LENGTH(NUM) ((NUM) * 2 * GCOV_WORD_SIZE)
#define GCOV_TAG_COUNTER_NUM(LENGTH) ((LENGTH / GCOV_WORD_SIZE) / 2)
#define GCOV_TAG_OBJECT_SUMMARY  (0xa1000000)
#define GCOV_TAG_OBJECT_SUMMARY_LENGTH (2 * GCOV_WORD_SIZE)
#define GCOV_TAG_PROGRAM_SUMMARY (0xa3000000) /* Obsolete */
#define GCOV_TAG_AFDO_FILE_NAMES (0xaa000000)
#define GCOV_TAG_AFDO_FUNCTION (0xac000000)
#define GCOV_TAG_AFDO_WORKING_SET (0xaf000000)

#define GCOV_COUNTER_ARCS       0  /* Arc transitions.  */

/* Convert a counter index to a tag.  */
#define GCOV_TAG_FOR_COUNTER(COUNT)				\
	(GCOV_TAG_COUNTER_BASE + ((uint32_t)(COUNT) << 17))
/* Convert a tag to a counter.  */
#define GCOV_COUNTER_FOR_TAG(TAG)					\
	((unsigned)(((TAG) - GCOV_TAG_COUNTER_BASE) >> 17))
/* Check whether a tag is a counter tag.  */
#define GCOV_TAG_IS_COUNTER(TAG)				\
	(!((TAG) & 0xFFFF) && GCOV_COUNTER_FOR_TAG (TAG) < GCOV_COUNTERS)

/* The tag level mask has 1's in the position of the inner levels, &
   the lsb of the current level, and zero on the current and outer
   levels.  */
#define GCOV_TAG_MASK(TAG) (((TAG) - 1) ^ (TAG))

/* Return nonzero if SUB is an immediate subtag of TAG.  */
#define GCOV_TAG_IS_SUBTAG(TAG,SUB)				\
	(GCOV_TAG_MASK (TAG) >> 8 == GCOV_TAG_MASK (SUB) 	\
	 && !(((SUB) ^ (TAG)) & ~GCOV_TAG_MASK (TAG)))

/* Return nonzero if SUB is at a sublevel to TAG.  */
#define GCOV_TAG_IS_SUBLEVEL(TAG,SUB)				\
     	(GCOV_TAG_MASK (TAG) > GCOV_TAG_MASK (SUB))

/* Basic block flags.  */
#define GCOV_BLOCK_UNEXPECTED	(1 << 1)

/* Arc flags.  */
#define GCOV_ARC_ON_TREE 	(1 << 0)
#define GCOV_ARC_FAKE		(1 << 1)
#define GCOV_ARC_FALLTHROUGH	(1 << 2)

struct arc_info {
  /* source and destination blocks.  */
  struct block_info *src;
  struct block_info *dst;

  /* transition counts.  */
  int64_t count;
  /* used in cycle search, so that we do not clobber original counts.  */
  int64_t cs_count;

  uint32_t count_valid : 1;
  uint32_t on_tree : 1;
  uint32_t fake : 1;
  uint32_t fall_through : 1;

  /* Arc to a catch handler.  */
  uint32_t is_throw : 1;

  /* Arc is for a function that abnormally returns.  */
  uint32_t is_call_non_return : 1;

  /* Arc is for catch/setjmp.  */
  uint32_t is_nonlocal_return : 1;

  /* Is an unconditional branch.  */
  uint32_t is_unconditional : 1;

  /* Loop making arc.  */
  uint32_t cycle : 1;

  /* Links to next arc on src and dst lists.  */
  arc_info *succ_next;
  arc_info *pred_next;
};


/* Describes which locations (lines and files) are associated with
   a basic block.  */

class block_location_info {
public:
  block_location_info(unsigned _source_file_idx)
      : source_file_idx(_source_file_idx) {}

  unsigned source_file_idx;
  std::vector<unsigned> lines;
};

/* Describes a basic block. Contains lists of arcs to successor and
   predecessor blocks.  */
struct block_info {
  /* Chain of exit and entry arcs.  */
  arc_info *succ;
  arc_info *pred;

  /* Number of unprocessed exit and entry arcs.  */
  int64_t num_succ;
  int64_t num_pred;

  unsigned id;

  /* Block execution count.  */
  int64_t count;
  uint32_t count_valid : 1;
  uint32_t valid_chain : 1;
  uint32_t invalid_chain : 1;
  uint32_t exceptional : 1;

  /* Block is a call instrumenting site.  */
  uint32_t is_call_site : 1;   /* Does the call.  */
  uint32_t is_call_return : 1; /* Is the return.  */

  /* Block is a landing pad for longjmp or throw.  */
  uint32_t is_nonlocal_return : 1;

  std::vector<block_location_info> locations;

  struct {
    /* Single line graph cycle workspace.  Used for all-blocks
       mode.  */
    arc_info *arc;
    unsigned ident;
  } cycle; /* Used in all-blocks mode, after blocks are linked onto
             lines.  */

  /* Temporary chain for solving graph, and for chaining blocks on one
     line.  */
  struct block_info *chain;
};

/* Describes which locations (lines and files) are associated with
   a basic block.  */

class line_info {
public:
  /* Default constructor.  */
  line_info();

  /* Return true when NEEDLE is one of basic blocks the line belongs to.  */
  bool has_block(block_info *needle);

  /* Execution count.  */
  int64_t count;

  /* Branches from blocks that end on this line.  */
  std::vector<arc_info *> branches;

  /* blocks which start on this line.  Used in all-blocks mode.  */
  std::vector<block_info *> blocks;

  uint32_t exists : 1;
  uint32_t unexceptional : 1;
  uint32_t has_unexecuted_block : 1;
};


struct function_info_t {
	
	/* Name of function.  */
	std::string m_name;
	char *m_demangled_name;
	uint32_t ident;
	uint32_t lineno_checksum;
	uint32_t cfg_checksum;

	/* The graph contains at least one fake incoming edge.  */
	uint32_t has_catch : 1;

	/* True when the function is artificial and does not exist
	 in a source file.  */
	uint32_t artificial : 1;

	/* True when multiple functions start at a line in a source file.  */
	uint32_t is_group : 1;

	/* Array of basic blocks.  Like in GCC, the entry block is
	 at blocks[0] and the exit block is at blocks[1].  */
	#define ENTRY_BLOCK (0)
	#define EXIT_BLOCK (1)
	std::vector<struct block_info> blocks;
	uint32_t blocks_executed;

	/* Raw arc coverage counts.  */
	std::vector<int64_t> counts;

	/* All arcs in function  */
	std::vector<arc_info*> arcs;

	/* First line number.  */
	uint32_t start_line;

	/* First line column.  */
	uint32_t start_column;

	/* Last line number.  */
	uint32_t end_line;

	/* Last line column.  */
	uint32_t end_column;

	/* Index of source file where the function is defined.  */
	uint32_t src;

	/* Vector of line information (used only for group functions).  */
	std::vector<line_info> lines;

	/* Next function.  */
	function_info_t *next;
};

/* Object & program summary record.  */

struct gcov_summary
{
  uint32_t runs;		/* Number of program runs.  */
  int64_t sum_max;    	/* Sum of individual run max values.  */
};


int read_count_file(std::string count_file_name, std::map<uint32_t, function_info_t *>* ident_to_fn);
int read_notes_file(std::string notes_file_name, std::vector<function_info_t*>* functions, std::map<uint32_t, function_info_t *>* ident_to_fn);
void solve_flow_graph(function_info_t *fn, std::string notes_file_name);


#endif
