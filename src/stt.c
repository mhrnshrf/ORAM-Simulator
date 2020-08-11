//  Mehrnoosh:

#include "stt.h"
#include "memory_controller.h"
#include <stdio.h>
#include <stdlib.h>




STT_Entry STT[STT_SET][STT_WAY];     // the last level stt
// char STT[STT_SET][STT_WAY];          // an array to keep track of lru for eviction
// long long int STT[STT_SET][STT_WAY];          // an array to keep track of lru for eviction


// invalidate all stt blocks upon init
void stt_init(){
    for (unsigned int i = 0; i < STT_SET; i++)
    {
        for (unsigned int j = 0; j < STT_WAY; j++)
        {
            STT[i][j].valid = false;
            STT[i][j].pinned = false;
            
            STT[i][j].lru = 0;
        }
    }
}

void update_ts(unsigned int index, unsigned int way){
    STT[index][way].lru = CYCLE_VAL;
}

void reset_ts(unsigned int index, unsigned int way){
    STT[index][way].lru = CYCLE_VAL;
}


int stt_find_spot(unsigned int index){
    for (int j = 0; j < STT_WAY; j++)
    {
        if (!STT[index][j].valid)
        {
            return j;
        }
    }
    return -1;  
}


// find the sttline with the least recently used
int stt_find_victim(unsigned int index) {
    if (STT_WAY == 1)
    {
        return 0;
    }
    
    int victim = -1;
    long long int min = CYCLE_VAL;
    for (int j = 0; j < STT_WAY; j++)
    {
        if (STT[index][j].lru < min && !STT[index][j].pinned)
        {
            victim = j;
            min = STT[index][j].lru;
        }
    }

    return victim;
}

// void stt_pin(unsigned int addr){
//     unsigned int index = stt_index(addr);
//     unsigned int tag = stt_tag(addr);

//     for (int i = 0; i < STT_WAY; i++)
//     {
//         if (STT[index][i].pinned)
//         {
//             pincount++;
//         }
//     }
    
//     if (pincount < STT_WAY-1)
//     {
//         for (unsigned int j = 0; j < STT_WAY; j++)
//         {
//             // exist
//             if (STT[index][j].tag == tag && STT[index][j].valid)
//             {   
//                 STT[index][j].pinned = true;
//                 pinctr++;
//                 return;    
//             }        
//         }
//     }

// }


// void stt_unpin(unsigned int addr){
//     unsigned int index = stt_index(addr);
//     unsigned int tag = stt_tag(addr);

//     for (unsigned int j = 0; j < STT_WAY; j++)
//     {
//         // exist
//         if (STT[index][j].tag == tag && STT[index][j].valid)
//         {   
//             STT[index][j].pinned = false;
//             unpinctr++;
//             return;    
//         }        
//     }
//     // not exist ~~~> error
//     printf("ERROR: stt pin block %d not found!\n", addr);
//     exit(1);
// }

unsigned int stt_index(unsigned int addr){
    unsigned int index = addr % STT_SET;
    return index;

}

unsigned int stt_tag(unsigned int addr){
    return addr;
}


// return true on hit and false on miss
bool stt_access(unsigned int addr){
    unsigned int index = stt_index(addr);
    unsigned int tag = stt_tag(addr);
    

    for (unsigned int j = 0; j < STT_WAY; j++)
    {
        // hit
        if (STT[index][j].tag == tag && STT[index][j].valid)
        {   if (STT_WAY != 1)
            {
                update_ts(index, j);  
            }
            return true;    
        }        
    }
    // miss
    return false;
}

bool stt_contain(unsigned int addr){
    unsigned int index = stt_index(addr);
    unsigned int tag = stt_tag(addr);

    for (unsigned int j = 0; j < STT_WAY; j++)
    {
        // hit
        if (STT[index][j].tag == tag && STT[index][j].valid)
        {   
            return true;    
        }        
    }
    // miss
    return false;
}



// try to fill the stt with new data, it may lead to eviction ~~~> is called when miss happens
bool stt_fill(unsigned int addr){
    unsigned int index = stt_index(addr);
    unsigned int tag = stt_tag(addr);

    // int victim = -1;
   
    // miss only
    int way = stt_find_spot(index);

    // miss & evict
    // if (way == -1)
    // {
    //     way = stt_find_victim(index);
    //     victim = STT[index][way].tag;
    // }


     if (way < 0)
    {
        // printf("ERROR: stt fill way: %d   index %d\n", way, index);
        // exit(1);
        return false;
    }

    // sttline fill
    STT[index][way].valid = true;
    STT[index][way].tag = tag;


    update_ts(index, way);  

    return true;  
}


int stt_candidate(int label, int level){
    int mask = label>>(LEVEL-1-level);
    for (int i = 0; i < STT_SET; i++)
    {
        for (int j = 0; j < STT_WAY; j++)
        {
            if (STT[i][j].valid)
            {
                int target = PosMap[STT[i][j].tag];
                target = target>>(LEVEL-1-level);       
                if (((target)^mask) == 0) 
                {
                   STT[i][j].valid = false;
                   return STT[i][j].tag; 
                }
            }
        }
    }
    
    return -1;
}

void stt_print(int addr){
    unsigned int index = stt_index(addr);
    unsigned int tag = stt_tag(addr);

    for (unsigned int j = 0; j < STT_WAY; j++)
    {
        // hit
        if (STT[index][j].tag == tag && STT[index][j].valid)
        {   
            printf("\n\naddr: %d   STT: %lld\n", addr, STT[index][j].lru);
        }        
    }
}

void stt_test(){

    stt_init();
    int base = 100;
    int index = base % STT_SET;
    for (int i = 0; i < STT_WAY; i++)
    {
        stt_fill(base+i*STT_SET);
    }

    for (int i = 0; i < STT_WAY; i++)
    {
        printf("STT[%d][%d]: %d     LRU: %lld\n", index, i, STT[index][i].tag, STT[index][i].lru);
    }

    for (int i = 0; i < STT_WAY; i++)
    {
        for (int j = STT_WAY-1; j > i; j--)
        {
            stt_access(base+i*STT_SET);
        }
        
    }
    printf("\n");

    for (int i = 0; i < STT_WAY; i++)
    {
        printf("STT[%d][%d]: %d     LRU: %lld\n", index, i, STT[index][i].tag, STT[index][i].lru);
    }

    int victim = stt_fill(base+STT_WAY*STT_SET);
    printf("victim: %d\n", victim);
    
    
}
//  Mehrnoosh.