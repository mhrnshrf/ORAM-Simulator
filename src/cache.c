//  Mehrnoosh:

#include "cache.h"
#include <stdio.h>



Cacheline LLC[NUM_SET][NUM_WAY];     // the cache
char LRU[NUM_SET][NUM_WAY];                  // a array to keep track of lru for eviction

// invalidate all cahce blocks upon init
void cache_init(){
    for (unsigned int i = 0; i < NUM_SET; i++)
    {
        for (unsigned int j = 0; j < NUM_WAY; j++)
        {
            LLC[i][j].valid = false;
            LLC[i][j].dirty = false;
            LLC[i][j].tag = -1;
            
            LRU[i][j] = 0;
        }
    }
}

void update_LRU(unsigned int index, unsigned int way){
    if (LRU[index][way] == 255)
    {
        LRU[index][way] = 0;
    }
    else
    {
        LRU[index][way]++;
    } 
}

void reset_LRU(unsigned int index, unsigned int way){
    LRU[index][way] = 1;

}


unsigned int find_spot(unsigned int index){
    for (unsigned int j = 0; j < NUM_WAY; j++)
    {
        if (!LLC[index][j].valid)
        {
            return j;
        }
    }
    return -1;  
}


// find the cacheline with the least recently used
unsigned int find_victim(unsigned int index) {
    unsigned int victim = -1;
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



unsigned int get_index(unsigned int addr){
    unsigned int index = addr << TAG_WIDTH;
    index = index >> (TAG_WIDTH+OFFSET_WIDTH);
    return index;

}

unsigned int get_tag(unsigned int addr){
    unsigned int tag = addr >> (INDEX_WIDTH+OFFSET_WIDTH);
    return tag;
}


// return true on hit and false on miss
bool cache_access(unsigned int addr, char type){
    unsigned int index = get_index(addr);
    unsigned int tag = get_tag(addr);

    for (unsigned int j = 0; j < NUM_WAY; j++)
    {
        // hit
        if (LLC[index][j].tag == tag && LLC[index][j].valid)
        {   
            if (type == CWRITE)
            {
                LLC[index][j].dirty = true;
            }
            update_LRU(index, j);  
            return true;    
        }        
    }
    // miss
    return false;
}


// try to fill the cache with new data, it may lead to eviction ~~~> is called when miss happens
unsigned int cache_fill(unsigned int addr,  char type){
    unsigned int index = get_index(addr);
    unsigned int tag = get_tag(addr);

    unsigned int victim = -1;
   
    // miss only
    unsigned int way = find_spot(index);

    // miss & evict
    if (way == -1)
    {
        way = find_victim(index);
        if (LLC[index][way].dirty)
        {
            victim = LLC[index][way].addr;
        }
    }


    // cacheline fill
    if (type == CWRITE)
    {
        LLC[index][way].dirty = true;
    }
    LLC[index][way].valid = true;
    LLC[index][way].tag = tag;
    LLC[index][way].addr = addr;
    reset_LRU(index, way);  

    return victim;  
}

//  Mehrnoosh.