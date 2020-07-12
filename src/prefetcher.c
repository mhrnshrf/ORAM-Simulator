//  Mehrnoosh:

#include "prefetcher.h"
#include <stdio.h>



HistEntry HistoryTable[NUM_SET][NUM_WAY];     // the history table
char LRU[NUM_SET][NUM_WAY];                   // an array to keep track of lru for eviction
MatchType INDEX_VAR = PC;
MatchType TAG_VAR = PC_ADDR;


// profiling stat
int match_hit = 0;


// invalidate all cahce blocks upon init
void table_init(){
    for (unsigned int i = 0; i < NUM_SET; i++)
    {
        for (unsigned int j = 0; j < NUM_WAY; j++)
        {
            // HistoryTable[i][j].dirty = false;
            HistoryTable[i][j].valid = false;
            // HistoryTable[i][j].tag = -1;
            
            LRU[i][j] = 0;
        }
    }
}

void updateLRU_hist(unsigned int index, unsigned int way){
    if (LRU[index][way] == 255)
    {
        LRU[index][way] = 0;
    }
    else
    {
        LRU[index][way]++;
    } 
}

void resetLRU_hist(unsigned int index, unsigned int way){
    LRU[index][way] = 1;

}


int find_spot_hist(unsigned int index){
    for (unsigned int j = 0; j < NUM_WAY; j++)
    {
        if (!HistoryTable[index][j].valid)
        {
            return j;
        }
    }
    return -1;  
}


// find the tableline with the least recently used
int find_victim_hist(unsigned int index) {
    int victim = -1;
    for (unsigned int j = 0; j < NUM_WAY; j++)
    {
        unsigned int min = 256;
        if (LRU[index][j] < min)
        {
            victim = j;
        }
    }

    return victim;
}



unsigned int index_hist(Event e){
    unsigned int index; 
    switch (INDEX_VAR)
    {
    case PC:
        index = e.pc % NUM_SET; 
        break;
    case ADDR:
        index = e.pc % NUM_SET; 
        break;    
    default:
        index = e.pc % NUM_SET;
        break;
    }

    return index;
}


bool match_tag(Event e, int index, int way){
    if (HistoryTable[index][way].valid)
    {
        switch (TAG_VAR)
        {
        case PC_ADDR:
            if (HistoryTable[index][way].tag.pc == e.pc && HistoryTable[index][way].tag.addr == e.addr)
            {
                return true;
            }
            break;
        case PC_OFFSET:
            if (HistoryTable[index][way].tag.pc == e.pc && HistoryTable[index][way].tag.offset == e.offset)
            {
                return true;
            }
            break;
        case PC:
            if (HistoryTable[index][way].tag.pc == e.pc)
            {
                return true;
            }
            break;
        case ADDR:
            if (HistoryTable[index][way].tag.addr == e.addr)
            {
                return true;
            }
        case OFFSET:
            if (HistoryTable[index][way].tag.offset == e.offset)
            {
                return true;
            }    
            break;
        default:  
            break;
        }
    }
    else
    {
        return false;
    }
    
    return false;
}

// return true on hit and false on miss
int table_access(Event e){
    unsigned int index = index_hist(e);

    for (unsigned int j = 0; j < NUM_WAY; j++)
    {
        // hit
        if (match_tag(e, index, j))
        {   
            updateLRU_hist(index, j);
            match_hit++;  
            return HistoryTable[index][j].candidate;    
        }        
    }
    // miss
    return -1;
}


// try to fill the table with new data, it may lead to eviction ~~~> evicted entry is simply overwritten
void table_fill(Event e, unsigned int candidate){
    unsigned int index = index_hist(e);  

    // miss only
    int way = find_spot_hist(index);

    // miss & evict
    if (way == -1)
    {
        way = find_victim_hist(index);
    }

    HistoryTable[index][way].valid = true;
    HistoryTable[index][way].tag.pc = e.pc;
    HistoryTable[index][way].tag.addr = e.addr;
    HistoryTable[index][way].tag.offset = e.offset;
    HistoryTable[index][way].candidate = candidate;
    resetLRU_hist(index, way);  

}

//  Mehrnoosh.