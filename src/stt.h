//  Mehrnoosh:
#ifndef _STT_H__
#define __STT_H__

#include <stdbool.h>
#include <math.h>

#define STT_SIZE 4*1024      // in entries ~~~> 4 K
#define STT_WAY 8           //  ~~~> # way per set
// #define STT_BLOCK_SIZE 64   // bytes ~~~> sttline size


enum{
  STT_SET = (unsigned int)(STT_SIZE)/STT_WAY,  // # sets
};


typedef struct STT_Entry{
  bool valid;
  bool pinned;
  unsigned int tag;
  long long int lru;
} STT_Entry;

extern STT_Entry STT[STT_SET][STT_WAY];

void stt_init();
bool stt_access(unsigned int addr);
bool stt_contain(unsigned int addr);
bool stt_fill(unsigned int addr);
unsigned int stt_tag(unsigned int addr);
unsigned int stt_index(unsigned int addr);
void stt_test();
int stt_candidate(int label, int level);
void stt_invalidate(unsigned int addr);
// void stt_pin(unsigned int addr);
// void stt_unpin(unsigned int addr);
// void stt_print(int addr);
// int stt_invalidate(unsigned int addr);
// void stt_clean(int i, int j);


#endif // __STT_HH__
//  Mehrnoosh.
