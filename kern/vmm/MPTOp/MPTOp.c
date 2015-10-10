#include <lib/x86.h>

#include "import.h"
#define VM_USERLO   0x40000000
#define VM_USERHI   0xF0000000
#define VM_USERLO_PI    (VM_USERLO / PAGESIZE)
#define VM_USERHI_PI    (VM_USERHI / PAGESIZE)

/**
 * Returns the page table entry corresponding to the virtual address,
 * according to the page structure of process # [proc_index].
 * Returns 0 if the mapping does not exist.
 */
unsigned int get_ptbl_entry_by_va(unsigned int proc_index, unsigned int vaddr)
{
    unsigned int dir_index = vaddr >> 12 >> 10;
	unsigned int tbl_index = (vaddr & 0x003FF000) >> 12;

	return get_ptbl_entry(proc_index, dir_index, tbl_index);
	//return 0;
}         

// returns the page directory entry corresponding to the given virtual address
unsigned int get_pdir_entry_by_va(unsigned int proc_index, unsigned int vaddr)
{
    unsigned int dir_index = vaddr >> 12 >>10;

	return get_pdir_entry(proc_index, dir_index);
	//return 0;
}

// removes the page table entry for the given virtual address
void rmv_ptbl_entry_by_va(unsigned int proc_index, unsigned int vaddr)
{
	unsigned int dir_index = vaddr >> 12 >> 10;
	unsigned int tbl_index = (vaddr & 0x003ff000) >> 12;

	rmv_ptbl_entry(proc_index, dir_index, tbl_index);
}

// removes the page directory entry for the given virtual address
void rmv_pdir_entry_by_va(unsigned int proc_index, unsigned int vaddr)
{
	unsigned int dir_index = vaddr >> 12 >> 10;

	rmv_pdir_entry(proc_index, dir_index);
}

// maps the virtual address [vaddr] to the physical page # [page_index] with permission [perm]
// you do not need to worry about the page directory entry. just map the page table entry.
void set_ptbl_entry_by_va(unsigned int proc_index, unsigned int vaddr, unsigned int page_index, unsigned int perm)
{
	unsigned int dir_index = vaddr >> 12 >> 10;
	unsigned int tbl_index = (vaddr & 0x003ff000) >> 12;

	set_ptbl_entry(proc_index, dir_index, tbl_index, page_index, perm);
}

// registers the mapping from [vaddr] to physical page # [page_index] in the page directory
void set_pdir_entry_by_va(unsigned int proc_index, unsigned int vaddr, unsigned int page_index)
{
	unsigned int dir_index = vaddr >> 12 >> 10;

	set_pdir_entry(proc_index, dir_index, page_index);
}   

// initializes the identity page table
// the permission for the kernel memory should be PTE_P, PTE_W, and PTE_G,
// while the permission for the rest should be PTE_P and PTE_W.
void idptbl_init(unsigned int mbi_adr)
{
    unsigned int i, j;
	unsigned int page_index;

	container_init(mbi_adr);

	for(i = 0; i < 1024; i ++) {
		for(j = 0; j < 1024; j ++) {
			page_index = i << 10 | j;
			if(page_index < VM_USERLO_PI || page_index >= VM_USERHI_PI) {
				set_ptbl_entry_identity(i, j, PTE_P | PTE_W | PTE_G);
			}
			else {
				set_ptbl_entry_identity(i, j, PTE_P | PTE_W);
			}
		}
	}
}
