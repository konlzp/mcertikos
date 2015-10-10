#include <lib/debug.h>
#include "import.h"

#define PAGESIZE	4096
#define VM_USERLO	0x40000000
#define VM_USERHI	0xF0000000
#define VM_USERLO_PI	(VM_USERLO / PAGESIZE)
#define VM_USERHI_PI	(VM_USERHI / PAGESIZE)

/**
 * The initialization function for the allocation table AT.
 * It contains two major parts:
 * 1. Calculate the actual physical memory of the machine, and sets the number of physical pages (NUM_PAGES).
 * 2. Initializes the physical allocation table (AT) implemented in MATIntro layer, based on the
 *    information available in the physical memory map table.
 *    Review import.h in the current directory for the list of avaiable getter and setter functions.
 */
void
pmem_init(unsigned int mbi_addr)
{
	unsigned int nps;
	unsigned int i, j, size, maxAddr;
	
  //Calls the lower layer initializatin primitives.
  //The parameter mbi_addr shell not be used in the further code.
	devinit(mbi_addr);

  /**
   * Calculate the number of actual number of avaiable physical pages and store it into the local varaible nps.
   * Hint: Think of it as the highest address possible in the ranges of the memory map table,
   *       divided by the page size.
   */
	size = get_size();
	maxAddr = get_mms(0) + get_mml(0);
	for(i = 0; i < size; i ++) {
		if(maxAddr < get_mms(i) + get_mml(i)){
			maxAddr = get_mms(i) + get_mml(i);
		}
	}
	nps = maxAddr / PAGESIZE;
	dprintf("%u\n",nps);
	set_nps(nps); // Setting the value computed above to NUM_PAGES.
  /**
   * Initialization of the physical allocation table (AT).
   *
   * In CertiKOS, the entire addresses < VM_USERLO or >= VM_USERHI are reserved by the kernel.
   * That corresponds to the physical pages from 0 to VM_USERLO_PI-1, and from VM_USERHI_PI to NUM_PAGES-1.
   * The rest of pages that correspond to addresses [VM_USERLO, VM_USERHI), can be used freely ONLY IF
   * the entire page falls into one of the ranges in the memory map table with the permission marked as usable.
   *
   * Hint:
   * 1. You have to initialize AT for all the page indices from 0 to NPS - 1.
   * 2. For the pages that are reserved by the kernel, simply set its permission to 1.
   *    Recall that the setter at_set_perm also marks the page as unallocated. 
   *    Thus, you don't have to call another function to set the allocation flag.
   * 3. For the rest of the pages, explore the memory map table to set its permission accordingly.
   *    The permission should be set to 2 only if there is a range containing the entire page that
   *    is marked as available in the memory map table. O.w., it should be set to 0.
   *    Note that the ranges in the memory map are not aligned by pages.
   *    So it may be possible that for some pages, only part of the addresses are in the ranges.
   *    Currently, we do not utilize partial pages, so in that case, you should consider those pages as unavailble.
   * 4. Every page in the allocation table shold be initialized.
   *    But the ranges in the momory map table may not cover the entire available address space.
   *    That means there may be some gaps between the ranges.
   *    You should still set the permission of those pages in allocation table to 0.
   */
	j = 0;
    for(i = 0; i < nps; i ++){
		if(((i << 12) <= get_mms(j) + get_mml(j)) && (((i + 1) << 12) > get_mms(j) + get_mml(j))) {
			j ++;
			at_set_perm(i, 0);
		}
		else if(!is_usable(j)){
			at_set_perm(i, 0);
		}
		else if(i < VM_USERLO_PI || i >= VM_USERHI_PI){
			at_set_perm(i, 1);
		}	
		else {
			at_set_perm(i, 2);
		}
	}
}


