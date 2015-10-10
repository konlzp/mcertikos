#ifndef _KERN_MM_MALOP_H_
#define _KERN_MM_MALOP_H_

#define PAGESIZE 4096
#define VM_USERLO 0x40000000
#define VM_USERHI 0xF0000000
#define VM_USERLO_PI (VM_USERLO / PAGESIZE)
#define VM_USERHI_PI (VM_USERHI / PAGESIZE)

#ifdef _KERN_

/**
 * The getter and setter functions implemented in the MATIntro layer.
 */

// The number of avaiable physical pages.
unsigned int get_nps(void);

// Whether the page with the given index has the normal permission.
unsigned int at_is_norm(unsigned int);

// Whether the page with the given index is already allocated.
unsigned int at_is_allocated(unsigned int);

// Mark the allocation flag of the page with the given index using the given value.
void at_set_allocated(unsigned int, unsigned int);

#endif /* _KERN_ */

#endif /* !_KERN_MM_MALOP_H_ */
