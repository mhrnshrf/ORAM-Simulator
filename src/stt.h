//  Mehrnoosh:
#ifndef _STT_H__
#define __STT_H__

#include <stdbool.h>
#include <math.h>

#define STT_SIZE 64*1024  // in bytes ~~~> 64 KB
#define STT_WAY 1           //  ~~~> # way per set
#define STT_BLOCK_SIZE 64       // bytes ~~~> sttline size


enum{
  STT_SET = (unsigned int)(STT_SIZE/STT_BLOCK_SIZE)/STT_WAY,  // # sets
};


typedef struct STT_Entry{
  bool valid;
  bool pinned;
  unsigned int tag;
} STT_Entry;

extern STT_Entry STT[STT_SET][STT_WAY];
extern int pinctr;
extern int unpinctr;

void stt_init();
bool stt_access(unsigned int addr);
bool stt_contain(unsigned int addr);
int stt_fill(unsigned int addr);
unsigned int stt_tag(unsigned int addr);
unsigned int stt_index(unsigned int addr);
void stt_test();
void stt_pin(unsigned int addr);
void stt_unpin(unsigned int addr);
void stt_print(int addr);
// int stt_invalidate(unsigned int addr);
// void stt_clean(int i, int j);


#endif // __STT_HH__
//  Mehrnoosh.
