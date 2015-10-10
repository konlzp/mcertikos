#include <lib/x86.h>

#include "import.h"
#define PGESIZE 4096
#define VM_USERLO   0x40000000
#define VM_USERHI   0xF0000000
#define VM_USERLO_PI    (VM_USERLO / PAGESIZE)
#define VM_USERHI_PI    (VM_USERHI / PAGESIZE)

/**
 * For each process from id 0 to NUM_IDS -1,
 * set the page directory entries sothat the kernel portion of the map as identity map,
 * and the rest of page directories are unmmaped.
 */
void pdir_init(unsigned int mbi_adr)
{
	unsigned int i, j, dir_index;

    idptbl_init(mbi_adr);

	for(i = 0; i < NUM_IDS; i ++) {
		for(j = 0; j < 1024; j ++) {
			dir_index = j << 10;
			if(dir_index < VM_USERLO_PI || dir_index >= VM_USERHI_PI){
				set_pdir_entry_identity(i, j);
			}
		}
	}
}

/**
 * Allocates a page (with container_alloc) for the page table,
 * and registers it in page directory for the given virtual address,
 * and clears (set to 0) the whole page table entries for this newly mapped page table.
 * It returns the page index of the newly allocated physical page.
 * In the case when there's no physical page available, it returns 0.
 */
unsigned int alloc_ptbl(unsigned int proc_index, unsigned int vadr)
{
  unsigned int allocPgIndex = container_alloc(proc_index);
  unsigned int vadrDir = vadr >> 22;
  int i;

  if(allocPgIndex) {
	set_pdir_entry_by_va(proc_index, vadr, allocPgIndex);
	for(i = 0; i < 1024; i ++) {
		rmv_ptbl_entry(proc_index, vadrDir, i);
	}
	return allocPgIndex;
  }
  return 0;
}

// Reverse operation of alloc_ptbl.
// Removes corresponding page directory entry,
// and frees the page for the page table entries (with container_free).
void free_ptbl(unsigned int proc_index, unsigned int vadr)
{
	container_free(proc_index, get_pdir_entry_by_va(proc_index, vadr) >> 12);
	rmv_pdir_entry_by_va(proc_index, vadr);
}
