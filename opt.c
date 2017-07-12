#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"
#define MAXBUF 100

extern int memsize;

extern int debug;

extern struct frame *coremap;

extern char *tracefile; //the trace file name

int tr_size; //the number of lines in trace file

addr_t *addr_array; //array of vaddr accessed by trace file
int addr_index; //index of addr_array



/* Page to evict is chosen using the optimal (aka MIN) algorithm. 
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {
	int t = 0; //index in coremap
	int outframe = (memsize - 1); //this will return the frame with largest distance
	int dist = 0; //records largest distance to the next reference
	
	while(t < memsize){
		//only use memory indexes that are in use
		if(coremap[t].in_use == 1){
			//dist is set equal to the largest distance.
			//Two frames will have the same distance at the end of trace when they have no more references
			if(coremap[t].distance >= dist){
				dist = coremap[t].distance;
				outframe = t;
			}
		}
		t++;
	}
	return outframe;
}




/* This function is called on each access to a page to update any information
 * needed by the opt algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void opt_ref(pgtbl_entry_t *p) {
	//temp_index will have the index of addr_array
	int temp_index = addr_index;
	
	p->addr = addr_array[temp_index];//address is stored in pte

	addr_t current = p->addr; //current records the current address being referenced


	int fram = (p->frame >> PAGE_SHIFT);

	//distance to next reference is set to length of trace file left to reference
	coremap[fram].distance = tr_size - addr_index;

	temp_index++;
	int dist = 0;
	while(temp_index < tr_size){
		if(addr_array[temp_index] == current){
			coremap[fram].distance = dist + 1;
			temp_index = tr_size;
		}
		dist++;
		temp_index++;

	}
	
	coremap[fram].addr = p->addr;
	

	//addr_index goes to the addr in addr_array and frame reference bit is set
	addr_index++;
	p->frame |= PG_REF;
	return;
}


//counts the lines in trace file
int count_lines(FILE *f){
	int n = 0;
	int c = 0;
	while(!feof(f)){
		c = fgetc(f);
		if (c == '\n'){
			n++;
		}
	}
	fclose(f);
	return n;
}





/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {
	//this sequence will make an array of addr_t, containing all the addresses in trace file.
	//file pointer
	FILE *point;
	if(tracefile != NULL) {
		if((point = fopen(tracefile, "r")) == NULL) {
			perror("Error opening tracefile:");
			exit(1);
		}
	}
	//number of lines in trace file
	tr_size = count_lines(point);
	//opens trace file again, for reading
	point = fopen(tracefile, "r");


	addr_index = 0;

	//allocates space for array of addr_t
	addr_array = malloc(sizeof(addr_t) * tr_size);
	int i = 0;
	char buf[MAXBUF];
	addr_t vaddr = 0;
	char type;
	while(fgets(buf, MAXBUF, point) != NULL) {
		if(buf[0] != '=') {
			sscanf(buf, "%c %lx", &type, &vaddr);
			addr_array[i] = vaddr;
			i++;
		} else {
			continue;
		}
		
	}


	

	fclose(point);
	return;
}

