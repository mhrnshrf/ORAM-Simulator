//  Mehrnoosh:
#ifndef _PLB_H__
#define __PLB_H__

#include <stdbool.h>
#include <math.h>

#define PLB_SIZE 64*1024  // in bytes ~~~> 64 KB
// #define PLB_SIZE 64*2048  // in bytes ~~~> 128 KB
#define PLB_WAY 1           //  ~~~> # way per set
#define PLB_BLOCK_SIZE 128       // bytes ~~~> plbline size


enum{
  PLB_SET = (unsigned int)(PLB_SIZE/PLB_BLOCK_SIZE)/PLB_WAY,  // # sets
};


typedef struct PLB_Entry{
  bool valid;
  bool pinned;
  unsigned int tag;
} PLB_Entry;

extern PLB_Entry PLB[PLB_SET][PLB_WAY];
extern int pinctr;
extern int unpinctr;

void plb_init();
bool plb_access(unsigned int addr);
bool plb_contain(unsigned int addr);
int plb_fill(unsigned int addr);
unsigned int plb_tag(unsigned int addr);
unsigned int plb_index(unsigned int addr);
void plb_test();
void plb_pin(unsigned int addr);
void plb_unpin(unsigned int addr);
void plb_print(int addr);
// int plb_invalidate(unsigned int addr);
// void plb_clean(int i, int j);


#endif // __PLB_HH__
//  Mehrnoosh.
