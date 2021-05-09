/*
 * File: pager-predict.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 *  This file contains a predictive pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>
#include <limits.h>

#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) { 
    
    /* This file contains the stub for a predictive pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int timeStampsInitialized = 0;
    static int matrixInitialized = 0;
    static int previousInitialized = 0;
    static int tick = 1; // artificial time
    /* Transition matrix - transitionMatrix[0][0][1] = # of times P0 has gone to page1 from page0 */
    static int transitionMatrix[MAXPROCESSES][MAXPROCPAGES][MAXPROCPAGES];
    /* Use timestamps to get which page we just came from */
    static int timeStamps[MAXPROCESSES][MAXPROCPAGES];
    /* Array that holds each process' previous page */
    static int previousPage[MAXPROCESSES];
    
    
    int proctmp;
    int pagetmp1;
    int pagetmp2;

    if(!timeStampsInitialized){
    for(proctmp=0; proctmp < MAXPROCESSES; proctmp++){
        for(pagetmp1=0; pagetmp1 < MAXPROCPAGES; pagetmp1++){
        timeStamps[proctmp][pagetmp1] = 0; 
        }
    }
    timeStampsInitialized = 1;
    }

    /*initialize transition matrix on first run */
    if(!matrixInitialized){
    /* Init complex static vars here */
    for(proctmp = 0; proctmp < MAXPROCESSES; proctmp++) { 
        for(pagetmp1 = 0; pagetmp1 < MAXPROCPAGES; pagetmp1++) {
            for(pagetmp2 = 0; pagetmp2 < MAXPROCPAGES; pagetmp2++) {
                transitionMatrix[proctmp][pagetmp1][pagetmp2] = 0;
            }
        }
    }
    matrixInitialized = 1;
    }

    /* initialize previousPage array on first run */
    if(!previousInitialized) {
        for(proctmp = 0; proctmp < MAXPROCESSES; proctmp++) {
            previousPage[proctmp] = -1;
        }
        previousInitialized = 1;
    }
    
    /* TODO: Implement Predictive Paging */
    /*local vars */
    int proc;
    int pc;
    int page;
    /* for LRU implementation */
    int oldPage;
    int oldPageTick;
    int currentTick;
    int pageToSwap;
    /* for finding most transitioned to page */
    int maxItem;
    int mLP;
    /* for finding least transitioned to page */
    int minItem;
    int lLP;

    /* Track how many frames are in use at the time of this pageit() call */
    int pagesInUse = 0;

    /* Calculate the total number of pages currently in */
    for(proc = 0; proc < MAXPROCESSES; proc++) {
        for(page = 0; page < MAXPROCPAGES; page++) {
            pagesInUse += q[proc].pages[page];
        }
    }
    // if(tick % 500 == 0) {
    //     printf("Pages in use: %d\n", pagesInUse);
    // }

    /* select first active process */
    for(proc = 0; proc < MAXPROCESSES; proc++) {
        if(q[proc].active) {
            pc = q[proc].pc;
            page = pc/PAGESIZE;
            /*update tick value for this page */
            timeStamps[proc][page] = tick;
            /* update transition matrix */
            if(previousPage[proc] == -1) {
                /* no previous page */
                transitionMatrix[proc][page][page] += 0;
                /* page in page, page + 1*/
                pagein(proc,page);
                pagein(proc,page + 1);                
                /* update previousPage array */
                previousPage[proc] = page;
                continue;

            }
            else if(page != previousPage[proc]) {
                /* have a previous page */
                transitionMatrix[proc][previousPage[proc]][page] += 1;
            }
            /* most likely page is most transitioned to page */
            maxItem = transitionMatrix[proc][page][0];
            mLP = 0;
            for(int i = 0; i < MAXPROCPAGES; i++) {
                if(transitionMatrix[proc][page][i] > maxItem) {
                    maxItem = transitionMatrix[proc][previousPage[proc]][i];
                    mLP = i;
                }
            }
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
                            oldPageTick = timeStamps[proc][oldPage];
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
                    pageout(proc,pageToSwap);
                }
            }
            /* if mLP isn't in memory */
            if(!q[proc].pages[mLP]) {
                /* Try to swap in */
                if(!pagein(proc,mLP)) {
                    /* LRU IMPLEMENTATION */
                    currentTick = tick;
                    /* Memory is full, swap another page out */
                    for(oldPage = 0; oldPage < q[proc].npages; oldPage++) {
                        /* Don't swap out the page that we want swapped in */
                        if(oldPage != page) {
                            oldPageTick = timeStamps[proc][oldPage];
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
                    pageout(proc,pageToSwap);
                }
            }
        }
    /* update previousPage array */
    previousPage[proc] = page;
    }

    /* advance time for next pageit iteration */
    tick++;
} 
