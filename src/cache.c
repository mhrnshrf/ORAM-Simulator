//  Mehrnoosh:

#include "cache.h"



Cacheline LLC[NUM_SET][NUM_WAY];     // the cache
char LRU[NUM_SET][NUM_WAY];                  // a array to keep track of lru for eviction

// invalidate all cahce blocks upon init
void cache_init(){
    for (int i = 0; i < NUM_SET; i++)
    {
        for (int j = 0; j < NUM_WAY; j++)
        {
            LLC[i][j].valid = false;
            LLC[i][j].dirty = false;
            LLC[i][j].tag = -1;
            
            LRU[i][j] = 0;
        }
    }
}

void update_LRU(int index, int way){
    if (LRU[index][way] == 255)
    {
        LRU[index][way] = 0;
    }
    else
    {
        LRU[index][way]++;
    } 
}

void reset_LRU(int index, int way){
    LRU[index][way] = 1;
}


int find_spot(int index){
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
int find_victim(int index) {
    int victim = -1;
    for (int j = 0; j < NUM_WAY; j++)
    {
        int min = 256;
        if (LRU[index][j] < min)
        {
            victim = j;
        }
    }

    return victim;
}



int get_index(int addr){
    int index = addr << TAG_WIDTH;
    index = index >> (TAG_WIDTH+OFFSET_WIDTH);
    return index;

}

int get_tag(int addr){
    int tag = addr >> (INDEX_WIDTH+OFFSET_WIDTH);
    return tag;
}


// return true on hit and false on miss
bool cache_access(int addr, char type){
    int index = get_index(addr);
    int tag = get_tag(addr);

    for (int j = 0; j < NUM_WAY; j++)
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
int cache_fill(int addr,  char type){
    int index = get_index(addr);
    int tag = get_tag(addr);

    int victim = -1;
   
    // miss only
    int way = find_spot(index);

    // miss & evict
    if (way != -1)
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
    LLC[index][way].tag = addr;
    reset_LRU(index, way);  

    return victim;  
}

//  Mehrnoosh.