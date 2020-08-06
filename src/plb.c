//  Mehrnoosh:

#include "plb.h"
#include <stdio.h>
#include <stdlib.h>




PLB_Entry PLB[PLB_SET][PLB_WAY];     // the last level plb
char REP[PLB_SET][PLB_WAY];          // an array to keep track of lru for eviction

// invalidate all plb blocks upon init
void plb_init(){
    for (unsigned int i = 0; i < PLB_SET; i++)
    {
        for (unsigned int j = 0; j < PLB_WAY; j++)
        {
            PLB[i][j].valid = false;
            
            REP[i][j] = 0;
        }
    }
}

void update_REP(unsigned int index, unsigned int way){
    if (REP[index][way] >= PLB_WAY - 1)
    {
        REP[index][way] = 0;
    }
    else
    {
        REP[index][way]++;
    } 
}

void reset_REP(unsigned int index, unsigned int way){
    REP[index][way] = 1;

}


int plb_find_spot(unsigned int index){
    for (int j = 0; j < PLB_WAY; j++)
    {
        if (!PLB[index][j].valid)
        {
            return j;
        }
    }
    return -1;  
}


// find the plbline with the least recently used
int plb_find_victim(unsigned int index) {
    int victim = -1;
    char min = PLB_WAY+1;
    for (int j = 0; j < PLB_WAY; j++)
    {
        if (REP[index][j] < min)
        {
            victim = j;
            min = REP[index][j];
        }
    }

    return victim;
}



unsigned int plb_index(unsigned int addr){
    unsigned int index = addr % PLB_SET;
    return index;

}

unsigned int plb_tag(unsigned int addr){
    return addr;
}


// return true on hit and false on miss
bool plb_access(unsigned int addr){
    unsigned int index = plb_index(addr);
    unsigned int tag = plb_tag(addr);

    for (unsigned int j = 0; j < PLB_WAY; j++)
    {
        // hit
        if (PLB[index][j].tag == tag && PLB[index][j].valid)
        {   
            update_REP(index, j);  
            return true;    
        }        
    }
    // miss
    return false;
}

bool plb_contain(unsigned int addr){
    unsigned int index = plb_index(addr);
    unsigned int tag = plb_tag(addr);

    for (unsigned int j = 0; j < PLB_WAY; j++)
    {
        // hit
        if (PLB[index][j].tag == tag && PLB[index][j].valid)
        {   
            return true;    
        }        
    }
    // miss
    return false;
}

// int plb_invalidate(unsigned int addr){
//     unsigned int index = plb_index(addr);
//     unsigned int tag = plb_tag(addr);

//     for (unsigned int j = 0; j < PLB_WAY; j++)
//     {
//         // hit
//         if (PLB[index][j].tag == tag && PLB[index][j].valid)
//         {   
//             PLB[index][j].valid = false;
//             PLB[index][j].dirty = false;
//             plb_dirty--;
//             return  PLB[index][j].addr;    
//         }        
//     }
//     // miss
//     return -1;
// }

// void plb_clean(int i, int j){
//     PLB[i][j].dirty = false;
// }

// try to fill the plb with new data, it may lead to eviction ~~~> is called when miss happens
int plb_fill(unsigned int addr){
    unsigned int index = plb_index(addr);
    unsigned int tag = plb_tag(addr);

    int victim = -1;
   
    // miss only
    int way = plb_find_spot(index);

    // miss & evict
    if (way == -1)
    {
        way = plb_find_victim(index);
        victim = PLB[index][way].tag;
    }


     if (way < 0)
    {
        printf("ERROR: plb fill way: %d   index %d\n", way, index);
        exit(1);
    }

    // plbline fill
    PLB[index][way].valid = true;
    PLB[index][way].tag = tag;


    reset_REP(index, way);  

    return victim;  
}

//  Mehrnoosh.