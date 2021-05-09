/*
 * File: pager-lru.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains an lru pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) { 
    
    /* This file contains the stub for an LRU pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];

    /* Local vars */
    int proctmp;
    int pagetmp;

    int proc;
    int pc;
    int page;
    int oldPage;
    int currentTick;
    int oldPageTick;
    int pageToSwap;

    /* initialize static vars on first run */
    if(!initialized){
	for(proctmp=0; proctmp < MAXPROCESSES; proctmp++){
	    for(pagetmp=0; pagetmp < MAXPROCPAGES; pagetmp++){
		timestamps[proctmp][pagetmp] = 0; 
	    }
	}
	initialized = 1;
    }
    
    /* TODO: Implement LRU Paging */
   	/* select first active process */
    for(proc = 0; proc < MAXPROCESSES; proc++) {
    	if(q[proc].active) {
    		/* Dedicate all work to first active process*/ 
	    	pc = q[proc].pc; 		        // program counter for process
	    	page = pc/PAGESIZE;				// page the program counter needs
    		/* Update tick value for this page, even if it's in memory */
    		timestamps[proc][page] = tick; 
    		/* If page isn't in memory */
    		if(!q[proc].pages[page]) {
    			/* Try to swap in */
    			if(!pagein(proc,page)) {
    				/* LRU IMPLEMENTATION */
    				currentTick = tick;
    				/* Memory is full, swap another page out */
    				for(oldPage = 0; oldPage < q[proc].npages; oldPage++) {
    					/* Don't swap out the page that we want swapped in */
    					if(oldPage != page) {
    						oldPageTick = timestamps[proc][oldPage];
    						/* if old page was used least then current */
    						if(oldPageTick < currentTick) {
    							/* check if that page still exists in memory */
    							if(q[proc].pages[oldPage]) {
    								/* update page to be swapped out */
    								pageToSwap = oldPage;
    								/* new smallest tick value */
    								currentTick = oldPageTick;
    						    }
    						}
    					}
    				}
    				/* we now have the LRU page (smallest tick still in memory) */
    				/* swap out the page */
    				if(!pageout(proc,pageToSwap)) {
    					/* page couldn't be swapped out */
    					printf("Failed to pageout a page\n");
    				}
    			}
    		}
    	}
    }
    /* advance time for next pageit iteration */
    tick++;
} 
