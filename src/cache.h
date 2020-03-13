//  Mehrnoosh:
#ifndef _CACHE_H__
#define __CACHE_H__

#include <stdbool.h>
#include <math.h>

#define CACHE_SIZE 2097152  // in bytes ~~~> 2 MB
#define NUM_WAY 8           // bytes ~~~> # way per set
#define BLOCK_SIZE 64       // bytes ~~~> cacheline size
#define ADDR_WIDTH 32       // bits

enum reqlkdgdjffn {CREAD = 'R', CWRITE = 'W'};
enum status {MISS = false, HIT = true};
enum eviction{NO_EVICT = -1};


enum{
  NUM_SET = (unsigned int)(CACHE_SIZE/BLOCK_SIZE)/NUM_WAY,  // # sets
  INDEX_WIDTH = (unsigned int)log2(NUM_SET), 
  OFFSET_WIDTH = (unsigned int)log2(BLOCK_SIZE),
  TAG_WIDTH = (unsigned int)ADDR_WIDTH - INDEX_WIDTH - OFFSET_WIDTH,
};


typedef struct Cacheline{
  bool valid;
  bool dirty;
  unsigned int tag;
  unsigned int addr;
} Cacheline;


void cache_init();
bool cache_access(unsigned int addr, char type);
unsigned int cache_fill(unsigned int addr,  char type);
unsigned int get_tag(unsigned int addr);
unsigned int get_index(unsigned int addr);


#endif // __CACHE_HH__
//  Mehrnoosh.
