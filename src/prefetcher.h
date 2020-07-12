//  Mehrnoosh:
#ifndef _PREFETCHER_H__
#define __PREFETCHER_H__

#include <stdbool.h>
#include <math.h>

// #define PREFETCHER_SIZE 262144  // in bytes ~~~> 256 KB
#define HISTORY_TABLE_ENTRY 16*1024  // # entries of history table
#define NUM_WAY 16                   //  ~~~> # way per set
#define BLOCK_SIZE 64                // bytes ~~~> tableline size
#define ADDR_WIDTH 32                // bits

typedef enum {PC, ADDR, OFFSET, PC_ADDR, PC_OFFSET} MatchType;

#define INDEX_TYPE PC
#define TAG_TYPE PC_ADDR

enum{
  NUM_SET = (unsigned int)(HISTORY_TABLE_ENTRY)/NUM_WAY,  // # sets
  INDEX_WIDTH = (unsigned int)log2(NUM_SET), 
  // OFFSET_WIDTH = (unsigned int)log2(BLOCK_SIZE),
  OFFSET_WIDTH = 0,
  TAG_WIDTH = (unsigned int)ADDR_WIDTH - INDEX_WIDTH - OFFSET_WIDTH,
};

typedef struct Event{
    unsigned int pc;
    unsigned int addr;
    char offset;
}Event; 


typedef struct HistEntry{
  // bool dirty;
  // unsigned int tag;        
  bool valid;
  Event tag;          // the tuple of events that is the key to lookup history table, it can be pc+addr, pc+offset or only pc
  unsigned int addr;  // the candidate address to prefetch
} HistEntry;


void table_init();
bool table_access(Event e);
void table_fill(Event e, unsigned int candidate);
unsigned int get_tag(unsigned int addr);
unsigned int get_index(Event e);


#endif // __PREFETCHER_HH__
//  Mehrnoosh.
