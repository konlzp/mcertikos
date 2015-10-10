#include <lib/debug.h>
#include "import.h"

/**
 * Allocation of a physical page.
 *
 * 1. First, implement a naive page allocator that scans the allocation table (AT) 
 *    using the functions defined in import.h to find the first unallocated page
 *    with usable permission.
 *    (Q: Do you have to scan allocation table from index 0? Recall how you have
 *    initialized the table in pmem_init.)
 *    Then mark the page as allocated in the allocation table and return the page
 *    index of the page found. In the case when there is no avaiable page found,
 *    return 0.
 * 2. Optimize the code with the memorization techniques so that you do not have to
 *    scan the allocation table from scratch every time.
 */
static int prevAlloc = 0;

unsigned int
palloc()
{
  unsigned int nps = get_nps();
  unsigned int i;

  for(i = prevAlloc + 1 > VM_USERLO_PI ? prevAlloc + 1 : VM_USERLO_PI; i != prevAlloc; ){
	if(at_is_norm(i) && !at_is_allocated(i)){
		at_set_allocated(i, 1);
		prevAlloc = i;
		return i;
	}
	i ++;
	if(i >= VM_USERHI_PI) {
		i = VM_USERLO_PI;
	}
  }
  return 0;
} 


/**
 * Free of a physical page.
 *
 * This function marks the page with given index as unallocated
 * in the allocation table.
 *
 * Hint: Simple.
 */
void
pfree(unsigned int pfree_index)
{
  at_set_allocated(pfree_index, 0);
}
