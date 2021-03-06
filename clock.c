#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

int clock_hand;

/* Page to evict is chosen using the clock algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int clock_evict() {
	int index = clock_hand; //set index equal to clock hand index
	while ((coremap[index].pte->frame) & PG_REF){ //PG_REF is ref bit and loops until frame with ref bit 0 is found

		coremap[index].pte->frame &= (~PG_REF); //set ref bit to 0

		index++; //iterate to next index

		if (index == memsize){ //reset index to 0 if equal to memsize

			index = 0; 
		}


	} 
	clock_hand = index; //clock_hand stores current index for next time
	return index; //return first frame with ref bit 0
	
		
}

/* This function is called on each access to a page to update any information
 * needed by the clock algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(pgtbl_entry_t *p) {
	p->frame |= PG_REF; //set ref bit each time a page is accessed
	return;
}

/* Initialize any data structures needed for this replacement
 * algorithm. 
 */
void clock_init() {
	clock_hand = 0; //clock_hand initial position is 0.
}
