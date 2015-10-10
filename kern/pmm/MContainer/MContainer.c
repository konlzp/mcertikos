#include <lib/debug.h>
#include <lib/x86.h>
#include "import.h"

struct SContainer {
  int quota;      //maximum memory quota of the process
  int usage;      //the current memory usage of the process
  int parent;     //the id of the parent process
  int nchildren;  //the number of child processes
  int used;       //whether current container is used by a process
};

// mCertiKOS supports up to NUM_IDS processes
static struct SContainer CONTAINER[NUM_IDS]; 

/**
 * Initializes the container data for the root process (the one with index 0).
 * The root process is the one that gets spawned first by the kernel.
 */
void container_init(unsigned int mbi_addr)
{
  unsigned int real_quota;
  unsigned int nps;
  unsigned int index;

  pmem_init(mbi_addr);
  real_quota = 0;
  nps = get_nps();
  /**
   * TODO: compute the available quota and store it into the variable real_quota.
   * It should be the number of the unallocated pages with the normal permission
   * in the physical memory allocation table.
   */
  for(index = 0; index < nps; index ++) {
	if(at_is_norm(index) && (!at_is_allocated(index))) {
		real_quota ++;
	}
  }
  
  KERN_DEBUG("\nnps: %u", nps);
  KERN_DEBUG("\nreal quota: %d\n\n", real_quota);

  CONTAINER[0].quota = real_quota;
  CONTAINER[0].usage = 0;
  CONTAINER[0].parent = 0;
  CONTAINER[0].nchildren = 0;
  CONTAINER[0].used = 1;
}


// get the id of parent process of process # [id]
unsigned int container_get_parent(unsigned int id)
{
  if(id < NUM_IDS && id >= 0) {
	return CONTAINER[id].parent;
  }
  return 0;
}


// get the number of children of process # [id]
unsigned int container_get_nchildren(unsigned int id)
{
  if(id < NUM_IDS && id >= 0) {
	return CONTAINER[id].nchildren;
  }
  return 0;
}


// get the maximum memory quota of process # [id]
unsigned int container_get_quota(unsigned int id)
{
  if(id < NUM_IDS && id >= 0) {
	return CONTAINER[id].quota;
  }
  return 0;
}


// get the current memory usage of process # [id]
unsigned int container_get_usage(unsigned int id)
{
  if(id < NUM_IDS && id >= 0) {
	return CONTAINER[id].usage;
  }
  return 0;
}


// determines whether the process # [id] can consume extra
// [n] pages of memory. If so, returns 1, o.w., returns 0.
unsigned int container_can_consume(unsigned int id, unsigned int n)
{
  if(container_get_quota(id) - container_get_usage(id) >= n) {
	return 1;
  }
  return 0;
}


/**
 * dedicates [quota] pages of memory for a new child process.
 * you can assume it is safe to allocate [quota] pages (i.e., the check is already done outside before calling this function)
 * returns the container index for the new child process.
 */
unsigned int container_split(unsigned int id, unsigned int quota)
{
  unsigned int child, nc;

  nc = CONTAINER[id].nchildren;
  child = id * MAX_CHILDREN + 1 + nc; //container index for the child process

  while(CONTAINER[child].used){
	child++;
  }

  CONTAINER[child].parent = id;
  CONTAINER[child].quota = quota;
  CONTAINER[child].used = 1;
  CONTAINER[id].usage += quota;
  CONTAINER[id].nchildren ++;
  /* update the container structure of both parent and child process appropriately.*/

  return child;
}


/**
 * allocates one more page for process # [id], given that its usage would not exceed the quota.
 * the container structure should be updated accordingly after the allocation.
 * returns the page index of the allocated page, or 0 in the case of failure.
 */
unsigned int container_alloc(unsigned int id)
{
  unsigned int pageIndex;

  if(container_can_consume(id, 1)){
	if(pageIndex = palloc()){
		CONTAINER[id].used = 1;
		CONTAINER[id].usage ++;
		return pageIndex;
	}
  }
  /* implement the function here.*/
  return 0;
}

// frees the physical page and reduces the usage by 1.
void container_free(unsigned int id, unsigned int page_index)
{
  CONTAINER[id].usage --;
  pfree(page_index);
}
