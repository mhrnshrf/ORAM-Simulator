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
            PLB[i][j].pinned = false;
            
            REP[i][j] = 0;
        }
    }
}

void update_REP(unsigned int index, unsigned int way){

    if (REP[index][way] >= PLB_WAY-1 )
    {
        if (!PLB[index][way].pinned)
        {
            REP[index][way] = 0;
        }
        
    }
    else
    {
        if (!PLB[index][way].pinned)
        {
            REP[index][way]++;
        }
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
    if (PLB_WAY == 1)
    {
        return 0;
    }
    
    int victim = -1;
    char min = PLB_WAY;
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

void plb_pin(unsigned int addr){
    unsigned int index = plb_index(addr);
    unsigned int tag = plb_tag(addr);
    

    for (unsigned int j = 0; j < PLB_WAY; j++)
    {
        // exist
        if (PLB[index][j].tag == tag && PLB[index][j].valid)
        {   
            REP[index][j] = PLB_WAY;
            PLB[index][j].pinned = true;
            return;    
        }        
    }
    // not exist ~~~> error
    printf("ERROR: plb pin block %d not found!\n", addr);
    exit(1);
}


void plb_unpin(unsigned int addr){
    unsigned int index = plb_index(addr);
    unsigned int tag = plb_tag(addr);
    

    for (unsigned int j = 0; j < PLB_WAY; j++)
    {
        // exist
        if (PLB[index][j].tag == tag && PLB[index][j].valid)
        {   
            PLB[index][j].pinned = false;
            update_REP(index, j);
            return;    
        }        
    }
    // not exist ~~~> error
    printf("ERROR: plb pin block %d not found!\n", addr);
    exit(1);
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
        {   if (PLB_WAY != 1)
            {
                update_REP(index, j);  
            }
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



void plb_test(){

    plb_init();
    int base = 100;
    int index = base % PLB_SET;
    for (int i = 0; i < PLB_WAY; i++)
    {
        plb_fill(base+i*PLB_SET);
    }

    for (int i = 0; i < PLB_WAY; i++)
    {
        printf("PLB[%d][%d]: %d     LRU: %d\n", index, i, PLB[index][i].tag, REP[index][i]);
    }

    for (int i = 0; i < PLB_WAY; i++)
    {
        for (int j = PLB_WAY-1; j > i; j--)
        {
            plb_access(base+i*PLB_SET);
        }
        
    }
    printf("\n");

    for (int i = 0; i < PLB_WAY; i++)
    {
        printf("PLB[%d][%d]: %d     LRU: %d\n", index, i, PLB[index][i].tag, REP[index][i]);
    }

    int victim = plb_fill(base+PLB_WAY*PLB_SET);
    printf("victim: %d\n", victim);
    
    
}
//  Mehrnoosh.