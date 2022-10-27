//  Mehrnoosh:

#include "metacache.h"
#include "memory_controller.h"
#include "params.h"
#include <stdio.h>
#include <stdlib.h>




MetaCacheline METACACHE[META_NUM_SET][META_NUM_WAY];     // the last level cache
// char LRU[META_NUM_SET][META_NUM_WAY];          // an array to keep track of lru for eviction
// long long int LRU[META_NUM_SET][META_NUM_WAY];          // an array to keep track of lru for eviction
int metacache_dirty = 0;                 // # dirty blocks ever
int meta_dirty_coor[2] = {0};             // keep the coordinates of last dirty block llc[i][j] ~~~> meta_dirty_coor[0] = i, meta_dirty_coor[1] = j

// invalidate all cahce blocks upon init
void metacache_init(){
    for (unsigned int i = 0; i < META_NUM_SET; i++)
    {
        for (unsigned int j = 0; j < META_NUM_WAY; j++)
        {
            METACACHE[i][j].valid = false;
            METACACHE[i][j].dirty = false;
            METACACHE[i][j].tag = -1;
            METACACHE[i][j].LRU = 0;
        }
    }
}

void meta_update_LRU(unsigned int index, unsigned int way){
    METACACHE[index][way].LRU = cache_clk;
    // if (LRU[index][way] >= META_NUM_WAY - 1)
    // {
    //     LRU[index][way] = 0;
    // }
    // else
    // {
    //     LRU[index][way]++;
    // } 
}

void meta_reset_LRU(unsigned int index, unsigned int way){
    METACACHE[index][way].LRU = cache_clk;
    // LRU[index][way] = 1;
}


int meta_find_spot(unsigned int index){
    for (int j = 0; j < META_NUM_WAY; j++)
    {
        if (!METACACHE[index][j].valid)
        {
            return j;
        }
    }
    return -1;  
}


// find the cacheline with the least recently used
int meta_find_victim(unsigned int index) {
    int victim = -1;
    // char min = META_NUM_WAY;
    long long int min = cache_clk;
    // printf("min %lld\n", min);
    for (int j = 0; j < META_NUM_WAY; j++)
    {
        // printf(" LRU[index][j] %lld\n",  LRU[index][j]);
        if (METACACHE[index][j].LRU < min)
        {
            victim = j;
            min = METACACHE[index][j].LRU;
        }
    }

    return victim;
}



unsigned int meta_get_index(unsigned int addr){
    unsigned int index = addr >> META_OFFSET_WIDTH;
    // index = index << (META_TAG_WIDTH);
    // index = index >> (META_TAG_WIDTH);
    index = index & ((unsigned int)pow(2, META_INDEX_WIDTH)-1);
    return index;

}

unsigned int meta_get_tag(unsigned int addr){
    unsigned int tag = addr >> (META_INDEX_WIDTH + META_OFFSET_WIDTH);
    // unsigned int tag = addr >> META_OFFSET_WIDTH;
    return tag;
}


// return true on hit and false on miss
bool metacache_access(unsigned int addr, char type){
    unsigned int index = meta_get_index(addr);
    unsigned int tag = meta_get_tag(addr);

    for (unsigned int j = 0; j < META_NUM_WAY; j++)
    {
        // hit
        if (METACACHE[index][j].tag == tag && METACACHE[index][j].valid)
        {   
            if (type == MCWRITE) //|| METACACHE_DIRTY)
            {
                // printf("METACACHE_DIRTY %s\n", METACACHE_DIRTY?"on":"off");
                METACACHE[index][j].dirty = true;
                metacache_dirty++;

                meta_dirty_coor[0] = index;
                meta_dirty_coor[1] = j;

            }
            meta_update_LRU(index, j);  
            return true;    
        }        
    }
    // miss
    return false;
}


int metacache_invalidate(unsigned int addr){
    unsigned int index = meta_get_index(addr);
    unsigned int tag = meta_get_tag(addr);

    for (unsigned int j = 0; j < META_NUM_WAY; j++)
    {
        // hit
        if (METACACHE[index][j].tag == tag && METACACHE[index][j].valid)
        {   
            METACACHE[index][j].valid = false;
            METACACHE[index][j].dirty = false;
            metacache_dirty--;
            return  METACACHE[index][j].addr;    
        }        
    }
    // miss
    return -1;
}

void metacache_clean(int i, int j){
    METACACHE[i][j].dirty = false;
}

// try to fill the cache with new data, it may lead to eviction ~~~> is called when miss happens
int metacache_fill(unsigned int addr,  char type){
    unsigned int index = meta_get_index(addr);
    unsigned int tag = meta_get_tag(addr);

    int victim = -1;
   
    // miss only
    int way = meta_find_spot(index);

    // miss & evict
    if (way == -1)
    {
        way = meta_find_victim(index);
        if (METACACHE[index][way].dirty ) /*|| METACACHE_DIRTY */
        {
            // printf("METACACHE_DIRTY %s\n", METACACHE_DIRTY?"on":"off");
            victim = METACACHE[index][way].addr;
        }
    }


     if (way < 0)
    {
        printf("ERROR: cache fill way: %d   index %d\n", way, index);
        exit(1);
    }

    // cacheline fill
    METACACHE[index][way].valid = true;
    METACACHE[index][way].tag = tag;
    METACACHE[index][way].addr = addr;
    METACACHE[index][way].dirty = false;

    if (type == MCWRITE) // || METACACHE_DIRTY)
    {
        METACACHE[index][way].dirty = true;
        metacache_dirty++;

        meta_dirty_coor[0] = index;
        meta_dirty_coor[1] = way;
    }

    meta_reset_LRU(index, way);  

    return victim;  
}

//  Mehrnoosh.