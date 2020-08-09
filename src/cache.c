//  Mehrnoosh:

#include "cache.h"
#include "memory_controller.h"
#include <stdio.h>
#include <stdlib.h>




Cacheline LLC[NUM_SET][NUM_WAY];     // the last level cache
// char LRU[NUM_SET][NUM_WAY];          // an array to keep track of lru for eviction
long long int LRU[NUM_SET][NUM_WAY];          // an array to keep track of lru for eviction
int cache_dirty = 0;                 // # dirty blocks ever
int dirty_coor[2] = {0};             // keep the coordinates of last dirty block llc[i][j] ~~~> dirty_coor[0] = i, dirty_coor[1] = j

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
    LRU[index][way] = CYCLE_VAL;
    // if (LRU[index][way] >= NUM_WAY - 1)
    // {
    //     LRU[index][way] = 0;
    // }
    // else
    // {
    //     LRU[index][way]++;
    // } 
}

void reset_LRU(unsigned int index, unsigned int way){
    LRU[index][way] = CYCLE_VAL;
    // LRU[index][way] = 1;
}


int find_spot(unsigned int index){
    for (int j = 0; j < NUM_WAY; j++)
    {
        if (!LLC[index][j].valid)
        {
            return j;
        }
    }
    return -1;  
}


// find the cacheline with the least recently used
int find_victim(unsigned int index) {
    int victim = -1;
    // char min = NUM_WAY;
    long long int min = CYCLE_VAL;
    for (int j = 0; j < NUM_WAY; j++)
    {
        if (LRU[index][j] < min)
        {
            victim = j;
            min = LRU[index][j];
        }
    }

    return victim;
}



unsigned int get_index(unsigned int addr){
    unsigned int index = addr << TAG_WIDTH;
    index = index >> (TAG_WIDTH+OFFSET_WIDTH);
    // unsigned int index = addr >> OFFSET_WIDTH;
    // index = index % NUM_SET;
    return index;

}

unsigned int get_tag(unsigned int addr){
    unsigned int tag = addr >> (INDEX_WIDTH+OFFSET_WIDTH);
    // unsigned int tag = addr >> OFFSET_WIDTH;
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
                cache_dirty++;

                dirty_coor[0] = index;
                dirty_coor[1] = j;

            }
            update_LRU(index, j);  
            return true;    
        }        
    }
    // miss
    return false;
}


int cache_invalidate(unsigned int addr){
    unsigned int index = get_index(addr);
    unsigned int tag = get_tag(addr);

    for (unsigned int j = 0; j < NUM_WAY; j++)
    {
        // hit
        if (LLC[index][j].tag == tag && LLC[index][j].valid)
        {   
            LLC[index][j].valid = false;
            LLC[index][j].dirty = false;
            cache_dirty--;
            return  LLC[index][j].addr;    
        }        
    }
    // miss
    return -1;
}

void cache_clean(int i, int j){
    LLC[i][j].dirty = false;
}

// try to fill the cache with new data, it may lead to eviction ~~~> is called when miss happens
int cache_fill(unsigned int addr,  char type){
    unsigned int index = get_index(addr);
    unsigned int tag = get_tag(addr);

    int victim = -1;
   
    // miss only
    int way = find_spot(index);

    // miss & evict
    if (way == -1)
    {
        way = find_victim(index);
        if (LLC[index][way].dirty)
        {
            victim = LLC[index][way].addr;
        }
    }


     if (way < 0)
    {
        printf("ERROR: cahce fill way: %d   index %d\n", way, index);
        exit(1);
    }

    // cacheline fill
    LLC[index][way].valid = true;
    LLC[index][way].tag = tag;
    LLC[index][way].addr = addr;
    LLC[index][way].dirty = false;

    if (type == CWRITE)
    {
        LLC[index][way].dirty = true;
        cache_dirty++;

        dirty_coor[0] = index;
        dirty_coor[1] = way;
    }

    reset_LRU(index, way);  

    return victim;  
}

//  Mehrnoosh.