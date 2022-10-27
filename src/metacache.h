//  Mehrnoosh:
#ifndef _METACACHE_H__
#define __METACACHE_H__

#include <stdbool.h>
#include <math.h>

#define METACACHE_SIZE 2097152  // in bytes ~~~> 2 MB
// #define METACACHE_SIZE 4194304  // in bytes ~~~> 4 MB
// #define METACACHE_SIZE 1048576     // in bytes ~~~> 1 MB
// #define METACACHE_SIZE 262144  // in bytes ~~~> 256 KB
// #define METACACHE_SIZE 524288  // in bytes ~~~> 512 KB
#define META_NUM_WAY 16           //  ~~~> # way per set
// #define META_NUM_WAY 8           //  ~~~> # way per set
#define META_BLOCK_SIZE 64       // bytes ~~~> cacheline size
#define META_ADDR_WIDTH 32       // bits

enum meta_reqType {MCREAD = 'R', MCWRITE = 'W'};
// enum meta_status {MISS = false, HIT = true};
// enum meta_eviction{NO_EVICT = -1};

enum{
  META_NUM_SET = (unsigned int)(METACACHE_SIZE/META_BLOCK_SIZE)/META_NUM_WAY,  // # sets
  META_INDEX_WIDTH = (unsigned int)log2(META_NUM_SET), 
  // META_OFFSET_WIDTH = (unsigned int)log2(META_BLOCK_SIZE),
  META_OFFSET_WIDTH = 0,
  META_TAG_WIDTH = (unsigned int)META_ADDR_WIDTH - META_INDEX_WIDTH - META_OFFSET_WIDTH,
};


typedef struct MetaCacheline{
  bool valid;
  bool dirty;
  unsigned int tag;
  unsigned int addr;
  long long int LRU;
} MetaCacheline;

extern MetaCacheline METACACHE[META_NUM_SET][META_NUM_WAY];
extern int metacache_dirty;
extern int meta_dirty_coor[2];

void metacache_init();
bool metacache_access(unsigned int addr, char type);
int metacache_fill(unsigned int addr,  char type);
unsigned int meta_get_tag(unsigned int addr);
unsigned int meta_get_index(unsigned int addr);
int metacache_invalidate(unsigned int addr);
void metacache_clean(int i, int j);


#endif // __METACACHE_HH__
//  Mehrnoosh.
