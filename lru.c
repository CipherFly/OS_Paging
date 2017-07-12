#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

//array of ints. Contains frame indexes of coremap.
int* lru_stack;

/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {
	//returns frame that is at the top of lru_stack
	int last_rec_used = memsize - 1;
	while (lru_stack[last_rec_used] == -1){
		last_rec_used--;
		if (last_rec_used < 0){
			return 0;
		}
	}
	return lru_stack[last_rec_used];
}	

//finds frame in the stack, returns the index in lru_stack
int find_in_stack(int frame){
	int t = 0;
	while(t < memsize){
		if(frame == lru_stack[t]){
			return t;
			
		}
		t++;
	}
	return -1;
}



/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {
	int in_stack = -1;
	//if frame is in stack, in_stack = index of frame in the stack. -1 means it wasn't in stack.
	//if pte is in stack, ref bit is set
	if (p->frame & PG_REF){
		in_stack = find_in_stack(p->frame >> PAGE_SHIFT);
	}
	if(in_stack != -1){
		int p = 0;
		//remove the frame from the stack
		while(p < memsize){
			if(p >= in_stack){
				if((p+1) < memsize){
					lru_stack[p] = lru_stack[p+1];
				}
			}
			//place -1 at the top of stack
			if(p == (memsize - 1)){
				lru_stack[p] = -1;
			}
			p++;
		}
	}

	//we are putting pte in stack
	p->frame |= PG_REF;

	//index starts at the top of the stack
	int index = memsize - 1;
	//push frame on the stack (at the bottom)
	while (index > -1){
		//when the index reaches 0, the frame number is pushed to the bottom of the stack
		if (index == 0){
			lru_stack[index] = (p->frame >> PAGE_SHIFT);
			return;
		}
		lru_stack[index] = lru_stack[index - 1];
		index--;
	}
	
	return;
}



/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {
	//allocates space for array of ints
	lru_stack = malloc(sizeof(int) * memsize);
	//fills stack with -1	
	int t = 0;
	while(t < memsize){
		lru_stack[t] = -1;
		t++;
	}
	return;
}
