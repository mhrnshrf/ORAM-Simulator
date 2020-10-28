#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "utlist.h"

#include "utils.h"

#include "params.h"
#include "memory_controller.h"
#include "scheduler.h"
#include "processor.h"


// Current Processor Cycle
// extern long long int CYCLE_VAL;
extern long long int trace_clk;

/////////////// Mehrnoosh:
#include <stdbool.h>
#include <math.h>
#include <sys/time.h>
#include <string.h> 
#include "cache.h"
#include "prefetcher.h"
#include "plb.h"
#include "stt.h"

long long int ring_G = 0;
long long int ring_round = 0;
long long int ep_round = 0;
long long int touchcount = 0;

// long long int CYCLE_VAL = 0;

double cpu_time_used = 0;


long long int cache_clk = 0;

int hitctr = 0;		// # hits on llc
int missctr = 0;	// # misses on llc
long long int mem_clk = 0;

long long int orig_addr;
long long int orig_cycle; 
int orig_thread; 
int orig_instr; 
long long int orig_pc;
int oram_acc_addr;

typedef struct Slot{
  bool isData;     // Data block 1  , PosMap block 0
  bool isReal;       // real 1 , dummy 0
  int addr;         // address of block e.g. A, B
  int label;      // path label e.g. 0 (=> L0)
  bool valid;     // added for ring oram
}Slot;

// typedef struct Bucket{
//   Slot *slot;
// }Bucket;

typedef struct Bucket{
  Slot slot[Z];
  char count;  // added for ring oram
  char dumnum; // added for ring oram, # dummy slots available
}Bucket;

typedef struct Entry{
  int addr;       // address of block e.g. A, B
  int label;      // path label e.g. 0 (=> L0)
}Entry;

typedef struct EntryBuf{
  int addr;         // address of the posmap block prefetched
  int timestamp;    // a timestamp to prioritize in prefetch buffer when eviction needed
  bool valid;
  PosType type;
}EntryBuf;


Queue *oramQ;
Queue *plbQ;
Queue *pathQ;

int revarr[RING_REV];



Bucket GlobTree[NODE];      // global oram tree
int PosMap[BLOCK];          // position map
Slot Stash[STASH_SIZE];     // stash
// int PLB[PLB_SIZE] = {[0 ... PLB_SIZE-1] = -1};   // posmap lookaside buffer
EntryBuf PreBuffer[PREFETCH_BUF_SIZE]; // prefetch buffer
int SubMap[NODE];          // subtree address map

int intended = -1;         // index of intended block in stash
int intended_addr = -1;         // intended block in stash
bool pinFlag = false;     // a flag to indicate whether the intended block should be pinned to the stash or not 
int trace[TRACE_SIZE] = {0};    // array for pre-reading traces from a file
int candidate[Z] = {[0 ... Z-1] = -1};    // keep index of candidates in stash for write back to a specific node
bool write_cache_hit = false;
int curr_trace = -1; 	// the current trace address 
int next_trace = 0; 	// the current trace address 
int curr_page = 0; 	// the current page address 
long long int curr_pc = 0; 	// the current pc address 
char curr_offset = 0; 	// the current offset
unsigned long long int curr_footprint = 0; 	// the current footprint of accessed block of the current page 



void pinOn() {pinFlag = true;}    // turn the pin flag on
void pinOff() {pinFlag = false;}  // turn the pin flag off


// rho
typedef struct RhoBucket{
  Slot slot[RHO_Z];
}RhoBucket;

RhoBucket RhoTree[RHO_NODE];      // rho tree
int TagArray[RHO_SET][RHO_WAY];         // rho postion map that store tags ~~~> pararell to tag array label
int TagArrayLabel[RHO_SET][RHO_WAY];     // rho postion map that store the corresponding label~~~>  parallel to tag array
long long int TagArrayLRU[RHO_SET][RHO_WAY];      // rho lru policiy for eviction ~~~>  parallel to tag array
Slot RhoStash[RHO_STASH_SIZE];       // rho stash
int RhoSubMap[NODE];              // rho subtree address map


// profiling stats
int tracectr = 0; // # lines read from the trace file 
int tracectr_test = 0;  // # lines read from the trace file for testing without simulator
int prefetchctr = 0; // # prefetch access
int pos1ctr = 0; // # prefetch pos1
int pos2ctr = 0; // # prefetch pos2
int pos1acc_ctr = 0; // # accesses to prefetched pos1
int pos2acc_ctr = 0; // # accesses to prefetched pos2
int pos1hit = 0; // # hit on prefetch buffer for pos1
int pos2hit = 0; // # hit on prefetch buffer for pos2
int pos1conf = 0;
int pos2conf = 0;
int case1 = 0;
int case2 = 0;
int case3 = 0;
int plbpos1 = 0;
int stashpos1 = 0;
int bufferpos1 = 0;
int pos1_recent = 0;
int pos2_recent = 0;
int stashctr = 0; // # blocks in stash ~ stash occupancy
int bkctr = 0;  // # background eviction invoked
int invokectr = 0; // # memory requests coming from outside (# invokation of oram)
int oramctr = 0;  // # oram accesses
int stash_dist[STASH_SIZE+1] = {0}; // stash occupancy distribution

int rhoctr = 0;  // # rho accesses
int rho_stashctr = 0;  // stash occupancy distribution in rho
int rho_bkctr = 0;  // # background eviction invoked in rho
int rho_stash_dist[RHO_STASH_SIZE+1] = {0}; // rho stash occupancy distribution
int rho_hit = 0;  // # hits on rho lookup

int dummyctr = 0;
int rho_dummyctr = 0;
long long int topctr = 0;
long long int midctr = 0;
long long int botctr = 0;

struct timeval start, end, mid;
long int timeavg = 0;
long int timeavg_mid = 0;
long int duration = 0;
long int timesum = 0;

int cap_count[CAP_NODE] = {0};
int path_length = 0;
int sub_cap = 0;

int earlyctr = 0;
int evictctr = 0;	// # evictions caused after misses on llc
int pos1_access = 0;
int pos2_access = 0;
int ptr_fail = 0;
int search_fail = 0;
int precase = 0;
int sttctr = 0;
int stash_leftover = 0;
int stash_removed = 0;
int fillhit = 0;
int fillmiss = 0;
int ring_evictctr = 0;
int stash_cont = 0;
int linger_discard = 0;
int ringctr = 0;
int injcount = 0;
int wbctr = 0;


long long int plb_hit[H-1] = {0};   // # hits on a0, a1, a2, ...
long long int plbaccess[H-1] = {0};   // # total plb access (hits + misses)
int plb_evict[PLB_SIZE] = {[0 ... PLB_SIZE-1] = -1};  // array to keep address of blocks that are evicted from plb 
int plb_hist[PLB_SIZE] = {0};   // array to track the amount of reuse of blocks that are evicted from plb 
int plb_conflict[PLB_SIZE] = {0};   // array to track number of confilicts that happens on evictions
int plb_trace[PLB_SIZE] = {[0 ... PLB_SIZE-1] = -1}; 
int plb_interval[PLB_SIZE] =  {[0 ... PLB_SIZE-1] = -1}; 
int plb_temp[PLB_SIZE] =  {[0 ... PLB_SIZE-1] = -1}; 

int shuff[LEVEL] = {0};
int wb[LEVEL] = {0};

// these are constants used for oram alg, by defualt initialized to oram params unless the tree is switched to rho
TreeType TREE_VAR = ORAM;
int LEVEL_VAR = LEVEL;
int Z_VAR = Z;
int STASH_SIZE_VAR = STASH_SIZE;
int OV_THRESHOLD_VAR = OV_THRESHOLD;  
int BK_EVICTION_VAR = BK_EVICTION;
int EMPTY_TOP_VAR = EMPTY_TOP;
int TOP_CACHE_VAR = TOP_CACHE;
int LZ_VAR[LEVEL] = {[0 ... L1] = Z1, [L1+1 ... L2] = Z2, [L2+1 ... L3] = Z3, [L3+1 ... LEVEL-1] = Z4}; 
int PATH_VAR = PATH;
AccessType ACCESS_VAR = REGULAR;      // to indicate whether a block shoulb be remapped and written back to the path or it should be evicted entirly
EnqueueType ENQUEUE_VAR = TAIL;    // to indicate whether enqueue to oramq should be regularely added to the tail or head ~~~> head in case of dummy access 
PosType pos_var = POS2;
bool SIM_ENABLE_VAR = true;

int set_start = 0;
int way_start = 0;

int dirty_pointctr = 0;

int oram_effective_pl = 0;


unsigned int byte_addr(long long int physical_addr){
  unsigned int addr = (unsigned int)(physical_addr & (0x7fffffff));
  unsigned int max = ((BLOCK-1)<<((unsigned int)log2(BLOCK_SIZE))) | (BLOCK_SIZE-1);
  if (addr > max)
  {
    addr = max; 
  }
  return addr;
}


// convert byte address to block address, each block is 64 bytes 
// upper 32 bits are discarded and also oram memory address is cut into half because of utilization 50%
// unsigned int block_addr(long long int physical_addr){
//   unsigned int addr = (unsigned int)(physical_addr & (0xffffffff));
//   addr = addr >> (unsigned int) log2(BLOCK_SIZE);
//   addr = (addr & (BLOCK+1));
//   if (addr >= BLOCK)
//   {
//     addr = BLOCK - 1; 
//   }
  
//   return addr;
// }

unsigned int block_addr(unsigned int caddr){
  unsigned int addr = caddr >> (unsigned int) log2(BLOCK_SIZE);
  return addr;
}


// convert byte address to page address, each page is 4KB i.e 64 blocks
unsigned int page_addr(unsigned int physical_addr){
  unsigned int addr = block_addr(physical_addr);
  addr = addr >> (unsigned int) log2(PAGE_SIZE/BLOCK_SIZE);
  return addr;
}



unsigned int region_addr(unsigned int physical_addr){
  unsigned int addr = block_addr(physical_addr);
  addr = addr >> (unsigned int) log2((X*PAGE_SIZE)/BLOCK_SIZE);
  return addr;
}

unsigned int subregion_addr(unsigned int physical_addr){
  unsigned int addr = block_addr(physical_addr);
  addr = addr >> (unsigned int) log2((PAGE_SIZE/4)/BLOCK_SIZE);
  return addr;
}


char offset_val(unsigned int addr){
  unsigned int block = block_addr(addr);
  unsigned int mask = 0b111111;
  char val = block & mask;
  return val;
}

void footprint_update(unsigned int addr){
  unsigned int block = block_addr(addr);
  unsigned int mask = 0b111111;
  block = block & mask;
  curr_footprint = curr_footprint | (1<<block);
}


// void test_footprint(){
// 	long long int pa = 0x7fff219c90d7;
// 	printf("byte addr:    %llx\n", pa);
// 	printf("block addr:   %x\n", block_addr(pa));
// 	printf("page addr:    %x\n", page_addr(pa));
// 	printf("offset val:   %d\n", offset_val(pa));
// 	footprint_update(0x7fff219c90d8);
// 	footprint_update(0x7fff219c90d9);
// 	footprint_update(0x7fff219c90da);
// 	footprint_update(0x7fff219c90db);
// 	footprint_update(0x7fff219c90dc);
// 	footprint_update(0x7fff219c91dd);
// 	footprint_update(0x7fff219c92de);
// 	footprint_update(0x7fff219c93df);
// 	printf("footprint:    %llx\n", curr_footprint);
// }

Queue *ConstructQueue(int limit) {
    Queue *queue = (Queue*) malloc(sizeof (Queue));
    if (queue == NULL) {
        return NULL;
    }
    if (limit <= 0) {
        limit = 65535;
    }
    queue->limit = limit;
    queue->size = 0;
    queue->head = NULL;
    queue->tail = NULL;

    return queue;
}

void DestructQueue(Queue *queue) {
    Element *pN;
    while (!isEmpty(queue)) {
        pN = Dequeue(queue);
        free(pN);
    }
    free(queue);
}


bool Enqueue(Queue *pQueue, Element *item) {
    /* Bad parameter */
    if ((pQueue == NULL) || (item == NULL)) {
        return false;
    }
    // if(pQueue->limit != 0)
    if (pQueue->size >= pQueue->limit) {
        return false;
    }
    /*the queue is empty*/
    item->prev = NULL;
    if (pQueue->size == 0) {
        pQueue->head = item;
        pQueue->tail = item;

    } else {
        /*adding item to the end of the queue*/
        pQueue->tail->prev = item;
        pQueue->tail = item;
    }
    pQueue->size++;
    return true;
}

bool EnqueueHead(Queue *pQueue, Element *item) {
    /* Bad parameter */
    if ((pQueue == NULL) || (item == NULL)) {
        return false;
    }
    // if(pQueue->limit != 0)
    if (pQueue->size >= pQueue->limit) {
        return false;
    }
    /*the queue is empty*/
    item->prev = NULL;
    if (pQueue->size == 0) {
        pQueue->head = item;
        pQueue->tail = item;

    } else {
        /*adding item to the head of the queue*/
        item->prev = pQueue->head;
        pQueue->head = item;
    }
    pQueue->size++;
    return true;
}

Element * Dequeue(Queue *pQueue) {
    /*the queue is empty or bad param*/
    Element *item;
    if (isEmpty(pQueue))
        return NULL;
    item = pQueue->head;
    pQueue->head = (pQueue->head)->prev;
    pQueue->size--;
    return item;
}

bool isEmpty(Queue* pQueue) {
    if (pQueue == NULL) {
        return false;
    }
    if (pQueue->size == 0) {
        return true;
    } else {
        return false;
    }
}

void test_queue(){
  for (int i = 0; i < 10; i++) {
    Element *pN = (Element*) malloc(sizeof (Element));
    pN->addr = rand() % 500;
    pN->cycle = i;
    pN->thread = i;
    pN->instr = i;
    pN->pc = i;
    pN->type = (rand() % 2) ? 'R' : 'W';
    // Enqueue(oramQ, pN);
    if (ENQUEUE_VAR == TAIL)
    {
      Enqueue(oramQ, pN);
    }
    else if (ENQUEUE_VAR == HEAD){
      EnqueueHead(oramQ, pN);
    }
    
  }

  while (!isEmpty(oramQ)) {
      Element *pN = Dequeue(oramQ);
      printf("\nDequeued: %lld	%lld	%d	%d	%lld	%c \n", pN->addr, pN->cycle, pN->thread, pN->instr, pN->pc, pN->type);
      printf("queue size: %d\n", oramQ->size);
      free(pN);
  }
  DestructQueue(oramQ);

	exit(0);
}

void insert_oramQ(long long int addr, long long int cycle, int thread, int instr, long long int pc, char type) {
  addr = addr << (int)log2(BLOCK_SIZE);
  Element *pN = (Element*) malloc(sizeof (Element));
  pN->addr = addr;
  pN->cycle = cycle; 
  pN->thread = thread; 
  pN->instr = instr; 
  pN->pc = pc; 
  pN->type = type;
  pN->oramid = (TREE_VAR == RHO) ? rhoctr : oramctr;
  pN->oramid = (ENQUEUE_VAR == HEAD) ? -1 : pN->oramid;   // in case of dummy access set oram id to -1
  pN->tree = TREE_VAR;
  pN->orig_addr = orig_addr;
  bool added = false;
  if (ENQUEUE_VAR == TAIL)
  {
    added = Enqueue(oramQ, pN);
  }
  else if (ENQUEUE_VAR == HEAD)
  {
    added = EnqueueHead(oramQ, pN);
  }
  
  if (!added)
  {
    printf("ERROR: insert oramQ: failed to enqueue block: %lld    oramq size: %d    trace: %d  stash: %d bkctr: %d\n", addr, oramQ->size, tracectr, stashctr, bkctr);
    print_oram_stats();
    exit(1);
  }
}



void insert_plbQ(int addr){
  Element *pN = (Element*) malloc(sizeof (Element));
  pN->addr = addr;
  bool added = Enqueue(plbQ, pN);
  if (!added)
  {
    printf("ERROR: insert plbQ: failed to enqueue block: %d    plbq size: %d\n", addr, plbQ->size);
    print_oram_stats();
    exit(1);
  }

}

// to set the parameters for different funcs like path read and write according to whether oram is being accessed or rho
void switch_tree_to(TreeType tree){
  TREE_VAR = tree;
  LEVEL_VAR = (tree == RHO) ? RHO_LEVEL: LEVEL ;
  Z_VAR = (tree == RHO) ? RHO_Z: Z;
  STASH_SIZE_VAR = (tree == RHO) ? RHO_STASH_SIZE: STASH_SIZE;
  OV_THRESHOLD_VAR = (tree == RHO) ? RHO_OV_THRESHOLD: OV_THRESHOLD;
  BK_EVICTION_VAR = (tree == RHO) ? RHO_BK_EVICTION: BK_EVICTION;
  EMPTY_TOP_VAR = (tree == RHO) ? RHO_EMPTY_TOP: EMPTY_TOP; 
  TOP_CACHE_VAR = (tree == RHO) ? RHO_TOP_CACHE: TOP_CACHE;
  PATH_VAR = (tree == RHO) ? RHO_PATH: PATH;
  for (int i = 0; i < LEVEL_VAR; i++)
  {
    LZ_VAR[i] =  (tree == RHO) ? RHO_LZ[i] : LZ[i];
  }
}

void switch_access_to(AccessType access){
  ACCESS_VAR = access;
}

void switch_enqueue_to(EnqueueType enqueue){
  ENQUEUE_VAR = enqueue;
}


void switch_sim_enable_to(bool tf){
  SIM_ENABLE_VAR = tf;
}
/***********************
  Utility Functions
************************/

int calc_level(int index){  
  return floor(log_base2(index+1));
}


int  calc_index(int label,  int l){
  int sum = 0;
  int index = -1;
  for(int i = 0; i < l; i++)
    sum += pow(2,i);

  // int a = pow(2,LEVEL-1)/pow(2,l);
  index = sum + floor(label/pow(2,(LEVEL_VAR-l-1)));

  return index;
}


int digcount(int num)
{
    int count = 0;

    /* Calculate total digits */
    count = log2(num) + 1;
    if (num == 0)
    {
      count = 1;
    }
    
    return count;
}

int concat(int a, int b) { 

  int c;
  /*
  // decimal concat ~~~~> concat(4,13) will be 413
    char s1[32]; 
    char s2[32]; 
  
    // Convert both the integers to string 
    sprintf(s1, "%d", a); 
    sprintf(s2, "%d", b); 
  
    // Concatenate both strings 
    strcat(s1, s2); 
  
    // Convert the concatenated string 
    // to integer 
    c = atoi(s1); 
  */
 // binary concat ~~~~> concat(1,0) will be 2
  c = (a<<digcount(b) )| b;
    // return the formed integer 
    return c; 
} 

/********************************/

void oram_alloc(){

  for (int i = 0; i < LEVEL; i++)
	{
		path_length += LZ[i];
	}

  for (int i = CAP_LEVEL; i < LEVEL; i++)
  {
    sub_cap += LZ[i]*pow(2,i-(CAP_LEVEL));
    
  }
  
  for (int i = 0 ; i < NODE; i++) 
  {
    int l = calc_level(i);  
     GlobTree[i].count = 0;
    for (int k = 0; k < LZ[l]; ++k)
    {
      GlobTree[i].slot[k].addr = -1;
      GlobTree[i].slot[k].label = -1;
      GlobTree[i].slot[k].isReal = false;
      GlobTree[i].slot[k].isData = false;
      GlobTree[i].slot[k].valid = true;  // ??? to be revised
      GlobTree[i].dumnum++;
    }
  }

  for (int i = 0; i < STASH_SIZE; i++)
  {
    Stash[i].isReal = false;
  } 

  for (int i = 0; i < PREFETCH_BUF_SIZE; i++)
  {
    PreBuffer[i].valid = false;
  }  

  oramQ = ConstructQueue(QUEUE_SIZE);
  plbQ = ConstructQueue(128);
  pathQ = ConstructQueue(RING_A);

  for (int i = 0; i < RING_REV; i++)
  {
    revarr[i] = reverse_lex(i);
  }
  

}

// initialize the oram tree by assigning a random path to each addr of address space
void oram_init(){
  for(int i = 0; i < BLOCK; i++)
  {
    // printf("i: %d\n", i);
    PosMap[i] =  assign_a_path(i);
  }

  // initialize subtree addressing for oram tree
  for (int i = 0; i < NODE; i++)
  {
    SubMap[i] = index_to_addr(i);
  }

  // initialize subtree addressing for rho tree
  switch_tree_to(RHO);
  for (int i = 0; i < RHO_NODE; i++)
  {
    RhoSubMap[i] = index_to_addr(i);
  }
  switch_tree_to(ORAM);
  
}


void oram_init_path(){
  switch_sim_enable_to(false);

  for(int i = 0; i < BLOCK; i++)
  {
    int label = rand() % PATH;
    PosMap[i] = label;
    Slot s = {.addr = i , .label = PosMap[i], .isReal = true, .isData = false};
    add_to_stash(s);

    read_path(label);
    remap_block(i);
    write_path(label);

    while (BK_EVICTION && bk_evict_needed())
    {
      // printf("\nbefore bk evict @ stash %d\n", stashctr);
      int randpath = rand() % PATH;
      read_path(randpath);
      write_path(randpath);
      // printf("after bk evict @ stash %d\n", stashctr);
    } 

  }



  //   // initialize subtree addressing for oram tree
  // for (int i = 0; i < NODE; i++)
  // {
  //   SubMap[i] = index_to_addr(i);
  // }

  // // initialize subtree addressing for rho tree
  // switch_tree_to(RHO);
  // for (int i = 0; i < RHO_NODE; i++)
  // {
  //   RhoSubMap[i] = index_to_addr(i);
  // }
  // switch_tree_to(ORAM);

  switch_sim_enable_to(true);
  printf("oram init path done.\n");
  fflush(stdout);
}

void print_count_level(){
  printf("\n");
  for (int l = 0; l < LEVEL; l++)
  {
    int counter = 0;
    int index = calc_index(0, l);
    for (int i = 0; i < pow(2, l); i++)
    {
      for (int j = 0; j < LZ[l]; j++)
      {
        if (GlobTree[index+i].slot[j].isReal)
        {
          counter++;
        }        
      }
      
    }   
    printf("%f\n", counter/(LZ[l]*pow(2,l)));
  }
  
}

// read trace file in fill in the trace array 
void init_trace(){
  for (int i = 0; i < TRACE_SIZE; i++)
  {
    FILE *fp;
    char c[100];
    fp = fopen("trace.txt","r");
    fscanf(fp,"%[^\n]", c);

    int i = 0;
    while (fgets(c, 100, fp) != NULL)
    {
      // printf("%s", c);
      int addr;
      sscanf(c, "%d", &addr);
      if (addr > pow(2,24) -1)
      {
        c[0] = '0';
        c[1] = '0';
        c[2] = '0';
        sscanf(c, "%d", &addr);
      }
      trace[i] = addr;
      i++;
      if (i == TRACE_SIZE)
      {
        break;
      }
      
    }
    fclose(fp);
    
  }
  

}


// print glob tree structure
void print_tree(){
    int prev = -1;
    int curr = 0;
    for(int i = 0; i < NODE; i++ )
    {
        curr = calc_level(i);
        if(curr != prev)
        {
          printf("\n");
          for(int l = 0; l < pow(2, (LEVEL-curr)) ; l++)
            printf("     ");

        }

        printf("   ");
        for (int j = 0; j < LZ[curr]; j++)
        {
         printf("%d:%d, ", GlobTree[i].slot[j].addr, GlobTree[i].slot[j].label);
        }
        printf("   ");
        

        prev = curr;

    }
    printf("\n");
}


// assign a random path to a data block
int assign_a_path(int addr){
  int label = label = rand() % PATH;
  

  while(true)
  {
    // printf("while true: addr: %d\n", addr);
    // while (label == RL)
      
    
    for(int i = LEVEL-1; i >= EMPTY_TOP; i--)
    {
      int index = calc_index(label, i);
      for(int j = 0; j < LZ[i]; j++)
      {
        if(!GlobTree[index].slot[j].isReal)
        {
          if (!RING_ENABLE || GlobTree[index].dumnum > LS[i])
          {
            GlobTree[index].slot[j].addr = addr;
            GlobTree[index].slot[j].label = label;
            GlobTree[index].slot[j].isReal = true;
            GlobTree[index].slot[j].isData = true;

            GlobTree[index].dumnum--;

            cap_count[calc_index(label, CAP_LEVEL)-CAP_NODE+1]++;
            return label;
          }
        }
      }
    }
  }
}

// lookup one entry of posmap proof check with globmap
void test_init(){
  FILE *fp;
  fp = fopen("check.txt","w");
  int addr = 1;
  int label = PosMap[1];
  fprintf(fp,"PosMap: %d, %d\n", addr, label);

  for(int i = LEVEL-1; i >= EMPTY_TOP; i--)
  {
    int index = calc_index(label, i);
    for(int j = 0; j < LZ[i]; j++)
    {
      if(GlobTree[index].slot[j].addr == addr)
      {
        fprintf(fp,"GlobTree: %d, %d\n", GlobTree[index].slot[j].addr, GlobTree[index].slot[j].label);
        fclose(fp);
        return;
      }
    }
  }
  
  
}

void reset_candidate(){
  for (int i = 0; i < Z; i++)
  {
    candidate[i] = -1;
  }
}

void count_tree(){
  
  int sum = 0;

  for (int i = 0; i < NODE; i++)
  {
    for (int j = 0; j < LZ[i]; j++)
    {
      if(GlobTree[i].slot[j].isReal)
      {
        sum++;
      }
    }
  }

  printf("count tree: counted block: %d,  BLOCK: %d ~> they match? %s \n", sum, BLOCK, (sum == BLOCK)?"yes!":"no!");
  
}

void read_path(int label){

    // printf("\nread path @ trace %d\n", tracectr);
    int gi = -1;

    for(int i = LEVEL_VAR-1; i >= EMPTY_TOP_VAR; i--)
    {
      // printf("\nread path %d level %d\n", label, i);
      // print_path(0);
      int index = calc_index(label, i);
      // if (RING_ENABLE)
      // {
        int reqmade = 0;
        int dum_cand[Z] = {0};
        int cand_ind = 0;
      // }
      
      for(int j = 0; j < LZ_VAR[i]; j++)
      {
        // printf("j: %d \n", j);
        gi++;
        if (i >= TOP_CACHE_VAR && SIM_ENABLE_VAR)
        {
          if(!RING_ENABLE || GlobTree[index].slot[j].isReal)
          {
            int  addr = (!SUBTREE_ENABLE) ? (index*Z_VAR+j): (TREE_VAR == ORAM)? SubMap[index]+j : RhoSubMap[index]+j;
            if (TREE_VAR == ORAM && STL_ENABLE && SUBTREE_ENABLE && NUM_CHANNELS_SUBTREE >  1)
            {
              int gi_prime = gi + (LEVEL-TOP_CACHE)*Z - oram_effective_pl;
              int i_prime = floor(gi_prime/Z) + 1 + L1;
              int j_prime = gi_prime % Z;
              int index_prime = calc_index(label, i_prime);
              addr = SubMap[index_prime] + j_prime;
            }
            
            insert_oramQ(addr, orig_cycle, orig_thread, orig_instr, orig_pc, 'R');
            reqmade++;
            // printf("%d: slot %d accessed ~> real? %s\n", reqmade, j,  GlobTree[index].slot[j].isReal?"yes":"no");
          }
        }

        if (RHO_ENABLE && (TREE_VAR == RHO))
        {
          if(RhoTree[index].slot[j].isReal)
          {
            
            if(add_to_stash(RhoTree[index].slot[j]) != -1)
            {
              RhoTree[index].slot[j].isReal = false;
              RhoTree[index].slot[j].addr = -1;
              RhoTree[index].slot[j].label = -1;
            }
            else
            {
              printf("ERROR: read: rho stash overflow!  @ %d\n", rho_stashctr);
              print_oram_stats();
              exit(1);
            }
          }
        }
        else
        {
          if(GlobTree[index].slot[j].isReal)
          {
            if (GlobTree[index].slot[j].addr == oram_acc_addr)
            {
              if (i <= TOP_BOUNDRY)
              {
                topctr++;
              }
              else if (i <= MID_BOUNDRY)
              {
                midctr++;
              }
              else
              {
                botctr++;
              }
            }
            
            
            if(add_to_stash(GlobTree[index].slot[j]) != -1)
            {
              GlobTree[index].slot[j].isReal = false;
              GlobTree[index].slot[j].addr = -1;
              GlobTree[index].slot[j].label = -1;
              cap_count[calc_index(label, CAP_LEVEL)-CAP_NODE+1]--;
              GlobTree[index].dumnum++;
            }
            else
            {
              printf("ERROR: read: trace %d stash overflow!  @ %d\n", tracectr, stashctr);
              printf("stash leftover %d\n", stash_leftover);
              printf("stash removed  %d\n", stash_removed);
              printf("fill hit  %d\n", fillhit);
              printf("fill miss  %d\n", fillmiss);
              print_oram_stats();
              exit(1);
            }
          }
          else if(GlobTree[index].slot[j].valid)
          {
            // printf("\ncand ind %d\n", cand_ind);
            dum_cand[cand_ind] = j;
            // printf("dum cand %d\n", dum_cand[cand_ind]);
            cand_ind++;
          }
        }
        
      }
      if (RING_ENABLE && i >= TOP_CACHE_VAR && SIM_ENABLE_VAR)
      {
        for (int k = 0; k < RING_Z-reqmade; k++)
        {
          int ri = -1;
          int sd = -1;
          while (sd == -1)
          {
            ri = rand() % cand_ind;
            sd = dum_cand[ri];
            // printf("level %d   count %d\n", i, GlobTree[index].count);
          }
          
          int mem_addr = index*Z_VAR + sd;
          insert_oramQ(mem_addr, orig_cycle, orig_thread, orig_instr, orig_pc, 'R');
          // printf("%d: slot %d accessed ~> dummy? %s\n", k, sd, GlobTree[index].slot[sd].isReal?"no":"yes");
          dum_cand[ri] = -1;
        }
      }
    }

}

// at each bucket look entire stash once and pick candidates for LZ[i] number of slots that the bucket has
void pick_candidate(int index, int label, int i){
  int c = 0;
  int mask = label>>(LEVEL_VAR-1-i);
  for(int k= 0; k < STASH_SIZE_VAR; k++)
  {
    bool spot_real = (RHO_ENABLE && (TREE_VAR == RHO))? RhoStash[k].isReal : Stash[k].isReal;
    if (spot_real)
    {
      int stash_label = (RHO_ENABLE && (TREE_VAR == RHO))? RhoStash[k].label : Stash[k].label;
      int target = stash_label>>(LEVEL_VAR-1-i);
      if(/* (Stash[k].label == label || index == calc_index(Stash[k].label, i)) */ (((target)^mask) == 0) && (!pinFlag || k != intended || (RHO_ENABLE && (TREE_VAR == RHO))))
      {
        candidate[c] = k;
        c++; 
        if (c == Z_VAR)
        {
          return;
        }
      }
    }
  }
}

void write_path(int label){

    // printf("\nwrite path @ trace %d\n", tracectr);
  
  int gi = -1;
  
  for(int i = LEVEL_VAR-1; i >= EMPTY_TOP_VAR; i--)
  {
    int index = calc_index(label, i);
    int addr = 0;
    GlobTree[index].count = 0; // for ring oram evict path
    
    // int stashctr_var = (RHO_ENABLE && (TREE_VAR == RHO))? rho_stashctr : stashctr;
    // if (stashctr_var == 0)
    // {
    //   if (RHO_ENABLE && (TREE_VAR == RHO))
    //   {
    //     // printf("write path: @ LEVEL: %d   stash got empty\n\n", i);
    //   }
      
    //   for (int g = 0; g < LZ_VAR[i]; g++)
    //   {
    //     if (i >= TOP_CACHE_VAR)
    //     {
    //       // addr = SUBTREE_ENABLE ?  SubMap[index]+g:(index*Z_VAR+g);
    //       addr = (!SUBTREE_ENABLE) ? (index*Z_VAR+g): (TREE_VAR == ORAM)? SubMap[index]+g : RhoSubMap[index]+g;

    //       // insert_write (addr, orig_cycle, orig_thread, orig_instr);
    //       insert_oramQ(addr, orig_cycle, orig_thread, orig_instr, 0, 'W');
    //       // printf("insert oramq: %d\n", oramQ->size);

    //     }
        
    //   }
    // }
    // else
    // {
      reset_candidate();
      pick_candidate(index, label, i);
      int stt_cand = -1;

      for(int j = 0; j < LZ_VAR[i]; j++)
      {
         GlobTree[index].slot[j].valid = true;  // added for ring oram

        gi++;
        if (i >= TOP_CACHE_VAR  && SIM_ENABLE_VAR)
        {
          addr = (!SUBTREE_ENABLE) ? (index*Z_VAR+j): (TREE_VAR == ORAM)? SubMap[index]+j : RhoSubMap[index]+j;
          if (TREE_VAR == ORAM && STL_ENABLE && SUBTREE_ENABLE && NUM_CHANNELS_SUBTREE >  1)
          {
            int gi_prime = gi + (LEVEL-TOP_CACHE)*Z - oram_effective_pl;
            int i_prime = floor(gi_prime/Z) + 1 + L1;
            int j_prime = gi_prime % Z;
            int index_prime = calc_index(label, i_prime);
            addr = SubMap[index_prime] + j_prime;
          }
          insert_oramQ (addr, orig_cycle, orig_thread, orig_instr, 0, 'W');
        }


        if (STT_ENABLE && TREE_VAR == ORAM)
        {
          stt_cand = stt_candidate(label, i);
          if (stt_cand != -1 && (stt_cand != intended_addr || !pinFlag))  
          {
            if (!RING_ENABLE || GlobTree[index].dumnum > LS[i])
            {
              sttctr++;
              GlobTree[index].slot[j].addr = stt_cand;
              GlobTree[index].slot[j].label = PosMap[stt_cand];
              GlobTree[index].slot[j].isReal = true;
              GlobTree[index].slot[j].isData = true;
              stt_invalidate(stt_cand);

              GlobTree[index].dumnum--;

              continue;
            }
          }
        }
        

        if (candidate[j] != -1) 
        {
          if (!RING_ENABLE || GlobTree[index].dumnum > LS[i])
          {
            if (RHO_ENABLE && (TREE_VAR == RHO))
            {
              RhoTree[index].slot[j].addr = RhoStash[candidate[j]].addr;
              RhoTree[index].slot[j].label = RhoStash[candidate[j]].label;
              RhoTree[index].slot[j].isReal = true;
              RhoTree[index].slot[j].isData = true;
            }
            
            else
            {
              GlobTree[index].slot[j].addr = Stash[candidate[j]].addr;
              GlobTree[index].slot[j].label = Stash[candidate[j]].label;
              GlobTree[index].slot[j].isReal = true;
              GlobTree[index].slot[j].isData = true;
              cap_count[calc_index(label, CAP_LEVEL)-CAP_NODE+1]++;

              GlobTree[index].dumnum--;
            }

            remove_from_stash(candidate[j]);
          }

          // if ((TREE_VAR == RHO) /*&& (tracectr_test == 7911) && (j == 1)*/)
          // {
          //   printf("write path: @ LEVEL %d after stash removal @ trace: %d stash is   %d   full\n", i,  tracectr_test, rho_stashctr);
          //   // print_stash();
          // }
          

        }

        

      }
    // }
  }
  
}

void print_path(int label){
  for(int i = LEVEL_VAR-1; i >= 0; i--)
  {
    int index = calc_index(label, i);
    for(int j = 0; j < LZ_VAR[i]; j++)
    {
      if (RHO_ENABLE && (TREE_VAR == RHO))
      {
        printf("LOG: path: %d level:%d  slot: %d  addr:%d  label: %d real: %d\n", label, i,j, RhoTree[index].slot[j].addr, RhoTree[index].slot[j].label,  RhoTree[index].slot[j].isReal);
      }
      else
      {
        printf("LOG: path: %d level:%d  slot: %d  addr:%d  label: %d real: %d  valid: %d\n", label, i,j, GlobTree[index].slot[j].addr, GlobTree[index].slot[j].label, GlobTree[index].slot[j].isReal, GlobTree[index].slot[j].valid);
      }
    }
  }
}

void print_stash(){
  printf("print stash\n");
  for(int i = 0; i < STASH_SIZE_VAR; i++ )
  {
    if (RHO_ENABLE && (TREE_VAR == RHO))
    {
      if (RhoStash[i].isReal)
      {
      printf("Stash[%d]:  %d:%d\n", i, RhoStash[i].addr, RhoStash[i].label);
      }

    }
    else
    {
      if (Stash[i].isReal)
      {
      printf("Stash[%d]:  %d:%d\n", i, Stash[i].addr, Stash[i].label);
      }
    }
  }
}

// void print_plb(){
//   for (int i = 0; i < PLB_SIZE; i++)
//   {
//     if (PLB[i] != -1)
//     {
//      printf("PLB[%d]:  %d\n", i, PLB[i]);
//     }
//   }
  
// }

// void print_plb_stat(){
//   for (int i = 0; i < PLB_SIZE; i++)
//   {
//     if (plb_hist[i] != -1)
//     {
//      printf("plb_hist[%d]:  %d\n", i, plb_hist[i]);
//     }
//   }

//   printf("\n\n");
  
//   for (int i = 0; i < PLB_SIZE; i++)
//   {
//     if (plb_conflict[i] != -1)
//     {
//      printf("plb_conflict[%d]:  %d\n", i, plb_conflict[i]);
//     }
//   }

//   printf("\n\n");
  
//   for (int i = 0; i < PLB_SIZE; i++)
//   {
//     if (plb_interval[i] != -1)
//     {
//      printf("plb_interval[%d]:  %d\n", i, plb_interval[i]);
//     }
//   }

// }

// bool plb_contain(int tag){
//   if (PLB[tag % PLB_SIZE] == tag)
//   {
//     return true;
//   }
//   return false;
// }

void print_path_occupancy(int label){
  int count = 0;
  for (int i = LEVEL_VAR -1; i >= 0; i--)
  {
    int index = calc_index(label, i);
    for (int j = 0; j < LZ_VAR[j]; j++)
    {
      if (RHO_ENABLE && (TREE_VAR == RHO))
      {
        count = RhoTree[index].slot[j].isReal ? count+1 : count;
      }
      else
      {
        count = GlobTree[index].slot[j].isReal ? count+1 : count;
      }
    }
    
  }
  printf("Path[%d]: %d real blocks\n", label, count);
  
}

bool bk_evict_needed(){
  int stashctr_var = (RHO_ENABLE && (TREE_VAR == RHO))? rho_stashctr : stashctr;
  if(stashctr_var >= OV_THRESHOLD_VAR)
  {
    return true;
  }
  return false;
}

// issue dummy access (read path + write path) until stash occupancy drops a threshold
void background_eviction(){

  if (RHO_ENABLE && (TREE_VAR == RHO))
  {
    rho_bkctr++;
  }
  else
  {
    bkctr++;
  }
  int label = rand() % PATH_VAR;
  switch_enqueue_to(HEAD);
  read_path(label);
  write_path(label);
  switch_enqueue_to(TAIL);

  if(STT_ENABLE)
  {
    free_stash();
  }

  // printf("@> bk evict  %d   %d\n", stashctr, tracectr);

}

// assign the block a new label and update in posmap and stash 
void remap_block(int addr){

 
  int label = rand() % PATH_VAR;

  int prevlabel = PosMap[addr];


  intended_addr = addr;
  if (RHO_ENABLE && (TREE_VAR == RHO))
  {
    if (ACCESS_VAR == REGULAR)
    {
      rho_update_tag_array(addr, label);
    }
    
  }
  else
  {
    PosMap[addr] = label;   // $$$ remember to exclude current path later on
  }
  
  if (!STT_ENABLE || TREE_VAR != ORAM || !stt_access(addr))
  {
    int index = get_stash(addr);

    if (index == -1)
    {
      if (RHO_ENABLE && (TREE_VAR == RHO))
      {
        printf("ERROR: remap: @ trace %d  block %d not found in rho stash!\n", tracectr, addr);
      }
      else
      {
        printf("ERROR: remap: @ trace %d  block %d not found in stash!\n", tracectr, addr);
        printf("remap:  previous Posmap[%d]: %d\n", addr, prevlabel);
        printf("remap:  Posmap[%d]: %d\n", addr, PosMap[addr]);
        printf("remap:  stashctr: %d    bkctr: %d\n", stashctr, bkctr);
        // printf("remap:  PLB[%d]: %d\n", addr%PLB_SIZE, PLB[addr%PLB_SIZE]);
      }
      print_oram_stats();
      exit(1);
    }

    intended = index;

    if (RHO_ENABLE && (TREE_VAR == RHO) && (ACCESS_VAR == EVICT) )
    {
      remove_from_stash(index);
      return;
    }
    
    if (RHO_ENABLE && (TREE_VAR == RHO))
    {
      RhoStash[index].label = label;
    }
    else
    {
      Stash[index].label = label;
    }
  }
  

  
}

int add_to_stash(Slot s){

  // if (STT_ENABLE && TREE_VAR == ORAM)
  // {
  //   if (stt_fill(s.addr))
  //   {
  //     return -2;
  //   }
  // }

  

  if (RING_ENABLE && WRITE_LINGER)
  {
    if (s.label != PosMap[s.addr])
    {
      linger_discard++;
      return STASH_SIZE_ORG;
    }
  }
  
  

  if (s.addr == 0)
  {
    printf("exiting due to 0 added to stash!\n");
    // exit(1);
  }
  
  
  for(int i = 0; i < STASH_SIZE_VAR; i++ )
  {
    if (RHO_ENABLE && (TREE_VAR == RHO))
    {
      if(!RhoStash[i].isReal)
      {

        RhoStash[i].addr = s.addr;
        RhoStash[i].label = s.label;
        RhoStash[i].isReal = true;
        RhoStash[i].isData = true;
        
        rho_stashctr++;
        return i;
      }
    }
    else
    {
      if(!Stash[i].isReal)
      {

        Stash[i].addr = s.addr;
        Stash[i].label = s.label;
        Stash[i].isReal = true;
        Stash[i].isData = true;
        
        stashctr++;

        if (!s.isReal)
        {
          printf("ERROR: add to stash: dummy added @ trace %d\n", tracectr);
          print_stash();
          exit(1);
        }
        return i;
      }
    }
    
  }
  return -1;
}


// remove from stash given the item index in the stash
void remove_from_stash(int index){

  if (RHO_ENABLE && (TREE_VAR == RHO))
  {
    rho_stashctr--;
    RhoStash[index].isReal = false;
  }
  else
  {
    stashctr--;
    Stash[index].isReal = false;
  }  

}


// get stash index of a block
int get_stash(int addr){
  for(int k= 0; k < STASH_SIZE_VAR; k++)
  {
    int stash_addr = (RHO_ENABLE && (TREE_VAR == RHO)) ? RhoStash[k].addr : Stash[k].addr;
    bool stash_real = (RHO_ENABLE && (TREE_VAR == RHO)) ? RhoStash[k].isReal :  Stash[k].isReal;
    if (stash_addr == addr && stash_real)
    {
      return k;
    }
  }
  return -1;
}


// check whether a block exist in stash
bool stash_contain(int addr){
  int count = 0;
  for(int k= 0; k < STASH_SIZE_VAR; k++)
  {
    if (count > stashctr)
    {
      break;    // break if already seen all the real blocks in the stash
    }
    
    int stash_addr = (RHO_ENABLE && (TREE_VAR == RHO)) ? RhoStash[k].addr : Stash[k].addr;
    bool stash_real = (RHO_ENABLE && (TREE_VAR == RHO)) ? RhoStash[k].isReal :  Stash[k].isReal;
    if (stash_real)
    {
      count++;
    }
    
    if (stash_addr == addr && stash_real)
    {
      return true;
    }
  }
  if (STT_ENABLE && TREE_VAR == ORAM)
  {
    if (stt_access(addr))
    {
      return true;
    }
  }
  
  return false;
}

void take_snapshot(char * argv[]){
  switch_sim_enable_to(false);

  char newstr[64];

  FILE *tif; 
  FILE *tifrep; 
  int nonmemops;
  char opertype;
  long long int taddr;
  long long int instrpc;


  tif = fopen(argv[2], "r");
  tifrep = fopen(argv[2], "r");

  int addr;
  int label;

  for(int i = 0; i < 400000001; i++)
  {
    
    if (fgets(newstr,64,tif)) {
      if (sscanf(newstr,"%d %c %Lx %Lx",&nonmemops,&opertype,&taddr,&instrpc) < 1) {
        printf("Panic.  Poor trace format.\n");
        print_oram_stats();
        exit(1);
        }
      addr = block_addr(byte_addr(taddr));
    }
    else if (fgets(newstr,64,tifrep)) {
      if (sscanf(newstr,"%d %c %Lx %Lx",&nonmemops,&opertype,&taddr,&instrpc) < 1) {
        printf("Panic.  Poor trace format.\n");
        print_oram_stats();
        exit(1);
        }
      addr = block_addr(byte_addr(taddr));
    }
    else
    {
      addr = rand() % BLOCK;
    }

    label = PosMap[addr];
  

    if (i <= 10000000 )
    {
      if (i % 4000000 == 0 )
      {
        printf("%dm\n\n",(int)(i/1000000));
        print_count_level();
        printf("\n\n\n\n");
      }
    }
    else if(i <= 90000000 )
    {
      if (i % 20000000 == 0 )
      {
        printf("%dm\n\n",(int)(i/1000000));
        print_count_level();
        printf("\n\n\n\n");
      }
    }
    else if(i <= 300000000 )
    {
      if (i % 50000000 == 0 )
      {
        printf("%dm\n\n",(int)(i/1000000));
        print_count_level();
        printf("\n\n\n\n");
      }
    }
    else if(i <= 400000000 )
    {
      if (i % 10000000 == 0 )
      {
        printf("%dm\n\n",(int)(i/1000000));
        print_count_level();
        printf("\n\n\n\n");
      }
    }
    
    
    


    stash_dist[stashctr]++;

    if (label == -1)
    {
      printf("ERROR: block label not found in pos map!\n");
      print_oram_stats();
      exit(1);
    }
    
    
    read_path(label);
    
    remap_block(addr);

    write_path(label);

    
    while (BK_EVICTION && bk_evict_needed())
    {
      // printf("\nbefore bk evict @ stash %d\n", stashctr);
      int randpath = rand() % PATH;
      read_path(randpath);
      write_path(randpath);
      // printf("after bk evict @ stash %d\n", stashctr);
    } 

    
    fflush(stdout);


  }

  printf("finished.\n");
  switch_sim_enable_to(true);
  exit(0);

}


void free_stash(){
  for (int i = 0; i < STASH_SIZE; i++)
  {
    if (Stash[i].isReal)
    {
      stash_leftover++;
      if (stt_fill(Stash[i].addr))
      {
        stash_removed++;
        remove_from_stash(i);
      }
    }
  }
}

// read the path, remap the block, write back the path
void oram_access(int addr){
  oramctr++;
  stash_dist[stashctr]++;

  oram_acc_addr = addr;

  int label = PosMap[addr];
  if (label == -1)
  {
    printf("ERROR: block label not found in pos map!\n");
    print_oram_stats();
    exit(1);
  }
    
  read_path(label);
  
  remap_block(addr);

  write_path(label);

}

// Freecursive 4.2.4 ORAM access algorithm
void freecursive_access(int addr, char type){
  

  if (stash_contain(addr))      // check if the block is already in the stash
  {
    return;
  }

/*
  // if write bypass feature is on and there is write req hit in the cache
  if (WRITE_BYPASS && write_cache_hit && type == 'W')
  {
    write_cache_hit = false;
    // if the intended block is in plb return now & skip the oram access
    if (plb_contain(addr))
    {
      return;
    }
    
  }
  else
  {
    int i_saved = -1;  // STEP 1   PLB lookup 
    for (int i = 0; i <= H-2; i++)
    {
      plbaccess[i]++;
      // reading form PLB if miss then proceed to access ORAM tree
      int ai = addr/pow(X,i);
      int tag = concat(i, ai);  // tag = i || ai  (bitwise concat)

      // printf("@ trace %d  i: %d   ai: %x    tag: %x\n", tracectr, i, ai, tag);


      // profiling:

      // if ((PLB[tag % PLB_SIZE] != tag) && i != 0)
      // {
      //     if (i == 1)
      //     {
      //       // int pos1_diff = ai - pos1_recent;
      //       // printf("%d\n", pos1_diff);
      //       // pos1_recent = ai;
      //       pos1acc_ctr++;
      //     }
      //     else if (i == 2)
      //     {
      //       // int pos2_diff = ai - pos2_recent;
      //       // printf("%d\n", pos2_diff);
      //       // pos2_recent = ai;
      //       pos2acc_ctr++;
      //     }
      // }

      
      // profiling.
      
      if (plb_access(tag) || buffer_contain(tag))  // PLB hit
      {
        
        // profiling:
        // if (buffer_contain(tag))
        // {
        //   // int pi = buffer_index(tag);
        //   int pi = tag % PREFETCH_BUF_SIZE;

        //   if (i == 1)
        //   {
        //     // printf("%lld\n", trace_clk - PreBuffer[pi].timestamp);
        //     pos1hit++;
        //     if (PreBuffer[pi].type != POS1)
        //     {
        //       pos1conf++;
        //       //  printf("ERROR: freecursive: @trace %d pos1 & i %d don't match!\n", tracectr, i);
        //       //  print_oram_stats();
// exit(1);
        //     }
            
        //   }
        //   else if (i == 2)
        //   {
        //     // printf("%lld\n", trace_clk - PreBuffer[pi].timestamp);
        //     pos2hit++;
        //     if (PreBuffer[pi].type != POS2)
        //     {
        //       pos2conf++;
        //       //  printf("ERROR: freecursive: @trace %d pos2 & i %d don't match!\n", tracectr, i);
        //       //  print_oram_stats();
// exit(1);
        //     }
        //   }
        // }
        // else
        // {
        //   plb_hit[i]++;
        // }
          plb_hit[i]++;
        
       // profiling.
        
        if (i == 0)   // if the intended block is originally a posmap block itself terminate!
        {
          return;
        }
        
        i_saved = i-1;
        break;
      }
      else if(i == H-2)  // PLB miss (retval == 0) & it's last iteration
      {
        i_saved = H-2;
      }
    }

    // profiling:
    pos1_recent = addr/pow(X,1);
    pos2_recent = addr/pow(X,2);
    // profiling.

    while(i_saved >= 1)   // STEP 2  PosMap block access
    {
      int ai = addr/pow(X,i_saved);
      int tag = concat(i_saved, ai);  // tag = i || ai  (bitwise concat)
      // printf("@ trace %d  i saved: %d   ai: %x    tag: %x\n", tracectr, i_saved, ai, tag);

      if (tag == addr)
      {
        return;
      }
    
      // profiling:
      // if (tag == plb_evict[tag % PLB_SIZE])
      // {
      //   plb_hist[tag % PLB_SIZE]++;
      //   // plb_trace[tag % PLB_SIZE]
      //   if (plb_interval[tag % PLB_SIZE] == -1 && plb_trace[tag % PLB_SIZE] != -1)
      //   {
      //     plb_interval[tag % PLB_SIZE] = tracectr - plb_trace[tag % PLB_SIZE];
      //   }
        

      // }
      
      // profiling.
      if (!stash_contain(tag)) // access oram tree iff block does not exist in the stash
      {
        unsigned int caddr = tag << ((unsigned int) log2(BLOCK_SIZE));
        // if (cache_invalidate(caddr) != -1)
        // {
        //   printf("ERROR: @trace %d  cache contained %x  tagged %x \n", tracectr, caddr, tag);
        //   print_oram_stats();
// exit(1);
        // }

        cache_invalidate(caddr);
        // reset_dirty_search();
        
        pinOn();
        if (RING_ENABLE)
        {
          ring_access(tag);
        }
        else
        {
          oram_access(tag);
        }
        pinOff();

        if (i_saved == 1)
        {
          pos1_access++;
        }
        else if (i_saved == 2)
        {
          pos2_access++;
        }
        
        
      }
      // if (i_saved == 1)
      // {

        int si;
        int victim = plb_fill(tag);
        if( victim != -1)
        {

          // profiling:
          // if (plbQ->size < plbQ->limit)
          // {
          //   insert_plbQ(victim);
          // }

          // if (plb_evict[victim % PLB_SIZE] == -1)
          // {
          //   plb_evict[victim % PLB_SIZE] = victim;
          //   plb_trace[victim % PLB_SIZE] = tracectr;
          //   // plb_hist[victim % PLB_SIZE] = 1;
          // }
          // // else if(plb_evict[victim % PLB_SIZE] == victim)
          // // {
          // //   plb_hist[victim % PLB_SIZE]++;
          // // }
          // else if(plb_evict[victim % PLB_SIZE] != victim)
          // {
          //   plb_conflict[victim % PLB_SIZE]++;
          // }
          // profiling.
          
          
          Slot s = {.addr = victim , .label = PosMap[victim], .isReal = true, .isData = false};
          
          
          if (stash_contain(s.addr))
          {
            printf("ERROR: freecursive: block %d already in stash!\n", s.addr);
            print_oram_stats();
            exit(1);
          }
          else
          {
            si = add_to_stash(s);
            if(si == -1){
            printf("ERROR: freecursive: stash overflow!   @ %d\n", stashctr); 
            print_oram_stats();
            exit(1);
            }
            
          }
          
          

        }

        // PLB[tag % PLB_SIZE] = tag;
        if (!STT_ENABLE || !stt_contain(tag)) 
        {
          int index = get_stash(tag);
          if (index == -1)
          {
            printf("ERROR: freecursive: block not found in stash!\n");
            print_oram_stats();
            exit(1);
          }
          
          remove_from_stash(index);
        } 
        else
        {
          // printf("hereeeee blk: %d \n", tag);
          stt_invalidate(tag);
        }
        
        
      // }
      
      
      i_saved--;
    }

  }
*/
  // printf("freecursuve: b4 last oram access (data): %d\n", addr);
  // oram_access(addr);  // STEP 3   Data block access
  if (RING_ENABLE)
  {
    if (WRITE_LINGER && type == 'W')
    {
      int cur = PosMap[addr];
      while (PosMap[addr] == cur)
      {
        PosMap[addr] = rand() % PATH;
      }
      
      Slot s = {.addr = addr , .label = PosMap[addr], .isReal = true, .isData = true};
      int si = add_to_stash(s);
      if (si == -1)
      {
        printf("ERROR: freecursive: write linger: stash overflow!   @ %d\n", stashctr); 
        print_oram_stats();
        exit(1);
      }
      
    }
    else
    {
      ring_access(addr);
    }
    
    
  }
  else
  {
    oram_access(addr);
  }
}

void test_oram(char * argv[]){

  FILE **tif;  /* The handles to the trace input files. */
  int MAXTRACELINESIZE = 64;
  char newstr[MAXTRACELINESIZE];  // 64 is the max trace size line
  int NUMCORES = 1;

  int *nonmemops;
  char *opertype;
  long long int *addr;
  long long int *instrpc;
  int numc=0;

  int hitctr = 0;
  int missctr = 0;
  int evictctr = 0;

  typedef struct MemRequest{
    bool valid;
    int nonmemops; 
    char opertype;
    long long int addr;
    long long int instrpc;
  } MemRequest;

  MemRequest evicted[16]; 	// array of evicted request for cores, each core can have one evicted at a time 16: max num of cores
  bool no_miss_occured;	// a flag that is set based on cache access and used to keep on reading trace file until it's cache hit
  bool eviction_writeback[16] = {0}; // a flag that says next request is gonna be eviction writeback

  tif = (FILE **)malloc(sizeof(FILE *)*NUMCORES);
  nonmemops = (int *)malloc(sizeof(int)*NUMCORES);
  opertype = (char *)malloc(sizeof(char)*NUMCORES);
  addr = (long long int *)malloc(sizeof(long long int)*NUMCORES);
  instrpc = (long long int *)malloc(sizeof(long long int)*NUMCORES);


  for (int numc=0; numc < NUMCORES; numc++) {
     tif[numc] = fopen(argv[numc+2], "r");
     printf("test oram: tif:  %s \n", argv[numc+2]);
     if (!tif[numc]) {
       printf("Missing input trace file %d.  Quitting. \n",numc);
       print_oram_stats();
       exit(1);
     }
  }


  for (int i = 0; i < NUMCORES; i++)
	{
    evicted[i].valid = false;
	}

  while (tracectr_test < TRACE_SIZE)
  {
    // printf("test oram: while trace ctr: %d  \n", tracectr_test);
    for (int i = 0; i < oramQ->size; i++)
    {
      Dequeue(oramQ);
    }
    
    no_miss_occured = true;
		// cache enabled:
    if (CACHE_ENABLE)
		{
			while (no_miss_occured)
			{
        if (fgets(newstr,MAXTRACELINESIZE,tif[numc])) {
          if (evicted[numc].valid)
          {
            nonmemops[numc] = evicted[numc].nonmemops;
            opertype[numc] = evicted[numc].opertype;
            addr[numc] = evicted[numc].addr;
            evicted[numc].valid = false;
            eviction_writeback[numc] = true;
            break;
          }
          

          if (sscanf(newstr,"%d %c",&nonmemops[numc],&opertype[numc]) > 0) {
              tracectr_test++;
            if (opertype[numc] == 'R') {
              if (sscanf(newstr,"%d %c %Lx %Lx",&nonmemops[numc],&opertype[numc],&addr[numc],&instrpc[numc]) < 1) {
              printf("Panic.  Poor trace format.\n");
              print_oram_stats();
              exit(1);
              }
            }
            else {
              if (opertype[numc] == 'W') {
                if (sscanf(newstr,"%d %c %Lx",&nonmemops[numc],&opertype[numc],&addr[numc]) < 1) {
                  printf("Panic.  Poor trace format.\n");
                  print_oram_stats();
                  exit(1);
                }
              }
              else {
              printf("Panic.  Poor trace format.\n");
              print_oram_stats();
              exit(1);
              }
            }
            if (cache_access(addr[numc], opertype[numc]) == HIT)
            {
              hitctr++;
            }
            else // miss occured
            {
              // tracectr_test++;
              missctr++;
              int victim = cache_fill(addr[numc], opertype[numc]);
              if ( victim != -1)
              {
                evictctr++;
                evicted[numc].valid = true;
                evicted[numc].nonmemops = nonmemops[numc]+1;
                evicted[numc].opertype = 'W';
                evicted[numc].addr = victim;
              }

              no_miss_occured = false;

            }
          }
          else {
            printf("Panic.  Poor trace format.\n");
            print_oram_stats();
            exit(1);
          }
        
        }
			}
		}
		// cache disabled:
		else	
		{
		  /* Done consuming one line of the trace file.  Read in the next. */
	      if (fgets(newstr,MAXTRACELINESIZE,tif[numc])) {
	        if (sscanf(newstr,"%d %c",&nonmemops[numc],&opertype[numc]) > 0) {
				tracectr_test++;
		  if (opertype[numc] == 'R') {
		    if (sscanf(newstr,"%d %c %Lx %Lx",&nonmemops[numc],&opertype[numc],&addr[numc],&instrpc[numc]) < 1) {
		      printf("Panic.  Poor trace format.\n");
		      print_oram_stats();
          exit(1);
		    }
		  }
		  else {
		    if (opertype[numc] == 'W') {
		      if (sscanf(newstr,"%d %c %Lx",&nonmemops[numc],&opertype[numc],&addr[numc]) < 1) {
		        printf("Panic.  Poor trace format.\n");
		        print_oram_stats();
            exit(1);
		      }
		    }
		    else {
		      printf("Panic.  Poor trace format.\n");
		      print_oram_stats();
          exit(1);
		    }
		  }
		}
		else {
		  printf("Panic.  Poor trace format.\n");
		  print_oram_stats();
      exit(1);
		}
	      }
		}

    if (eviction_writeback[numc])
    {
      write_cache_hit = true;
      eviction_writeback[numc] = false;
      if (RHO_ENABLE)
      {
        // rho_insert(evicted[numc].addr);		// add evicted blk from llc to rho and consequently evicted blk from rho to oram

        int masked_addr = (int)(evicted[numc].addr & (BLOCK-1));
        if (rho_lookup(masked_addr) == -1)
        {
          // printf("\ntest oram: %d gonna be inserted to rho @ trace: %d\n", masked_addr, tracectr_test);
          rho_insert(addr[numc]);		// add evicted blk from llc to rho and consequently evicted blk from rho to oram
          /* code */
        }
        
      }
    }
    
    invoke_oram(addr[numc], 0, numc, 0, 0, opertype[numc]);

  }



  printf("\n............... Test ORAM Stats ...............\n");
	printf("trace ctr: %d\n", tracectr_test);
	printf("invoke ctr: 	%d\n", invokectr);
	printf("oram ctr: 	%d\n", oramctr);
	printf("bk evict rate: %f%%\n", 100*(double)bkctr/invokectr);
	printf("cache hit rate: %f%%\n", 100*(double)hitctr/(hitctr+missctr));
  printf("cache evict rate wrt # miss: %f%%\n", 100*(double)evictctr/(missctr));
	printf("rho hit rate: %f%%\n", 100*(double)rho_hit/(invokectr));
	printf("rho bk evict rate: %f%%\n", 100*(double)rho_bkctr/rho_hit);

  exit(0);

}

void invoke_oram(long long int physical_address, long long int arrival_time, int thread_id, int instruction_id, long long int instruction_pc, char type) {
    
  invokectr++;
  
  orig_addr = physical_address;
  orig_cycle = arrival_time; 
  orig_thread = thread_id; 
  orig_instr = instruction_id; 
  orig_pc = instruction_pc;

  // int addr = (int)(physical_address & (BLOCK-1));
  unsigned int addr = block_addr(physical_address);
  // printf("invoke oram: physical addr: %lld\n", addr);
  // printf("invoke oram: b4 freecursive call addr: %d\n", addr);

  // if (RING_ENABLE)
  // {
  //   if (RAND_ENABLE)
  //   {
  //     addr = rand() % BLOCK;
  //   }
    
  //   ring_access(addr);
  //   return;
  // }
  

  if (RHO_ENABLE)
  {
    int label = rho_lookup(addr);  // ???
    
    if (label != -1)
    {
      // printf("invoke: rho hit on %d\n", addr);
      // printf("invoke: b4 rho access stashctr: %d    @ %d\n", rho_stashctr, tracectr_test);
      
      // printf("invoke: before rho access stashctr:  %d\n", rho_stashctr);
	    // printf("invoke: before rho access hit rate: %f%%\n", 100*(double)rho_hit/(invokectr));
      switch_tree_to(RHO);
      // if (tracectr_test > 10000)
      // {
      //   print_stash();
      // }
      rho_access(addr, label);
      switch_tree_to(ORAM);
      // printf("invoke: after rho access @ %d\n", tracectr_test);
      rho_hit++;
      return;
    }
  }

 
  freecursive_access(addr, type);

  if (STT_ENABLE && TREE_VAR == ORAM)
  {
    free_stash();
    // printf("@> free stash: %d\n", stashctr);
  }
}

// capacity occupation analysis
void print_cap_percent(){
  printf("cap percent @ level: %d\n", CAP_LEVEL);
  printf("sub cap: %d\n", sub_cap);
  for (int i = 0; i < CAP_NODE; i++)
  {
    printf("%d:           %d \n",i, 100*cap_count[i]/sub_cap);
    if (i > 10000)
    {
      return;
    }
    
  }
}


// translate ORAM tree index to DRAM address using subtree scheme to exploit channel parallelism
int index_to_addr(int index){
  int level = floor(log_base2(index+1));
  int sublevel = floor(level/SUBTREE_LEVEL);
  int inner_sublevel = level - sublevel*SUBTREE_LEVEL;
  int head_of_curr_level = pow(2, level) - 1;
  int horiz_distance_index_from_head = index - head_of_curr_level;
  int num_sublevel_passed = floor ((horiz_distance_index_from_head)/pow(2, inner_sublevel));
  int term1 = pow(2, (level - inner_sublevel)) - 1;
  int term2 = num_sublevel_passed * SUBTREE_BUCKET;    // ??? maybe should substitute with # slots
  int root_of_curr_subtree = term1 + term2;
  int head_of_curr_sublevel = pow(2, inner_sublevel) - 1;
  int distance_from_root_subtree = head_of_curr_sublevel + horiz_distance_index_from_head -  num_sublevel_passed * pow(2, inner_sublevel);
  int addr = root_of_curr_subtree + distance_from_root_subtree;
  addr = addr*Z_VAR;
  // addr = addr << (int)log2(BLOCK_SIZE);
  return addr;
}


void test_subtree(){

  // print 3 random path to see whether the address of each # subtree_level consecutive blocks on the path fal into the range of subtree_slots or not  
  printf("\n\n\nPrinting 3 random paths...\n\n");
  for (int i = 0; i < 3; i++)
	{
		int pl = rand() % PATH;
		printf("\ni: %d		pl: %d\n", i, pl);
		for (int i = 0; i < LEVEL; i++)
		{
			int idx = calc_index(pl, i);
			int addr = index_to_addr(idx)+0;
			printf("@ Level: %d		index: %d		addr: %d\n", i, idx, addr);
		}
	}


  // print head node of each level to see whether the index and addr matches every other sublevel_tree-1 levels or not
  printf("\n\n\n\nPrinting head node of each level...\n\n");
	int prevlevel = -1;
	int level;

	for (int i = 0; i < NODE; i++)
	{
		for (int j = 0; j < Z; j++)
		{

			int addr = index_to_addr(i)+j;
			level = calc_level(i);
			if (level != prevlevel && j == 0)
			{
				printf("level: %d 	node: %d	slot: %d		index: %d		addr: %d\n", level, i, j,  i*Z+j, addr);
			}

		}

		prevlevel = level;
    if (prevlevel == LEVEL -1)
    {
      break;
    }
    
		
	}

	exit(0);
}



// rho implementation:

void rho_alloc(){

  for (int i = 0 ; i < RHO_NODE; i++) 
  {
    int l = calc_level(i);  
    for (int k = 0; k < RHO_LZ[l]; ++k)
    {
      RhoTree[i].slot[k].addr = -1;
      RhoTree[i].slot[k].label = -1;
      RhoTree[i].slot[k].isReal = false;
      RhoTree[i].slot[k].isData = false;
    }
  }

  for (int i = 0; i < RHO_STASH_SIZE; i++)
  {
    RhoStash[i].isReal = false;
  }  

  for (int i = 0; i < RHO_SET; i++)
  {
    for (int j = 0; j < RHO_WAY; j++)
    {
      TagArray[i][j] = -1;
      TagArrayLabel[i][j] = -1;
      TagArrayLRU[i][j] = -1;
    }
    
  }
  
}


int tag_array_find_spot(unsigned int index){
    for (unsigned int j = 0; j < RHO_WAY; j++)
    {
        if (TagArray[index][j] == -1)
        {
            return j;
        }
    }
    return -1;  
}


int tag_array_find_victim(unsigned int index) {
    int victim = -1;
    long long int min = CYCLE_VAL;
    for (unsigned int j = 0; j < RHO_WAY; j++)
    {
        if (TagArrayLRU[index][j] < min)
        {
          victim = j;
          min = TagArrayLRU[index][j];
        }
    }
    return victim;
}


void tag_array_update_LRU(unsigned int index, unsigned int way){
  TagArrayLRU[index][way] = CYCLE_VAL;
    // if (TagArrayLRU[index][way] >= RHO_WAY-1)
    // {
    //     TagArrayLRU[index][way] = 0;
    // }
    // else
    // {
    //     TagArrayLRU[index][way]++;
    // } 
}

void tag_array_reset_LRU(unsigned int index, unsigned int way){
  TagArrayLRU[index][way] = CYCLE_VAL;
    // TagArrayLRU[index][way] = 1;
}


void rho_update_tag_array(int addr, int label){
  int index = addr % RHO_SET;
  for (int i = 0; i < RHO_WAY; i++)
  {
    if (TagArray[index][i] == addr)
    {
      TagArrayLabel[index][i] = label;
      return;
    }
  }
}


int rho_lookup(int addr){
  
  int index = addr % RHO_SET;
  for (int i = 0; i < RHO_WAY; i++)
  {
    if (TagArray[index][i] == addr)
    {
      return TagArrayLabel[index][i];
    }
  }
  return -1;
}



void rho_access(int addr, int label){
  rhoctr++;
  rho_stash_dist[stashctr]++;

  if (stash_contain(addr))      // check if the block is already in the stash
  {
    return;
  }


  read_path(label);

  remap_block(addr);

  write_path(label);

  if (RHO_BK_EVICTION && bk_evict_needed())
  {
    background_eviction();  
  } 

}


// when a block gets evicted from llc it is placed into the rho
void rho_insert(int physical_address){
  // int addr = (int)(physical_address & (BLOCK-1));
  int addr = block_addr(physical_address);

  int index = addr % RHO_SET;

  int victim = -1;
  int victim_label = -1;
  
  int way = tag_array_find_spot(index);

  // evict
  if (way == -1)
  {
    way = tag_array_find_victim(index);
    victim = TagArray[index][way];
    victim_label = TagArrayLabel[index][way];
  }
  // insert new block info into the rho   ~~~> new block is the one got evicted from llc
  TagArray[index][way] = addr;
  TagArrayLabel[index][way] = rand() % RHO_PATH; 
  tag_array_reset_LRU(index, way);
  Slot new = {.addr = addr , .label = TagArrayLabel[index][way], .isReal = true, .isData = true};  // ??? isdata not necessarily true but doesn't matter at this point
  switch_tree_to(RHO);
  if (add_to_stash(new) == -1)
  {
    printf("ERROR: rho insert: block %d is not added to rho stash\n", addr);
    printf("ERROR: rho insert: rho stash ctr: %d\n", rho_stashctr);
    print_oram_stats();
    exit(1);
  }

  

  // if no eviction incured make a dummy rho access by reading and writing a random path to prevent stash overflow
  if (victim == -1)
  {
    // rho_access(new.addr, new.label);
    int label = rand() % RHO_PATH;
    // int label = new.label;
    // printf("\n\nrho insert: b4 dummy access rho stash ctr: %d    @ trace: %d\n", rho_stashctr, tracectr_test);
    // print_path_occupancy(label);
    read_path(label);
    // printf("rho insert: after read path rho stash ctr: %d    @ trace: %d\n", rho_stashctr, tracectr_test);
    // print_path_occupancy(label);
    // print_path(label);
    write_path(label);
    // printf("rho insert: after dummy access rho stash ctr: %d      @ trace: %d\n", rho_stashctr, tracectr_test);
    // print_path_occupancy(label);
    return;
  }
  
  // if the insertion incur an eviction make a rho access for the victim
  if (victim != -1)
  {
    // printf("rho insert: victim: %d    victim label: %d   @ trace: %d\n", victim, victim_label, tracectr_test);
    // print_path(victim_label);
    // printf("rho insert: victim label: %d\n", victim_label);
    switch_access_to(EVICT);
    rho_access(victim, victim_label);
    switch_access_to(REGULAR);

    // add the victim to oram stash
    PosMap[victim] = rand() % PATH;  
    Slot s = {.addr = victim , .label = PosMap[victim], .isReal = true, .isData = true};  // ??? isdata not necessarily true but doesn't matter at this point
    switch_tree_to(ORAM);
    if (add_to_stash(s) == -1)
    {
      printf("ERROR: rho insert: block %d is not added to oram stash\n", victim);
      printf("ERROR: rho insert: oram stash ctr: %d\n", stashctr);
      print_oram_stats();
      exit(1);
    }
  }
  


}

// dummy access is used when timing channel security is enabled, it accesses a random path w/o remapping any block (like in background eviction)
void dummy_access(TreeType tree){

  if (tree == RHO)
  {
    rho_dummyctr++;
  }
  else
  {
    dummyctr++;
  }
  

  switch_tree_to(tree);
  int label = rand() % PATH_VAR;
  switch_enqueue_to(HEAD);
  read_path(label);
  write_path(label);
  switch_enqueue_to(TAIL);  // switch back to normal tail enqueue 
  switch_tree_to(ORAM);     // switch back to oram tree 

}


// prefetch functions:
bool buffer_contain(int addr){
  // for (int i = 0; i < PREFETCH_BUF_SIZE; i++)
  // {
  //   if (PreBuffer[i].addr == addr && PreBuffer[i].valid)
  //   {
  //     return true;
  //   }
  // }
  // return false;
  int i = addr % PREFETCH_BUF_SIZE;
  if (PreBuffer[i].addr == addr && PreBuffer[i].valid)
  {
    return true;
  }
  return false;
}

int buffer_index(int addr){

  int index = -1;
  for (int i = 0; i < PREFETCH_BUF_SIZE; i++)
  {
    if (PreBuffer[i].addr == addr && PreBuffer[i].valid)
    {
      index = i;
      break;
    }
  }
  if (index == -1)
  {
    printf("ERROR: buffer index: @ trace %d  block %d does not exist!\n", tracectr, addr);
    print_oram_stats();
    exit(1);
  }

  return index;
}

int buffer_find_victim(){
  int min = CYCLE_VAL;
  int index = -1;
  for (int i = 0; i < PREFETCH_BUF_SIZE; i++)
  {
    if (PreBuffer[i].timestamp < min)
    {
      min = PreBuffer[i].timestamp;
      index = i;
    }
  }
  if (index == -1)
  {
    printf("ERROR: buffer find victim failed!\n");
    print_oram_stats();
    exit(1);
  }
  
  return index;
}

void insert_buffer(int addr){

  int index = -1;

  if (PREFETCH_INPLACE)
  {
    int victim = plb_fill(addr);
    if (victim != -1)
    {

      // add the victim to the stash

      Slot s = {.addr = victim, .label = PosMap[victim], .isReal = true, .isData = false};
      
      if (stash_contain(s.addr))
      {
        printf("ERROR: insert buffer: block %d already in stash!\n", s.addr);
        print_oram_stats();
        exit(1);
      }
      else
      {
        int vi = add_to_stash(s);
        if(vi == -1){
          printf("ERROR: insert buffer: stash overflow!   @ %d\n", stashctr); 
          print_oram_stats();
          exit(1);
        }
        
      }
    }

  }
  else
  {
    index = addr % PREFETCH_BUF_SIZE;

    // if no free spot found in the buffer, find a victim to evict
    if (PreBuffer[index].valid)
    {
      // index = buffer_find_victim(); 

      // add the victim to the stash

      Slot s = {.addr = PreBuffer[index].addr , .label = PosMap[PreBuffer[index].addr], .isReal = true, .isData = false};
      
      if (stash_contain(s.addr))
      {
        printf("ERROR: insert buffer: block %d already in stash!\n", s.addr);
        print_oram_stats();
        exit(1);
      }
      else
      {
        int vi = add_to_stash(s);
        if(vi == -1){
          printf("ERROR: insert buffer: stash overflow!   @ %d\n", stashctr); 
          print_oram_stats();
          exit(1);
        }
        
      }
    }
    
    // insert the intended posmap block to the buffer
    PreBuffer[index].addr = addr;
    PreBuffer[index].timestamp = trace_clk;
    PreBuffer[index].valid = true;  
    PreBuffer[index].type = pos_var;  
  }
  
  


  int si = get_stash(addr);
  if (si == -1)
  {
    printf("ERROR: insert buffer: block not found in stash!\n");
    print_oram_stats();
    exit(1);
  }
  
  remove_from_stash(si);
  
}

void prefetch_access(int addr){

  prefetchctr++;

  int label = PosMap[addr];
  if (label == -1)
  {
    printf("ERROR: prefetch access: block %d label not found in pos map!\n", addr);
    print_oram_stats();
    exit(1);
  }

  switch_tree_to(ORAM);     // switch to oram tree 
  switch_enqueue_to(HEAD);
  pinOn();
  read_path(label);
  remap_block(addr);
  write_path(label);
  pinOff();
  switch_enqueue_to(TAIL);  // switch back to normal tail enqueue 

  // insert the pinned block in the stash to the prefetch buffer and then remove it from the stash, 
  // if it leads to evict another block will add that evicted block to the stash 
  insert_buffer(addr);      

}

int pos_calc(int addr, int degree){
  int pos = (addr/pow(X,degree));
  pos = concat(degree, pos);
  return pos;
}


void invoke_prefetch(){
  prefetchctr++;

  int set = dirty_coor[0];
  int way = dirty_coor[1];

  if (LLC[set][way].valid && LLC[set][way].dirty)
  {
    int daddr = LLC[set][way].addr;
    int pos1 = pos_calc(daddr,1);
    int pos2 =  pos_calc(daddr,2);
    if (plb_contain(pos1))
    {
      // plb_pin(pos1);
      case1++;
    }
    else if (plb_contain(pos2))
    {
      case2++;
    }
    else
    {
      case3++;
    }
    
    
  }
  
}


// void invoke_prefetch(){

//   int candidate = -1;
//   unsigned int addr = block_addr(curr_trace);

//   if ((PREFETCH_TYPE == HISTORY_BASED) || (PREFETCH_TYPE == COMBO))
//   {
//     bool pos1_needed[4] = {false};     // flag to determine whether footprint suggest to prefetch pos1 nexts
//     bool pos1_possible[4] = {false};   // flag to determine whether prefetch pos1 nexts can be prefetched
//     int pos1_count[4] = {0};           // popcount of pos1 nexts
//     int pos1_next[4] = {0};                  // pos1_next[0] is pos1 of current block address
//     int pos2;
//     int max = 0;

//     Event e = {.pc = curr_pc, .addr = curr_page, .offset = curr_offset};
    
//     unsigned long long int footprint = table_access(e);

//     if (footprint != -1)
//     {
//       for (int i = 0; i < 4; i++)
//       {
//         pos1_needed[i] = ((footprint & (0xffff<<(i*X))) != 0);
//         pos1_count[i] = __builtin_popcount((footprint & (0xffff<<(i*X))));
//       }

//       for (int i = 0; i < 4; i++)
//       {
//         if (pos1_needed[i])
//         {
//           case1++;
//           pos1_next[i] = (addr/pow(X,1)) + i;  
//           pos2 = (pos1_next[i]/pow(X,1));
//           pos2 = concat(2, pos2);
//           pos1_next[i] = concat(1, pos1_next[i]);  
//           if (plb_contain(pos2) || stash_contain(pos2) || buffer_contain(pos2))
//           {
//             case2++;
//             if (!plb_contain(pos1_next[i]) && !stash_contain(pos1_next[i]) && !buffer_contain(pos1_next[i]))
//             {
//               case3++;
//               pos1_possible[i] = true;
//               break;
//             }
//           }
//         }
//       }

//       for (int i = 0; i < 4; i++)
//       {
//         if (pos1_possible[i])
//         {
//           if (pos1_count[i] > max)
//           {
//             max = pos1_count[i];
//             candidate = pos1_next[i];
//             pos_var = POS1;
//           }
//         }
//       }
      
//     } 
//   }

//   if ( (PREFETCH_TYPE == STRIDE_BASED) || (PREFETCH_TYPE == COMBO && (candidate == -1)) )
//   {
//     unsigned int curr_addr;
//     curr_addr = block_addr(curr_trace);
//     // curr_addr = block_addr(next_trace);
    
//     int pos1 = (curr_addr/pow(X,1));   // the 1st posmap block of current trace ~>  + stride will be candidate for prefetching
//     int pos2 = (curr_addr/pow(X,2));   // the 2nd posmap block of current trace ~>  + stride will be candidate for prefetching

//     int pos1_next = pos1 + PREFETCH_STRIDE;
//     int pos2_next = pos2 + PREFETCH_STRIDE;

//     pos1 = concat(1,pos1);
//     pos2 = concat(2,pos2);

//     pos1_next = concat(1, pos1_next);
//     pos2_next = concat(2, pos2_next);

//     if (plb_contain(pos2) || stash_contain(pos2) || buffer_contain(pos2))
//     {
//       case1++;
//       if (plb_contain(pos1_next))
//       {
//         plbpos1++;
//       }
//       else if (stash_contain(pos1_next))
//       {
//         stashpos1++;
//       }
//       else if (buffer_contain(pos1_next))
//       {
//         bufferpos1++;
//       }
      
//       if (!plb_contain(pos1_next) && !stash_contain(pos1_next) && !buffer_contain(pos1_next))
//       {
//         // curr_trace = curr_trace + (1 << (int)log2(X));
//         candidate = pos1_next; // if pos2 is available go ahead and preftech pos1
//         pos_var = POS1;
//         pos1ctr++;
//       }
//     } 
//     else
//     {
//       candidate = pos2; // otherwise prefetch pos2
//       pos_var = POS2;
//       pos2ctr++;
//       case2++;
//     }
//     if (candidate == -1)
//     {
//       if (!plb_contain(pos2_next) && !stash_contain(pos2_next) && !buffer_contain(pos2_next))
//       {
//         // curr_trace = curr_trace + (1 << 2*((int)log2(X)));
//         candidate = pos2_next; // if still has no candidate prefetch pos2_next
//         pos_var = POS2;
//         pos2ctr++;
//         case3++;
//       }
//     }
//   }

//   if (candidate != -1)
//   {
//     prefetch_access(candidate);
//   }
//   else
//   {
//     dummy_access(ORAM);
//   }
 
// }




void reset_dirty_search(){
  set_start = 0;
  way_start = 0;
}

void early_writeback(){

  int i_target = 0;
  int j_target = 0;
  int wb_cand = -1;

  int i = dirty_coor[0];
  int j = dirty_coor[1];
  int posblk;
  int pref_cand = -1;

  if (LLC[i][j].valid && LLC[i][j].dirty)
  {
    posblk = pos_calc(block_addr(LLC[i][j].addr), 1);
    // plb_print(posblk);
    if (plb_contain(posblk))
    {
      // plb_print(posblk);
      wb_cand = LLC[i][j].addr;
      i_target = i;
      j_target = j;
      dirty_pointctr++;
      plb_unpin(posblk);
    }
    else
    {
      int pospos = pos_calc(block_addr(LLC[i][j].addr), 2);
      if(plb_contain(pospos))
      {
        pref_cand = posblk;
        ptr_fail++;
      }
    }
    
  }
  else
  {
    for (i = set_start; i < NUM_SET; i++)
    {
      for (j = way_start; j < NUM_WAY; j++)
      {
        if (LLC[i][j].valid && LLC[i][j].dirty)
        {
          posblk = pos_calc(block_addr(LLC[i][j].addr), 1);
          // plb_print(posblk);
          if (plb_contain(posblk))
          {
            wb_cand = LLC[i][j].addr;
            i_target = i;
            j_target = j;
            plb_unpin(posblk);
            break;
          }
          else
          {
            int pospos = pos_calc(block_addr(LLC[i][j].addr), 2);
            if(plb_contain(pospos) && pref_cand == -1)
            {
              pref_cand = posblk;
              search_fail++;
            }
          }
          
        }
      }
      if(wb_cand != -1)
      {
        break;
      }
    }
  }
  

  if (wb_cand != -1)
  {
    earlyctr++;
    int addr = block_addr(wb_cand);
    int label = PosMap[addr];

    switch_tree_to(ORAM);     // switch to oram tree 
    switch_enqueue_to(HEAD);
    read_path(label);
    remap_block(addr);
    write_path(label);
    switch_enqueue_to(TAIL);  // switch back to normal tail enqueue

    cache_clean(i_target, j_target);

    // set_start = i_target;
    // way_start = j_target;
  }
  // else if(pref_cand != -1)
  // {
  //   prefetch_access(pref_cand);
  //   precase++;
  //   // reset_dirty_search();
  // }
  else
  {
    dummy_access(ORAM);
    // reset_dirty_search();
  }
  


}



void ring_access(int addr){
  ringctr++;
  int label = PosMap[addr];

  // printf("\n@ ring access  trace %d\n", tracectr);
  // print_stash();

  // if (tracectr % 100000 == 0)
  // {
  //   printf("\n@ ring access  trace %d\n", tracectr);
  //   print_count_level();
  // }
  

  if (stash_contain(addr))
  {
    stash_cont++;
    return;
  }
  

  // printf("\nb4 read stash %d  trace %d\n", stashctr, tracectr);
  ring_read_path(label, addr);
  // printf("af read stash %d  trace %d\n", stashctr, tracectr);
  // printf("@> ring read path  trace %d\n\n", tracectr);
  // print_stash();

  remap_block(addr);
  // printf("@> remap block  trace %d\n", tracectr);

  ring_round = (ring_round + 1) % RING_A; // ??? to be defined

  if (ring_round == 0)
  {
  //  printf("\n@- evict  trace %d  stash %d\n", tracectr, stashctr);
    ring_evict_path(label);
  //  printf("@> evict  trace %d  stash %d\n\n", tracectr, stashctr);
  }
  // printf("@> evict path  trace %d\n", tracectr);
  
  // printf("\n@- reshuffle  trace %d  stash %d\n", tracectr, stashctr);
  ring_early_reshuffle(label);
  // printf("@> reshuffle trace %d   stash %d\n", tracectr, stashctr);

}


void ring_read_path(int label, int addr){
  Element *pN = (Element*) malloc(sizeof (Element));
  pN->addr = label;
  // Enqueue(pathQ, pN);

  for (int i = 0; i < LEVEL; i++)
  {
    int index = calc_index(label, i);
    int offset = rand() % LZ_VAR[i]; // ??? should change to chose randomly from dummies
    while (GlobTree[index].slot[offset].isReal)
    {
      offset = rand() % LZ_VAR[i];
    }
    

    for(int j = 0; j < LZ_VAR[i]; j++)
    {
      if (GlobTree[index].slot[j].isReal && GlobTree[index].slot[j].addr == addr)
      {
        // printf("\n offset trace %d\n", tracectr);
        offset = j;

        // profiling
        if (i <= TOP_BOUNDRY)
        {
          topctr++;
        }
        else if (i <= MID_BOUNDRY)
        {
          midctr++;
        }
        else
        {
          botctr++;
        }
      }
    }

    if (GlobTree[index].slot[offset].isReal)
    {
        // printf("\n is real trace %d\n", tracectr);

      // if (GlobTree[index].slot[offset].addr == 0)
      // {

      //   printf("\nZERO \n");
        // printf("offset: %d\n", offset);
        // printf("index: %d\n", index);
        // printf("level: %d\n", i);
        // printf("addr: %d\n", GlobTree[index].slot[offset].addr);
        // printf("label: %d\n\n", GlobTree[index].slot[offset].label);
      // }
      
      if(add_to_stash(GlobTree[index].slot[offset]) != -1)
      {
        GlobTree[index].slot[offset].isReal = false;
        GlobTree[index].slot[offset].addr = -1;
        GlobTree[index].slot[offset].label = -1;
        GlobTree[index].dumnum++;
      }
      else
      {
        printf("ERROR: ring read: trace %d stash overflow!  @ %d\n", tracectr, stashctr);
        print_oram_stats();
        exit(1);
      }
    }
    

    ring_invalidate(index, offset);  // ??? to be implemented

    if (i >= TOP_CACHE_VAR && SIM_ENABLE_VAR)
    {
      int mem_addr = index*Z_VAR + offset;
      insert_oramQ(mem_addr, orig_cycle, orig_thread, orig_instr, orig_pc, 'R');
    }
     GlobTree[index].count++;
  }
}





void ring_evict_path(int label){
  // printf("\nevict path trace %d\n", tracectr);

  ring_evictctr++;
  // int label = ring_G % PATH;

  ep_round++;

  // if ((ep_round % RING_REV) == 0)
  // {
  //   // int diff = shuff[9] - touchcount; 
  //   // printf("%d \n", diff);
  //   // touchcount = shuff[9];
  //   printf("%d \n", injcount);
  //   injcount = 0;
  // }
  

  label = reverse_lex(ring_G);

  // Element *pN = Dequeue(pathQ);

  // ep_round = (ep_round + 1) % EP_TURN;

  // if (ep_round == 0 && pN != NULL)
  // {
  //   label = pN->addr;
  //   ring_G--;
  // }
  

  

  // printf("\npath %d\n", label);
  // for (int i = LEVEL-15; i >= 9; i--)
  // {
  //   // int gi = calc_index(ring_G, i);
  //   // if (revarr[gi] == label)
  //   // {
  //     int mask = 1<<(LEVEL-i-1);
  //     int bit = (label&mask)>>(LEVEL-i-1);
  //     int index = calc_index(label,i);
  //     int adjacent = (bit == 1) ? index-1 : index+1;

  //     // printf("mask %d\n", mask);
  //     // printf("bit %d\n", bit);
  //     // printf("index %d\n", index);
  //     // printf("adjacent %d\n", adjacent);

  //     // if (GlobTree[adjacent].count > GlobTree[index].count)
  //     if ((GlobTree[adjacent].count > GlobTree[index].count) && (GlobTree[adjacent].count >= LS[i]-1))
  //     {
  //       label = (bit == 1) ? label-(1<<(LEVEL-i-1)) : label+(1<<(LEVEL-i-1));
  //       ring_G--;
  //     }
  //     // printf("label %d\n", adjacent);
  //   // }
  //   // else
  //   // {
  //   //   int temp = revarr[gi];
  //   //   revarr[gi] = label;
  //   //   label = temp;
  //   // }
      
  // }

  // int gi = calc_index(ring_G, 9);
  // int max = GlobTree[gi].count;
  // int max_ind = gi;

  // for (int i = gi-255; i < RING_REV; i++)
  // {
  //   if (GlobTree[i].count > max && GlobTree[i].count >= RING_S-1)
  //   {
  //     max = GlobTree[i].count;
  //     max_ind = i;
  //   }
  // }

  // if (max_ind != gi && injcount < 120)
  // {
  //   label = label & 0b00000000011111111111111;
  //   label = label | (max_ind<<14);
  //   ring_G--;
  //   injcount++;
  // }
  
  
  

  ring_G++;
  int b4 = stashctr;
  read_path(label);
  write_path(label);
  wbctr += b4 - stashctr;

  // ring_early_reshuffle(label);

}


void ring_early_reshuffle(int label){
  // printf("reshuffle trace %d\n", tracectr);
  for (int i = 0; i < LEVEL; i++)
  {
    int index = calc_index(label, i);
    int reqmade = 0;
    int dum_cand[Z] = {0};
    int cand_ind = 0;
    if (GlobTree[index].count >= LS[i] || i < TOP_CACHE)
    {
      // printf("\nlevel %d reshuffle\n", i);
      shuff[i]++;
      for (int j = 0; j < LZ_VAR[i]; j++)
      {
        if (i >= TOP_CACHE_VAR && SIM_ENABLE_VAR && GlobTree[index].slot[j].isReal)
        {
          int mem_addr = index*Z_VAR + j;
          insert_oramQ(mem_addr, orig_cycle, orig_thread, orig_instr, orig_pc, 'R');
          // printf("%d: slot %d accessed ~> real? %s\n", reqmade, j,  GlobTree[index].slot[j].isReal?"yes":"no");
          reqmade++;
        }
        
        if (GlobTree[index].slot[j].isReal)
        {
          // printf("j: %d\n", j);
          // printf("index: %d\n", index);
          // printf("level: %d\n", i);
          // printf("addr: %d\n", GlobTree[index].slot[j].addr);
          // printf("label: %d\n\n", GlobTree[index].slot[j].label);
          if(add_to_stash(GlobTree[index].slot[j]) != -1)
          {
            GlobTree[index].slot[j].isReal = false;
            GlobTree[index].slot[j].addr = -1;
            GlobTree[index].slot[j].label = -1;
            GlobTree[index].dumnum++;
          }
          else
          {
            printf("ERROR: ring read: trace %d stash overflow!  @ %d\n", tracectr, stashctr);
            print_oram_stats();
            exit(1);
          }
        }
        else if(GlobTree[index].slot[j].valid)
        {
          // printf("\ncand ind %d\n", cand_ind);
          dum_cand[cand_ind] = j;
          cand_ind++;
        }
      }
      if (i >= TOP_CACHE_VAR && SIM_ENABLE_VAR)
      {
        for (int k = 0; k < RING_Z-reqmade; k++)
        {
          int ri = -1;
          int sd = -1;
          while (sd == -1)
          {
            ri = rand() % cand_ind;
            sd = dum_cand[ri];
          }
           
          int mem_addr = index*Z_VAR + sd;
          insert_oramQ(mem_addr, orig_cycle, orig_thread, orig_instr, orig_pc, 'R');
          // printf("%d: slot %d accessed ~> dummy? %s\n", k, sd, GlobTree[index].slot[sd].isReal?"no":"yes");
          dum_cand[ri] = -1;
        }
      }
      

      reset_candidate();
      pick_candidate(index, label, i);

      for (int j = 0; j < LZ_VAR[i]; j++)
      {
        GlobTree[index].slot[j].valid = true;
        if (i >= TOP_CACHE_VAR && SIM_ENABLE_VAR)
        {
          int mem_addr = index*Z_VAR + j;
          insert_oramQ(mem_addr, orig_cycle, orig_thread, orig_instr, orig_pc, 'W');
        }
        if (candidate[j] != -1 && GlobTree[index].dumnum > LS[i])
        {
          // printf("cand[%d]: %d\n", candidate[j], Stash[candidate[j]].addr);
          GlobTree[index].slot[j].addr = Stash[candidate[j]].addr;
          GlobTree[index].slot[j].label = Stash[candidate[j]].label;
          GlobTree[index].slot[j].isReal = true;
          GlobTree[index].slot[j].isData = true;
          GlobTree[index].dumnum--;


          remove_from_stash(candidate[j]);
        }
      }
      
      GlobTree[index].count = 0;
    }
    
  }
  

}

void ring_invalidate(int index, int offset){
  GlobTree[index].slot[offset].valid = false;
}

void print_shuff_stat(){
  printf("\nreshuffle count of each level \n");
  for (int i = 0; i < LEVEL; i++)
  {
    printf("%d\n", shuff[i]);
  }
}

int reverse_lex(int n){
	int rev = 0;
	int i = 0;
	while(n > 0)
	{
		int bit = n & 1;
		rev += bit*pow(2,INT_BITS-1-i);
		n = n >> 1;
		i++;
	}
	return rev;
}




























void print_oram_stats(){

  // print_count_level();

  print_shuff_stat();
  // print_stash();

  int shuffctr = 0; 
  for (int i = TOP_CACHE; i < LEVEL; i++)
  {
    shuffctr += shuff[i];
  }
  

  printf("\n\n\n\n............... ORAM Stats ...............\n\n");
  printf("Execution Time (s)       %f\n", cpu_time_used);
  printf("Total Cycles             %lld \n", CYCLE_VAL);
  printf("Trace Size               %d\n", tracectr);
  printf("Mem Cycles #             %lld\n", mem_clk);
  printf("Invoke Mem #             %d\n", invokectr);
  printf("ORAM Access #            %d\n", oramctr);
  printf("ORAM Dummy #             %d\n", dummyctr);
  printf("Pos1 Access #            %d\n", pos1_access);
  printf("Pos2 Access #            %d\n", pos2_access);
  printf("PLB pos0 hit             %f%%\n", 100*(double)plb_hit[0]/plbaccess[0]);
  printf("PLB pos1 hit             %f%%\n", 100*(double)plb_hit[1]/plbaccess[1]);
  printf("PLB pos2 hit             %f%%\n", 100*(double)plb_hit[2]/plbaccess[2]);
  printf("PLB pos0 hit #           %lld\n", plb_hit[0]);
  printf("PLB pos1 hit #           %lld\n", plb_hit[1]);
  printf("PLB pos2 hit #           %lld\n", plb_hit[2]);
  printf("PLB pos0 acc #           %lld\n", plbaccess[0]);
  printf("PLB pos1 acc #           %lld\n", plbaccess[1]);
  printf("PLB pos2 acc #           %lld\n", plbaccess[2]);
  printf("oramQ Size               %d\n", oramQ->size);
  printf("Bk Evict                 %f%%\n", 100*(double)bkctr/oramctr);
  printf("Bk Evict #               %d\n", bkctr);
  printf("Cache Hit                %f%%\n", 100*(double)hitctr/(hitctr+missctr));
  printf("Cache Evict              %f%%\n", 100*(double)evictctr/(missctr));
  printf("Rho Hit                  %f%%\n", 100*(double)rho_hit/(invokectr));
  printf("Rho Access #             %d\n", rhoctr);
  printf("Rho  Dummy #             %d\n", rho_dummyctr);
  printf("Rho Bk Evict             %f%%\n", 100*(double)rho_bkctr/rho_hit);
  printf("Early WB #               %d\n", earlyctr);
  printf("Early WB Pointer #       %d\n", dirty_pointctr);
  printf("Cache Dirty #            %d\n", cache_dirty);
  printf("ptr fail #               %d\n", ptr_fail);
  printf("search fail #            %d\n", search_fail);
  printf("pin ctr #                %d\n", pinctr);
  printf("unpin ctr #              %d\n", unpinctr);
  printf("prefetch case #          %d\n", precase);
  printf("STT Cand #               %d\n", sttctr);
  printf("Stash leftover #         %d\n", stash_leftover);
  printf("Stash removed #          %d\n", stash_removed);
  printf("fill hit #               %d\n", fillhit);
  printf("fill miss #              %d\n", fillmiss);
  printf("Top hit                  %f%%\n", 100*(double)topctr/(topctr+midctr+botctr));
  printf("Mid hit                  %f%%\n", 100*(double)midctr/(topctr+midctr+botctr));
  printf("Bot hit                  %f%%\n", 100*(double)botctr/(topctr+midctr+botctr));
  printf("Ring evict               %d\n", ring_evictctr);
  printf("Stash occ                %d\n", stashctr);
  printf("Stash Contain            %d\n", stash_cont);
  printf("Linger Discard           %d\n", linger_discard);
  printf("Ring shuff 10+           %d\n", shuffctr);
  printf("Ring acc                 %d\n", ringctr);
  printf("EP writeback             %d\n", wbctr);
  // printf("Path Latency Avg         %f\n", path_access_latency_avg);
}


// Mehrnoosh.

































// ROB Structure, used to release stall on instructions 
// when the read request completes
extern struct robstructure *ROB;

// // Current Processor Cycle
// extern long long int CYCLE_VAL;

#define max(a,b) (((a)>(b))?(a):(b))

#define BIG_ACTIVATION_WINDOW 1000000

// moving window that captures each activate issued in the past 
int activation_record[MAX_NUM_CHANNELS][MAX_NUM_RANKS][BIG_ACTIVATION_WINDOW];

// record an activate in the activation record
  void
record_activate (int channel, int rank, long long int cycle) 
{
  assert (activation_record[channel][rank][(cycle % BIG_ACTIVATION_WINDOW)] == 0);	//can't have two commands issued the same cycle - hence no two activations in the same cycle
  activation_record[channel][rank][(cycle % BIG_ACTIVATION_WINDOW)] = 1;
  return;
}


// Have there been 3 or less activates in the last T_FAW period 
  int
is_T_FAW_met (int channel, int rank, int cycle) 
{
  int start = cycle;
  int number_of_activates = 0;
  if (start >= T_FAW)

  {
    for (int i = 1; i <= T_FAW; i++)

    {
      if (activation_record[channel][rank]
          [(start - i) % BIG_ACTIVATION_WINDOW] == 1)
        number_of_activates++;
    }
  }

  else

  {
    for (int i = 1; i <= start; i++)

    {
      if (activation_record[channel][rank][start - i] %
          BIG_ACTIVATION_WINDOW == 1)
        number_of_activates++;
    }
  }
  if (number_of_activates < 4)
    return 1;

  else
    return 0;
}


// shift the moving window, clear out the past
  void
flush_activate_record (int channel, int rank, long long int cycle) 
{
  if (cycle >= T_FAW + PROCESSOR_CLK_MULTIPLIER)

  {
    for (int i = 1; i <= PROCESSOR_CLK_MULTIPLIER; i++)
      activation_record[channel][rank][(cycle - T_FAW - i) % BIG_ACTIVATION_WINDOW] = 0;	// make sure cycle >tFAW
  }
}  

// initialize dram variables and statistics
  void
init_memory_controller_vars () 
{
  num_read_merge = 0;
  num_write_merge = 0;
  for (int i = 0; i < NUM_CHANNELS; i++)

  {
    for (int j = 0; j < NUM_RANKS; j++)

    {
      for (int w = 0; w < BIG_ACTIVATION_WINDOW; w++)
        activation_record[i][j][w] = 0;
      for (int k = 0; k < NUM_BANKS; k++)

      {
        dram_state[i][j][k].state = IDLE;
        dram_state[i][j][k].active_row = -1;
        dram_state[i][j][k].next_pre = -1;
        dram_state[i][j][k].next_pre = -1;
        dram_state[i][j][k].next_pre = -1;
        dram_state[i][j][k].next_pre = -1;
        cmd_precharge_issuable[i][j][k] = 0;
        stats_num_activate_read[i][j][k] = 0;
        stats_num_activate_write[i][j][k] = 0;
        stats_num_activate_spec[i][j][k] = 0;
        stats_num_precharge[i][j][k] = 0;
        stats_num_read[i][j][k] = 0;
        stats_num_write[i][j][k] = 0;
        cas_issued_current_cycle[i][j][k] = 0;
      }  cmd_all_bank_precharge_issuable[i][j] = 0;
      cmd_powerdown_fast_issuable[i][j] = 0;
      cmd_powerdown_slow_issuable[i][j] = 0;
      cmd_powerup_issuable[i][j] = 0;
      cmd_refresh_issuable[i][j] = 0;
      next_refresh_completion_deadline[i][j] = 8 * T_REFI;
      last_refresh_completion_deadline[i][j] = 0;
      forced_refresh_mode_on[i][j] = 0;
      refresh_issue_deadline[i][j] =
        next_refresh_completion_deadline[i][j] - T_RP - 8 * T_RFC;
      num_issued_refreshes[i][j] = 0;
      stats_time_spent_in_active_power_down[i][j] = 0;
      stats_time_spent_in_precharge_power_down_slow[i][j] = 0;
      stats_time_spent_in_precharge_power_down_fast[i][j] = 0;
      last_activate[i][j] = 0;

      //If average_gap_between_activates is 0 then we know that there have been no activates to [i][j]
      average_gap_between_activates[i][j] = 0;
      stats_num_powerdown_slow[i][j] = 0;
      stats_num_powerdown_fast[i][j] = 0;
      stats_num_powerup[i][j] = 0;
      stats_num_activate[i][j] = 0;
      command_issued_current_cycle[i] = 0;
    }   read_queue_head[i] = NULL;
    write_queue_head[i] = NULL;
    read_queue_length[i] = 0;
    write_queue_length[i] = 0;
    command_issued_current_cycle[i] = 0;

    // Stats
    stats_reads_merged_per_channel[i] = 0;
    stats_writes_merged_per_channel[i] = 0;
    stats_reads_seen[i] = 0;
    stats_writes_seen[i] = 0;
    stats_reads_completed[i] = 0;
    stats_writes_completed[i] = 0;
    stats_average_read_latency[i] = 0;
    stats_average_read_queue_latency[i] = 0;
    stats_average_write_latency[i] = 0;
    stats_average_write_queue_latency[i] = 0;
    stats_page_hits[i] = 0;
    stats_read_row_hit_rate[i] = 0;
  } }  

/********************************************************/ 
/*	Utility Functions				*/ 
/********************************************************/ 
  unsigned int
log_base2 (unsigned int new_value) 
{
  int i;
  for (i = 0; i < 32; i++)
  {
    new_value >>= 1;
    if (new_value == 0)
      break;
  }
  return i;
}


// Function to decompose the incoming DRAM address into the
// constituent channel, rank, bank, row and column ids. 
// Note : To prevent memory leaks, call free() on the pointer returned
// by this function after you have used the return value.
dram_address_t * calc_dram_addr (long long int physical_address) 
{
  long long int input_a, temp_b, temp_a;
  int channelBitWidth = log_base2 (NUM_CHANNELS);
  int rankBitWidth = log_base2 (NUM_RANKS);
  int bankBitWidth = log_base2 (NUM_BANKS);
  int rowBitWidth = log_base2 (NUM_ROWS);
  int colBitWidth = log_base2 (NUM_COLUMNS);
  int byteOffsetWidth = log_base2 (CACHE_LINE_SIZE);
  dram_address_t * this_a =
    (dram_address_t *) malloc (sizeof (dram_address_t));
  this_a->actual_address = physical_address;
  input_a = physical_address;
  input_a = input_a >> byteOffsetWidth;	// strip out the cache_offset
  if (ADDRESS_MAPPING == 1)

  {
    temp_b = input_a;
    input_a = input_a >> colBitWidth;
    temp_a = input_a << colBitWidth;
    this_a->column = temp_a ^ temp_b;	//strip out the column address
    temp_b = input_a;
    input_a = input_a >> channelBitWidth;
    temp_a = input_a << channelBitWidth;
    this_a->channel = temp_a ^ temp_b;	// strip out the channel address
    temp_b = input_a;
    input_a = input_a >> bankBitWidth;
    temp_a = input_a << bankBitWidth;
    this_a->bank = temp_a ^ temp_b;	// strip out the bank address 
    temp_b = input_a;
    input_a = input_a >> rankBitWidth;
    temp_a = input_a << rankBitWidth;
    this_a->rank = temp_a ^ temp_b;	// strip out the rank address
    temp_b = input_a;
    input_a = input_a >> rowBitWidth;
    temp_a = input_a << rowBitWidth;
    this_a->row = temp_a ^ temp_b;	// strip out the row number
  }

  else

  {
    temp_b = input_a;
    input_a = input_a >> channelBitWidth;
    temp_a = input_a << channelBitWidth;
    this_a->channel = temp_a ^ temp_b;	// strip out the channel address
    temp_b = input_a;
    input_a = input_a >> bankBitWidth;
    temp_a = input_a << bankBitWidth;
    this_a->bank = temp_a ^ temp_b;	// strip out the bank address 
    temp_b = input_a;
    input_a = input_a >> rankBitWidth;
    temp_a = input_a << rankBitWidth;
    this_a->rank = temp_a ^ temp_b;	// strip out the rank address
    temp_b = input_a;
    input_a = input_a >> colBitWidth;
    temp_a = input_a << colBitWidth;
    this_a->column = temp_a ^ temp_b;	//strip out the column address
    temp_b = input_a;
    input_a = input_a >> rowBitWidth;
    temp_a = input_a << rowBitWidth;
    this_a->row = temp_a ^ temp_b;	// strip out the row number
  }
  return (this_a);
}


// Function to create a new request node to be inserted into the read
// or write queue.
  void *
init_new_node (long long int physical_address, long long int arrival_time,
    optype_t type, int thread_id, int instruction_id,
    long long int instruction_pc, int oramid, TreeType tree) 
{
  request_t * new_node = NULL;
  new_node = (request_t *) malloc (sizeof (request_t));
  if (new_node == NULL)

  {
    printf ("FATAL : Malloc Error\n");
    exit (-1);
  }

  else

  {
    // Mehrnoosh:
    new_node->oramid = oramid;
    new_node->tree = tree;
    // Mehrnoosh.


    new_node->physical_address = physical_address;
    new_node->arrival_time = arrival_time;
    new_node->dispatch_time = -100;
    new_node->completion_time = -100;
    new_node->latency = -100;
    new_node->thread_id = thread_id;
    new_node->next_command = NOP;
    new_node->command_issuable = 0;
    new_node->operation_type = type;
    new_node->request_served = 0;
    new_node->instruction_id = instruction_id;
    new_node->instruction_pc = instruction_pc;
    new_node->next = NULL;
    dram_address_t * this_node_addr = calc_dram_addr (physical_address);
    new_node->dram_addr.actual_address = physical_address;
    new_node->dram_addr.channel = this_node_addr->channel;
    new_node->dram_addr.rank = this_node_addr->rank;
    new_node->dram_addr.bank = this_node_addr->bank;
    new_node->dram_addr.row = this_node_addr->row;
    new_node->dram_addr.column = this_node_addr->column;
    free (this_node_addr);
    new_node->user_ptr = NULL;
    return (new_node);
  }
}


// Function that checks to see if an incoming read can be served by a
// write request pending in the write queue and return
// WQ_LOOKUP_LATENCY if there is a match. Also the function goes over
// the read_queue to see if there is a pending read for the same
// address and avoids duplication. The 2nd read is assumed to be
// serviced when the original request completes.

#define RQ_LOOKUP_LATENCY 1
  int
read_matches_write_or_read_queue (long long int physical_address) 
{

  //get channel info
  dram_address_t * this_addr = calc_dram_addr (physical_address);
  int channel = this_addr->channel;
  free (this_addr);
  request_t * wr_ptr = NULL;
  request_t * rd_ptr = NULL;
  LL_FOREACH (write_queue_head[channel], wr_ptr) 
  {
    if (wr_ptr->dram_addr.actual_address == physical_address)

    {
      num_read_merge++;
      stats_reads_merged_per_channel[channel]++;
      return WQ_LOOKUP_LATENCY;
    }
  }
  LL_FOREACH (read_queue_head[channel], rd_ptr) 
  {
    if (rd_ptr->dram_addr.actual_address == physical_address)

    {
      num_read_merge++;
      stats_reads_merged_per_channel[channel]++;
      return RQ_LOOKUP_LATENCY;
    }
  }
  return 0;
}


// Function to merge writes to the same address
  int
write_exists_in_write_queue (long long int physical_address) 
{

  //get channel info
  dram_address_t * this_addr = calc_dram_addr (physical_address);
  int channel = this_addr->channel;
  free (this_addr);
  request_t * wr_ptr = NULL;
  LL_FOREACH (write_queue_head[channel], wr_ptr) 
  {
    if (wr_ptr->dram_addr.actual_address == physical_address)

    {
      num_write_merge++;
      stats_writes_merged_per_channel[channel]++;
      return 1;
    }
  }
  return 0;
}


// Insert a new read to the read queue
request_t * insert_read (long long int physical_address,
    long long int arrival_time, int thread_id,
    int instruction_id, long long int instruction_pc, int oramid, TreeType tree) 
{
  optype_t this_op = READ;

  //get channel info
  dram_address_t * this_addr = calc_dram_addr (physical_address);
  int channel = this_addr->channel;
  free (this_addr);
  stats_reads_seen[channel]++;
  request_t * new_node =
    init_new_node (physical_address, arrival_time, this_op, thread_id,
        instruction_id, instruction_pc, oramid, tree);
  LL_APPEND (read_queue_head[channel], new_node);
  read_queue_length[channel]++;

  //UT_MEM_DEBUG("\nCyc: %lld New READ:%lld Core:%d Chan:%d Rank:%d Bank:%d Row:%lld RD_Q_Length:%lld\n", CYCLE_VAL, new_node->id, new_node->thread_id, new_node->dram_addr.channel,  new_node->dram_addr.rank,  new_node->dram_addr.bank,  new_node->dram_addr.row, read_queue_length[channel]);
  return new_node;
}


// Insert a new write to the write queue
request_t * insert_write (long long int physical_address,
    long long int arrival_time, int thread_id,
    int instruction_id, int oramid, TreeType tree) 
{
  optype_t this_op = WRITE;
  dram_address_t * this_addr = calc_dram_addr (physical_address);
  int channel = this_addr->channel;
  free (this_addr);
  stats_writes_seen[channel]++;
  request_t * new_node =
    init_new_node (physical_address, arrival_time, this_op, thread_id,
        instruction_id, 0, oramid, tree);
  LL_APPEND (write_queue_head[channel], new_node);
  write_queue_length[channel]++;

  //UT_MEM_DEBUG("\nCyc: %lld New WRITE:%lld Core:%d Chan:%d Rank:%d Bank:%d Row:%lld WR_Q_Length:%lld\n", CYCLE_VAL, new_node->id, new_node->thread_id, new_node->dram_addr.channel,  new_node->dram_addr.rank,  new_node->dram_addr.bank,  new_node->dram_addr.row, write_queue_length[channel]);
  return new_node;
}


// Function to update the states of the read queue requests.
// Each DRAM cycle, this function iterates over the read queue and
// updates the next_command and command_issuable fields to mark which
// commands can be issued this cycle
  void
update_read_queue_commands (int channel) 
{
  request_t * curr = NULL;
  LL_FOREACH (read_queue_head[channel], curr) 
  {

    // ignore the requests whose completion time has been determined
    // these requests will be removed this very cycle 
    if (curr->request_served == 1)
      continue;
    int bank = curr->dram_addr.bank;
    int rank = curr->dram_addr.rank;
    int row = curr->dram_addr.row;
    switch (dram_state[channel][rank][bank].state)

    {

      // if the DRAM bank has no rows open and the chip is
      // powered up, the next command for the request
      // should be ACT.
      case IDLE:
      case PRECHARGING:
      case REFRESHING:
        curr->next_command = ACT_CMD;
        if (CYCLE_VAL >= dram_state[channel][rank][bank].next_act
            && is_T_FAW_met (channel, rank, CYCLE_VAL))
          curr->command_issuable = 1;

        else
          curr->command_issuable = 0;

        // check if we are in OR too close to the forced refresh period
        if (forced_refresh_mode_on[channel][rank]
            || ((CYCLE_VAL + T_RAS) >
              refresh_issue_deadline[channel][rank]))
          curr->command_issuable = 0;
        break;
      case ROW_ACTIVE:

        // if the bank is active then check if this is a row-hit or not
        // If the request is to the currently
        // opened row, the next command should
        // be a COL_RD, else it should be a
        // PRECHARGE
        if (row == dram_state[channel][rank][bank].active_row)

        {
          curr->next_command = COL_READ_CMD;
          if (CYCLE_VAL >= dram_state[channel][rank][bank].next_read)
            curr->command_issuable = 1;

          else
            curr->command_issuable = 0;
          if (forced_refresh_mode_on[channel][rank]
              || ((CYCLE_VAL + T_RTP) >
                refresh_issue_deadline[channel][rank]))
            curr->command_issuable = 0;
        }

        else

        {
          curr->next_command = PRE_CMD;
          if (CYCLE_VAL >= dram_state[channel][rank][bank].next_pre)
            curr->command_issuable = 1;

          else
            curr->command_issuable = 0;
          if (forced_refresh_mode_on[channel][rank]
              || ((CYCLE_VAL + T_RP) >
                refresh_issue_deadline[channel][rank]))
            curr->command_issuable = 0;
        }
        break;

        // if the chip was powered, down the
        // next command required is power_up
      case PRECHARGE_POWER_DOWN_SLOW:
      case PRECHARGE_POWER_DOWN_FAST:
      case ACTIVE_POWER_DOWN:
        curr->next_command = PWR_UP_CMD;
        if (CYCLE_VAL >= dram_state[channel][rank][bank].next_powerup)
          curr->command_issuable = 1;

        else
          curr->command_issuable = 0;
        if ((dram_state[channel][rank][bank].state ==
              PRECHARGE_POWER_DOWN_SLOW)
            && ((CYCLE_VAL + T_XP_DLL) >
              refresh_issue_deadline[channel][rank]))
          curr->command_issuable = 0;

        else
          if (((dram_state[channel][rank][bank].state ==
                  PRECHARGE_POWER_DOWN_FAST)
                || (dram_state[channel][rank][bank].state ==
                  ACTIVE_POWER_DOWN))
              && ((CYCLE_VAL + T_XP) > refresh_issue_deadline[channel][rank]))
            curr->command_issuable = 0;
        break;
      default:
        break;
    }
  }
}


// Similar to update_read_queue above, but for write queue
  void
update_write_queue_commands (int channel) 
{
  request_t * curr = NULL;
  LL_FOREACH (write_queue_head[channel], curr) 
  {
    if (curr->request_served == 1)
      continue;
    int bank = curr->dram_addr.bank;
    int rank = curr->dram_addr.rank;
    int row = curr->dram_addr.row;
    switch (dram_state[channel][rank][bank].state)

    {
      case IDLE:
      case PRECHARGING:
      case REFRESHING:
        curr->next_command = ACT_CMD;
        if (CYCLE_VAL >= dram_state[channel][rank][bank].next_act
            && is_T_FAW_met (channel, rank, CYCLE_VAL))
          curr->command_issuable = 1;

        else
          curr->command_issuable = 0;

        // check if we are in or too close to the forced refresh period
        if (forced_refresh_mode_on[channel][rank]
            || ((CYCLE_VAL + T_RAS) >
              refresh_issue_deadline[channel][rank]))
          curr->command_issuable = 0;
        break;
      case ROW_ACTIVE:
        if (row == dram_state[channel][rank][bank].active_row)

        {
          curr->next_command = COL_WRITE_CMD;
          if (CYCLE_VAL >= dram_state[channel][rank][bank].next_write)
            curr->command_issuable = 1;

          else
            curr->command_issuable = 0;
          if (forced_refresh_mode_on[channel][rank]
              || ((CYCLE_VAL + T_CWD + T_DATA_TRANS + T_WR) >
                refresh_issue_deadline[channel][rank]))
            curr->command_issuable = 0;
        }

        else

        {
          curr->next_command = PRE_CMD;
          if (CYCLE_VAL >= dram_state[channel][rank][bank].next_pre)
            curr->command_issuable = 1;

          else
            curr->command_issuable = 0;
          if (forced_refresh_mode_on[channel][rank]
              || ((CYCLE_VAL + T_RP) >
                refresh_issue_deadline[channel][rank]))
            curr->command_issuable = 0;
        }
        break;
      case PRECHARGE_POWER_DOWN_SLOW:
      case PRECHARGE_POWER_DOWN_FAST:
      case ACTIVE_POWER_DOWN:
        curr->next_command = PWR_UP_CMD;
        if (CYCLE_VAL >= dram_state[channel][rank][bank].next_powerup)
          curr->command_issuable = 1;

        else
          curr->command_issuable = 0;
        if (forced_refresh_mode_on[channel][rank])
          curr->command_issuable = 0;
        if ((dram_state[channel][rank][bank].state ==
              PRECHARGE_POWER_DOWN_SLOW)
            && ((CYCLE_VAL + T_XP_DLL) >
              refresh_issue_deadline[channel][rank]))
          curr->command_issuable = 0;

        else
          if (((dram_state[channel][rank][bank].state ==
                  PRECHARGE_POWER_DOWN_FAST)
                || (dram_state[channel][rank][bank].state ==
                  ACTIVE_POWER_DOWN))
              && ((CYCLE_VAL + T_XP) > refresh_issue_deadline[channel][rank]))
            curr->command_issuable = 0;
        break;
      default:
        break;
    }
  }
}


// Remove finished requests from the queues.
  void
clean_queues (int channel) 
{
  request_t * rd_ptr = NULL;
  request_t * rd_tmp = NULL;
  request_t * wrt_ptr = NULL;
  request_t * wrt_tmp = NULL;

  // Delete all READ requests whose completion time has been determined i.e. COL_RD has been issued
  LL_FOREACH_SAFE (read_queue_head[channel], rd_ptr, rd_tmp) 
  {
    if (rd_ptr->request_served == 1)

    {
      assert (rd_ptr->next_command == COL_READ_CMD);
      assert (rd_ptr->completion_time != -100);
      LL_DELETE (read_queue_head[channel], rd_ptr);
      if (rd_ptr->user_ptr)
        free (rd_ptr->user_ptr);
      free (rd_ptr);
      read_queue_length[channel]--;
      assert (read_queue_length[channel] >= 0);
    }
  }

  // Delete all WRITE requests whose completion time has been determined i.e COL_WRITE has been issued
  LL_FOREACH_SAFE (write_queue_head[channel], wrt_ptr, wrt_tmp) 
  {
    if (wrt_ptr->request_served == 1)

    {
      assert (wrt_ptr->next_command == COL_WRITE_CMD);
      LL_DELETE (write_queue_head[channel], wrt_ptr);
      if (wrt_ptr->user_ptr)
        free (wrt_ptr->user_ptr);
      free (wrt_ptr);
      write_queue_length[channel]--;
      assert (write_queue_length[channel] >= 0);
    }
  }
}


// This affects state change
// Issue a valid command for a request in either the read or write
// queue.
// Upon issuing the request, the dram_state is changed and the
// next_"cmd" variables are updated to indicate when the next "cmd"
// can be issued to each bank
  int
issue_request_command (request_t * request) 
{
  long long int cycle = CYCLE_VAL;
  if (request->command_issuable != 1
      || command_issued_current_cycle[request->dram_addr.channel])

  {
    printf
      ("PANIC: SCHED_ERROR : Command for request selected can not be issued in  cycle:%lld.\n",
       CYCLE_VAL);
    return 0;
  }
  int channel = request->dram_addr.channel;
  int rank = request->dram_addr.rank;
  int bank = request->dram_addr.bank;
  long long int row = request->dram_addr.row;
  command_t cmd = request->next_command;
  switch (cmd)

  {
    case ACT_CMD:
      assert (dram_state[channel][rank][bank].state == PRECHARGING
          || dram_state[channel][rank][bank].state == IDLE
          || dram_state[channel][rank][bank].state == REFRESHING);

      //UT_MEM_DEBUG("\nCycle: %lld Cmd:ACT Req:%lld Chan:%d Rank:%d Bank:%d Row:%lld\n", CYCLE_VAL, request->id, channel, rank, bank, row);

      // open row
      dram_state[channel][rank][bank].state = ROW_ACTIVE;
      dram_state[channel][rank][bank].active_row = row;
      dram_state[channel][rank][bank].next_pre =
        max ((cycle + T_RAS), dram_state[channel][rank][bank].next_pre);
      dram_state[channel][rank][bank].next_refresh =
        max ((cycle + T_RAS), dram_state[channel][rank][bank].next_refresh);
      dram_state[channel][rank][bank].next_read =
        max (cycle + T_RCD, dram_state[channel][rank][bank].next_read);
      dram_state[channel][rank][bank].next_write =
        max (cycle + T_RCD, dram_state[channel][rank][bank].next_write);
      dram_state[channel][rank][bank].next_act =
        max (cycle + T_RC, dram_state[channel][rank][bank].next_act);
      dram_state[channel][rank][bank].next_powerdown =
        max (cycle + T_RCD, dram_state[channel][rank][bank].next_powerdown);
      for (int i = 0; i < NUM_BANKS; i++)
        if (i != bank)
          dram_state[channel][rank][i].next_act =
            max (cycle + T_RRD, dram_state[channel][rank][i].next_act);
      record_activate (channel, rank, cycle);
      if (request->operation_type == READ)
        stats_num_activate_read[channel][rank][bank]++;

      else
        stats_num_activate_write[channel][rank][bank]++;
      stats_num_activate[channel][rank]++;
      average_gap_between_activates[channel][rank] =
        ((average_gap_between_activates[channel][rank] *
          (stats_num_activate[channel][rank] - 1)) + (CYCLE_VAL -
            last_activate[channel]
            [rank])) /
        stats_num_activate[channel][rank];
      last_activate[channel][rank] = CYCLE_VAL;
      command_issued_current_cycle[channel] = 1;
      break;
    case COL_READ_CMD:
      assert (dram_state[channel][rank][bank].state == ROW_ACTIVE);
      dram_state[channel][rank][bank].next_pre =
        max (cycle + T_RTP, dram_state[channel][rank][bank].next_pre);
      dram_state[channel][rank][bank].next_refresh =
        max (cycle + T_RTP, dram_state[channel][rank][bank].next_refresh);
      dram_state[channel][rank][bank].next_powerdown =
        max (cycle + T_RTP, dram_state[channel][rank][bank].next_powerdown);
      for (int i = 0; i < NUM_RANKS; i++)

      {
        for (int j = 0; j < NUM_BANKS; j++)

        {
          if (i != rank)
            dram_state[channel][i][j].next_read =
              max (cycle + T_DATA_TRANS + T_RTRS,
                  dram_state[channel][i][j].next_read);

          else
            dram_state[channel][i][j].next_read =
              max (cycle + max (T_CCD, T_DATA_TRANS),
                  dram_state[channel][i][j].next_read);
          dram_state[channel][i][j].next_write =
            max (cycle + T_CAS + T_DATA_TRANS + T_RTRS - T_CWD,
                dram_state[channel][i][j].next_write);
        }
      }

      // set the completion time of this read request
      // in the ROB and the controller queue.
      request->completion_time = CYCLE_VAL + T_CAS + T_DATA_TRANS;
      request->latency = request->completion_time - request->arrival_time;
      request->dispatch_time = CYCLE_VAL;
      request->request_served = 1;

      // update the ROB with the completion time
      ROB[request->thread_id].comptime[request->instruction_id] =
        request->completion_time + PIPELINEDEPTH;
      stats_reads_completed[channel]++;
      stats_average_read_latency[channel] =
        ((stats_reads_completed[channel] -
          1) * stats_average_read_latency[channel] +
         request->latency) / stats_reads_completed[channel];
      stats_average_read_queue_latency[channel] =
        ((stats_reads_completed[channel] -
          1) * stats_average_read_queue_latency[channel] +
         (request->dispatch_time -
          request->arrival_time)) / stats_reads_completed[channel];

      //UT_MEM_DEBUG("Req:%lld finishes at Cycle: %lld\n", request->id, request->completion_time);

      //printf("Cycle: %10lld, Reads  Completed = %5lld, this_latency= %5lld, latency = %f\n", CYCLE_VAL, stats_reads_completed[channel], request->latency, stats_average_read_latency[channel]);     
      stats_num_read[channel][rank][bank]++;
      for (int i = 0; i < NUM_RANKS; i++)

      {
        if (i != rank)
          stats_time_spent_terminating_reads_from_other_ranks[channel][i]
            += T_DATA_TRANS;
      }
      command_issued_current_cycle[channel] = 1;
      cas_issued_current_cycle[channel][rank][bank] = 1;
      break;
    case COL_WRITE_CMD:
      assert (dram_state[channel][rank][bank].state == ROW_ACTIVE);

      //UT_MEM_DEBUG("\nCycle: %lld Cmd: COL_WRITE Req:%lld Chan:%d Rank:%d Bank:%d \n", CYCLE_VAL, request->id, channel, rank, bank);
      dram_state[channel][rank][bank].next_pre =
        max (cycle + T_CWD + T_DATA_TRANS + T_WR,
            dram_state[channel][rank][bank].next_pre);
      dram_state[channel][rank][bank].next_refresh =
        max (cycle + T_CWD + T_DATA_TRANS + T_WR,
            dram_state[channel][rank][bank].next_refresh);
      dram_state[channel][rank][bank].next_powerdown =
        max (cycle + T_CWD + T_DATA_TRANS + T_WR,
            dram_state[channel][rank][bank].next_powerdown);
      for (int i = 0; i < NUM_RANKS; i++)

      {
        for (int j = 0; j < NUM_BANKS; j++)

        {
          if (i != rank)

          {
            dram_state[channel][i][j].next_write =
              max (cycle + T_DATA_TRANS + T_RTRS,
                  dram_state[channel][i][j].next_write);
            dram_state[channel][i][j].next_read =
              max (cycle + T_CWD + T_DATA_TRANS + T_RTRS - T_CAS,
                  dram_state[channel][i][j].next_read);
          }

          else

          {
            dram_state[channel][i][j].next_write =
              max (cycle + max (T_CCD, T_DATA_TRANS),
                  dram_state[channel][i][j].next_write);
            dram_state[channel][i][j].next_read =
              max (cycle + T_CWD + T_DATA_TRANS + T_WTR,
                  dram_state[channel][i][j].next_read);
          }
        }
      }

      // set the completion time of this write request
      request->completion_time = CYCLE_VAL + T_DATA_TRANS + T_WR;
      request->latency = request->completion_time - request->arrival_time;
      request->dispatch_time = CYCLE_VAL;
      request->request_served = 1;
      stats_writes_completed[channel]++;
      stats_num_write[channel][rank][bank]++;
      stats_average_write_latency[channel] =
        ((stats_writes_completed[channel] -
          1) * stats_average_write_latency[channel] +
         request->latency) / stats_writes_completed[channel];
      stats_average_write_queue_latency[channel] =
        ((stats_writes_completed[channel] -
          1) * stats_average_write_queue_latency[channel] +
         (request->dispatch_time -
          request->arrival_time)) / stats_writes_completed[channel];

      //UT_MEM_DEBUG("Req:%lld finishes at Cycle: %lld\n", request->id, request->completion_time);

      //printf("Cycle: %10lld, Writes Completed = %5lld, this_latency= %5lld, latency = %f\n", CYCLE_VAL, stats_writes_completed[channel], request->latency, stats_average_write_latency[channel]);   
      for (int i = 0; i < NUM_RANKS; i++)

      {
        if (i != rank)
          stats_time_spent_terminating_writes_to_other_ranks[channel][i] +=
            T_DATA_TRANS;
      }
      command_issued_current_cycle[channel] = 1;
      cas_issued_current_cycle[channel][rank][bank] = 2;
      break;
    case PRE_CMD:
      assert (dram_state[channel][rank][bank].state == ROW_ACTIVE
          || dram_state[channel][rank][bank].state == PRECHARGING
          || dram_state[channel][rank][bank].state == IDLE
          || dram_state[channel][rank][bank].state == REFRESHING);

      //UT_MEM_DEBUG("\nCycle: %lld Cmd:PRE Req:%lld Chan:%d Rank:%d Bank:%d \n", CYCLE_VAL, request->id, channel, rank, bank);
      dram_state[channel][rank][bank].state = PRECHARGING;
      dram_state[channel][rank][bank].active_row = -1;
      dram_state[channel][rank][bank].next_act =
        max (cycle + T_RP, dram_state[channel][rank][bank].next_act);
      dram_state[channel][rank][bank].next_powerdown =
        max (cycle + T_RP, dram_state[channel][rank][bank].next_powerdown);
      dram_state[channel][rank][bank].next_pre =
        max (cycle + T_RP, dram_state[channel][rank][bank].next_pre);
      dram_state[channel][rank][bank].next_refresh =
        max (cycle + T_RP, dram_state[channel][rank][bank].next_refresh);
      stats_num_precharge[channel][rank][bank]++;
      command_issued_current_cycle[channel] = 1;
      break;
    case PWR_UP_CMD:
      assert (dram_state[channel][rank][bank].state ==
          PRECHARGE_POWER_DOWN_SLOW
          || dram_state[channel][rank][bank].state ==
          PRECHARGE_POWER_DOWN_FAST
          || dram_state[channel][rank][bank].state ==
          ACTIVE_POWER_DOWN);

      //UT_MEM_DEBUG("\nCycle: %lld Cmd: PWR_UP_CMD Chan:%d Rank:%d \n", CYCLE_VAL, channel, rank);
      for (int i = 0; i < NUM_BANKS; i++)

      {
        if (dram_state[channel][rank][i].state ==
            PRECHARGE_POWER_DOWN_SLOW
            || dram_state[channel][rank][i].state ==
            PRECHARGE_POWER_DOWN_FAST)

        {
          dram_state[channel][rank][i].state = IDLE;
          dram_state[channel][rank][i].active_row = -1;
        }

        else

        {
          dram_state[channel][rank][i].state = ROW_ACTIVE;
        }
        if (dram_state[channel][rank][i].state ==
            PRECHARGE_POWER_DOWN_SLOW)

        {
          dram_state[channel][rank][i].next_powerdown =
            max (cycle + T_XP_DLL,
                dram_state[channel][rank][i].next_powerdown);
          dram_state[channel][rank][i].next_pre =
            max (cycle + T_XP_DLL, dram_state[channel][rank][i].next_pre);
          dram_state[channel][rank][i].next_read =
            max (cycle + T_XP_DLL,
                dram_state[channel][rank][i].next_read);
          dram_state[channel][rank][i].next_write =
            max (cycle + T_XP_DLL,
                dram_state[channel][rank][i].next_write);
          dram_state[channel][rank][i].next_act =
            max (cycle + T_XP_DLL, dram_state[channel][rank][i].next_act);
          dram_state[channel][rank][i].next_refresh =
            max (cycle + T_XP_DLL,
                dram_state[channel][rank][i].next_refresh);
        }

        else

        {
          dram_state[channel][rank][i].next_powerdown =
            max (cycle + T_XP,
                dram_state[channel][rank][i].next_powerdown);
          dram_state[channel][rank][i].next_pre =
            max (cycle + T_XP, dram_state[channel][rank][i].next_pre);
          dram_state[channel][rank][i].next_read =
            max (cycle + T_XP, dram_state[channel][rank][i].next_read);
          dram_state[channel][rank][i].next_write =
            max (cycle + T_XP, dram_state[channel][rank][i].next_write);
          dram_state[channel][rank][i].next_act =
            max (cycle + T_XP, dram_state[channel][rank][i].next_act);
          dram_state[channel][rank][i].next_refresh =
            max (cycle + T_XP, dram_state[channel][rank][i].next_refresh);
        }
      }
      stats_num_powerup[channel][rank]++;
      command_issued_current_cycle[channel] = 1;
      break;
    case NOP:

      //UT_MEM_DEBUG("\nCycle: %lld Cmd: NOP Chan:%d\n", CYCLE_VAL, channel);
      break;
    default:
      break;
  }
  return 1;
}


// Function called to see if the rank can be transitioned into a fast low
// power state - ACT_PDN or PRE_PDN_FAST. 
  int
is_powerdown_fast_allowed (int channel, int rank) 
{
  int flag = 0;

  // if already a command has been issued this cycle, or if
  // forced refreshes are underway, or if issuing this command
  // will cause us to miss the refresh deadline, do not allow it
  if (command_issued_current_cycle[channel]
      || forced_refresh_mode_on[channel][rank]
      || (CYCLE_VAL + T_PD_MIN + T_XP >
        refresh_issue_deadline[channel][rank]))
    return 0;

  // command can be allowed if the next_powerdown is met for all banks in the rank
  for (int i = 0; i < NUM_BANKS; i++)

  {
    if ((dram_state[channel][rank][i].state == PRECHARGING
          || dram_state[channel][rank][i].state == ROW_ACTIVE
          || dram_state[channel][rank][i].state == IDLE
          || dram_state[channel][rank][i].state == REFRESHING)
        && CYCLE_VAL >= dram_state[channel][rank][i].next_powerdown)
      flag = 1;

    else
      return 0;
  }
  return flag;
}


// Function to see if the rank can be transitioned into a slow low
// power state - i.e. PRE_PDN_SLOW
  int
is_powerdown_slow_allowed (int channel, int rank) 
{
  int flag = 0;
  if (command_issued_current_cycle[channel]
      || forced_refresh_mode_on[channel][rank]
      || (CYCLE_VAL + T_PD_MIN + T_XP_DLL >
        refresh_issue_deadline[channel][rank]))
    return 0;

  // Sleep command can be allowed if the next_powerdown is met for all banks in the rank
  // and if all the banks are precharged
  for (int i = 0; i < NUM_BANKS; i++)

  {
    if (dram_state[channel][rank][i].state == ROW_ACTIVE)
      return 0;

    else

    {
      if ((dram_state[channel][rank][i].state == PRECHARGING
            || dram_state[channel][rank][i].state == IDLE
            || dram_state[channel][rank][i].state == REFRESHING)
          && CYCLE_VAL >= dram_state[channel][rank][i].next_powerdown)
        flag = 1;

      else
        return 0;
    }
  }
  return flag;
}


// Function to see if the rank can be powered up
  int
is_powerup_allowed (int channel, int rank) 
{
  if (command_issued_current_cycle[channel]
      || forced_refresh_mode_on[channel][rank])
    return 0;
  if (((dram_state[channel][rank][0].state == PRECHARGE_POWER_DOWN_SLOW)
        || (dram_state[channel][rank][0].state == PRECHARGE_POWER_DOWN_FAST)
        || (dram_state[channel][rank][0].state == ACTIVE_POWER_DOWN))
      && (CYCLE_VAL >= dram_state[channel][rank][0].next_powerup))

  {

    // check if issuing it will cause us to miss the refresh
    // deadline. If it does, don't allow it. The forced
    // refreshes will issue an implicit power up anyway
    if ((dram_state[channel][rank][0].state == PRECHARGE_POWER_DOWN_SLOW)
        && ((CYCLE_VAL + T_XP_DLL) > refresh_issue_deadline[channel][0]))
      return 0;
    if (((dram_state[channel][rank][0].state == PRECHARGE_POWER_DOWN_FAST)
          || (dram_state[channel][rank][0].state == ACTIVE_POWER_DOWN))
        && ((CYCLE_VAL + T_XP) > refresh_issue_deadline[channel][0]))
      return 0;
    return 1;
  }

  else
    return 0;
}


// Function to see if the bank can be activated or not
  int
is_activate_allowed (int channel, int rank, int bank) 
{
  if (command_issued_current_cycle[channel]
      || forced_refresh_mode_on[channel][rank]
      || (CYCLE_VAL + T_RAS > refresh_issue_deadline[channel][rank]))
    return 0;
  if ((dram_state[channel][rank][bank].state == IDLE
        || dram_state[channel][rank][bank].state == PRECHARGING
        || dram_state[channel][rank][bank].state == REFRESHING)
      && (CYCLE_VAL >= dram_state[channel][rank][bank].next_act)
      && (is_T_FAW_met (channel, rank, CYCLE_VAL)))
    return 1;

  else
    return 0;
}


// Function to see if the rank can be precharged or not
  int
is_autoprecharge_allowed (int channel, int rank, int bank) 
{
  long long int start_precharge = 0;
  if (cas_issued_current_cycle[channel][rank][bank] == 1)
    start_precharge =
      max (CYCLE_VAL + T_RTP, dram_state[channel][rank][bank].next_pre);

  else
    start_precharge =
      max (CYCLE_VAL + T_CWD + T_DATA_TRANS + T_WR,
          dram_state[channel][rank][bank].next_pre);
  if (((cas_issued_current_cycle[channel][rank][bank] == 1)
        && ((start_precharge + T_RP) <=
          refresh_issue_deadline[channel][rank]))
      || ((cas_issued_current_cycle[channel][rank][bank] == 2)
        && ((start_precharge + T_RP) <=
          refresh_issue_deadline[channel][rank])))
    return 1;

  else
    return 0;
}


// Function to see if the rank can be precharged or not
  int
is_precharge_allowed (int channel, int rank, int bank) 
{
  if (command_issued_current_cycle[channel]
      || forced_refresh_mode_on[channel][rank]
      || (CYCLE_VAL + T_RP > refresh_issue_deadline[channel][rank]))
    return 0;
  if ((dram_state[channel][rank][bank].state == ROW_ACTIVE
        || dram_state[channel][rank][bank].state == IDLE
        || dram_state[channel][rank][bank].state == PRECHARGING
        || dram_state[channel][rank][bank].state == REFRESHING)
      && (CYCLE_VAL >= dram_state[channel][rank][bank].next_pre))
    return 1;

  else
    return 0;
}


// function to see if all banks can be precharged this cycle
  int
is_all_bank_precharge_allowed (int channel, int rank) 
{
  int flag = 0;
  if (command_issued_current_cycle[channel]
      || forced_refresh_mode_on[channel][rank]
      || (CYCLE_VAL + T_RP > refresh_issue_deadline[channel][rank]))
    return 0;
  for (int i = 0; i < NUM_BANKS; i++)

  {
    if ((dram_state[channel][rank][i].state == ROW_ACTIVE
          || dram_state[channel][rank][i].state == IDLE
          || dram_state[channel][rank][i].state == PRECHARGING
          || dram_state[channel][rank][i].state == REFRESHING)
        && (CYCLE_VAL >= dram_state[channel][rank][i].next_pre))
      flag = 1;

    else
      return 0;
  }
  return flag;
}


// function to see if refresh can be allowed this cycle
  int
is_refresh_allowed (int channel, int rank) 
{
  if (command_issued_current_cycle[channel]
      || forced_refresh_mode_on[channel][rank])
    return 0;
  for (int b = 0; b < NUM_BANKS; b++)

  {
    if (CYCLE_VAL < dram_state[channel][rank][b].next_refresh)
      return 0;
  }
  return 1;
}


// Function to put a rank into the low power mode
  int
issue_powerdown_command (int channel, int rank, command_t cmd) 
{
  if (command_issued_current_cycle[channel])
  {
    printf
      ("PANIC : SCHED_ERROR: Got beat. POWER_DOWN command not issuable in cycle:%lld\n",
       CYCLE_VAL);
    return 0;
  }

  // if right CMD has been used
  if ((cmd != PWR_DN_FAST_CMD) && (cmd != PWR_DN_SLOW_CMD))

  {
    printf
      ("PANIC: SCHED_ERROR : Only PWR_DN_SLOW_CMD or PWR_DN_FAST_CMD can be used to put DRAM rank to sleep\n");
    return 0;
  }

  // if the powerdown command can indeed be issued
  if (((cmd == PWR_DN_FAST_CMD)
        && !is_powerdown_fast_allowed (channel, rank))
      || ((cmd == PWR_DN_SLOW_CMD)
        && !is_powerdown_slow_allowed (channel, rank)))

  {
    printf
      ("PANIC : SCHED_ERROR: POWER_DOWN command not issuable in cycle:%lld\n",
       CYCLE_VAL);
    return 0;
  }
  for (int i = 0; i < NUM_BANKS; i++)

  {

    // next_powerup and refresh times
    dram_state[channel][rank][i].next_powerup =
      max (CYCLE_VAL + T_PD_MIN,
          dram_state[channel][rank][i].next_powerdown);
    dram_state[channel][rank][i].next_refresh =
      max (CYCLE_VAL + T_PD_MIN, dram_state[channel][rank][i].next_refresh);

    // state change
    if (dram_state[channel][rank][i].state == IDLE
        || dram_state[channel][rank][i].state == PRECHARGING
        || dram_state[channel][rank][i].state == REFRESHING)

    {
      if (cmd == PWR_DN_SLOW_CMD)

      {
        dram_state[channel][rank][i].state = PRECHARGE_POWER_DOWN_SLOW;
        stats_num_powerdown_slow[channel][rank]++;
      }

      else if (cmd == PWR_DN_FAST_CMD)

      {
        dram_state[channel][rank][i].state = PRECHARGE_POWER_DOWN_FAST;
        stats_num_powerdown_fast[channel][rank]++;
      }
      dram_state[channel][rank][i].active_row = -1;
    }

    else if (dram_state[channel][rank][i].state == ROW_ACTIVE)

    {
      dram_state[channel][rank][i].state = ACTIVE_POWER_DOWN;
    }
  }
  command_issued_current_cycle[channel] = 1;
  return 1;
}


// Function to power a rank up
  int
issue_powerup_command (int channel, int rank) 
{
  if (!is_powerup_allowed (channel, rank))

  {
    printf
      ("PANIC : SCHED_ERROR: POWER_UP command not issuable in cycle:%lld\n",
       CYCLE_VAL);
    return 0;
  }

  else

  {
    long long int cycle = CYCLE_VAL;
    for (int i = 0; i < NUM_BANKS; i++)

    {
      if (dram_state[channel][rank][i].state ==
          PRECHARGE_POWER_DOWN_SLOW
          || dram_state[channel][rank][i].state ==
          PRECHARGE_POWER_DOWN_FAST)

      {
        dram_state[channel][rank][i].state = IDLE;
        dram_state[channel][rank][i].active_row = -1;
      }

      else

      {
        dram_state[channel][rank][i].state = ROW_ACTIVE;
      }
      if (dram_state[channel][rank][i].state ==
          PRECHARGE_POWER_DOWN_SLOW)

      {
        dram_state[channel][rank][i].next_powerdown =
          max (cycle + T_XP_DLL,
              dram_state[channel][rank][i].next_powerdown);
        dram_state[channel][rank][i].next_pre =
          max (cycle + T_XP_DLL, dram_state[channel][rank][i].next_pre);
        dram_state[channel][rank][i].next_read =
          max (cycle + T_XP_DLL,
              dram_state[channel][rank][i].next_read);
        dram_state[channel][rank][i].next_write =
          max (cycle + T_XP_DLL,
              dram_state[channel][rank][i].next_write);
        dram_state[channel][rank][i].next_act =
          max (cycle + T_XP_DLL, dram_state[channel][rank][i].next_act);
        dram_state[channel][rank][i].next_refresh =
          max (cycle + T_XP_DLL,
              dram_state[channel][rank][i].next_refresh);
      }

      else

      {
        dram_state[channel][rank][i].next_powerdown =
          max (cycle + T_XP,
              dram_state[channel][rank][i].next_powerdown);
        dram_state[channel][rank][i].next_pre =
          max (cycle + T_XP, dram_state[channel][rank][i].next_pre);
        dram_state[channel][rank][i].next_read =
          max (cycle + T_XP, dram_state[channel][rank][i].next_read);
        dram_state[channel][rank][i].next_write =
          max (cycle + T_XP, dram_state[channel][rank][i].next_write);
        dram_state[channel][rank][i].next_act =
          max (cycle + T_XP, dram_state[channel][rank][i].next_act);
        dram_state[channel][rank][i].next_refresh =
          max (cycle + T_XP, dram_state[channel][rank][i].next_refresh);
      }
    }
    command_issued_current_cycle[channel] = 1;
    return 1;
  }
}


// Function to issue a precharge command to a specific bank
  int
issue_autoprecharge (int channel, int rank, int bank) 
{
  if (!is_autoprecharge_allowed (channel, rank, bank))
    return 0;

  else

  {
    long long int start_precharge = 0;
    dram_state[channel][rank][bank].active_row = -1;
    dram_state[channel][rank][bank].state = PRECHARGING;
    if (cas_issued_current_cycle[channel][rank][bank] == 1)
      start_precharge =
        max (CYCLE_VAL + T_RTP, dram_state[channel][rank][bank].next_pre);

    else
      start_precharge =
        max (CYCLE_VAL + T_CWD + T_DATA_TRANS + T_WR,
            dram_state[channel][rank][bank].next_pre);
    dram_state[channel][rank][bank].next_act =
      max (start_precharge + T_RP,
          dram_state[channel][rank][bank].next_act);
    dram_state[channel][rank][bank].next_powerdown =
      max (start_precharge + T_RP,
          dram_state[channel][rank][bank].next_powerdown);
    dram_state[channel][rank][bank].next_pre =
      max (start_precharge + T_RP,
          dram_state[channel][rank][bank].next_pre);
    dram_state[channel][rank][bank].next_refresh =
      max (start_precharge + T_RP,
          dram_state[channel][rank][bank].next_refresh);
    stats_num_precharge[channel][rank][bank]++;

    // reset the cas_issued_current_cycle 
    for (int r = 0; r < NUM_RANKS; r++)
      for (int b = 0; b < NUM_BANKS; b++)
        cas_issued_current_cycle[channel][r][b] = 0;
    return 1;
  }
}


// Function to issue an activate command to a specific row
  int
issue_activate_command (int channel, int rank, int bank, long long int row) 
{
  if (!is_activate_allowed (channel, rank, bank))

  {
    printf
      ("PANIC : SCHED_ERROR: ACTIVATE command not issuable in cycle:%lld\n",
       CYCLE_VAL);
    return 0;
  }

  else

  {
    long long int cycle = CYCLE_VAL;
    dram_state[channel][rank][bank].state = ROW_ACTIVE;
    dram_state[channel][rank][bank].active_row = row;
    dram_state[channel][rank][bank].next_pre =
      max ((cycle + T_RAS), dram_state[channel][rank][bank].next_pre);
    dram_state[channel][rank][bank].next_refresh =
      max ((cycle + T_RAS), dram_state[channel][rank][bank].next_refresh);
    dram_state[channel][rank][bank].next_read =
      max (cycle + T_RCD, dram_state[channel][rank][bank].next_read);
    dram_state[channel][rank][bank].next_write =
      max (cycle + T_RCD, dram_state[channel][rank][bank].next_write);
    dram_state[channel][rank][bank].next_act =
      max (cycle + T_RC, dram_state[channel][rank][bank].next_act);
    dram_state[channel][rank][bank].next_powerdown =
      max (cycle + T_RCD, dram_state[channel][rank][bank].next_powerdown);
    for (int i = 0; i < NUM_BANKS; i++)
      if (i != bank)
        dram_state[channel][rank][i].next_act =
          max (cycle + T_RRD, dram_state[channel][rank][i].next_act);
    record_activate (channel, rank, cycle);
    stats_num_activate[channel][rank]++;
    stats_num_activate_spec[channel][rank][bank]++;
    average_gap_between_activates[channel][rank] =
      ((average_gap_between_activates[channel][rank] *
        (stats_num_activate[channel][rank] - 1)) + (CYCLE_VAL -
          last_activate[channel]
          [rank])) /
      stats_num_activate[channel][rank];
    last_activate[channel][rank] = CYCLE_VAL;
    command_issued_current_cycle[channel] = 1;
    return 1;
  }
}


// Function to issue a precharge command to a specific bank
  int
issue_precharge_command (int channel, int rank, int bank) 
{
  if (!is_precharge_allowed (channel, rank, bank))

  {
    printf
      ("PANIC : SCHED_ERROR: PRECHARGE command not issuable in cycle:%lld\n",
       CYCLE_VAL);
    return 0;
  }

  else

  {
    dram_state[channel][rank][bank].state = PRECHARGING;
    dram_state[channel][rank][bank].active_row = -1;
    dram_state[channel][rank][bank].next_act =
      max (CYCLE_VAL + T_RP, dram_state[channel][rank][bank].next_act);
    dram_state[channel][rank][bank].next_powerdown =
      max (CYCLE_VAL + T_RP,
          dram_state[channel][rank][bank].next_powerdown);
    dram_state[channel][rank][bank].next_pre =
      max (CYCLE_VAL + T_RP, dram_state[channel][rank][bank].next_pre);
    dram_state[channel][rank][bank].next_refresh =
      max (CYCLE_VAL + T_RP, dram_state[channel][rank][bank].next_refresh);
    stats_num_precharge[channel][rank][bank]++;
    command_issued_current_cycle[channel] = 1;
    return 1;
  }
}


// Function to precharge a rank
  int
issue_all_bank_precharge_command (int channel, int rank) 
{
  if (!is_all_bank_precharge_allowed (channel, rank))

  {
    printf
      ("PANIC : SCHED_ERROR: ALL_BANK_PRECHARGE command not issuable in cycle:%lld\n",
       CYCLE_VAL);
    return 0;
  }

  else

  {
    for (int i = 0; i < NUM_BANKS; i++)

    {
      issue_precharge_command (channel, rank, i);
      command_issued_current_cycle[channel] = 0;	/* Since issue_precharge_command would have set this, we need to reset it. */
    } command_issued_current_cycle[channel] = 1;
    return 1;
  }
}


// Function to issue a refresh
  int
issue_refresh_command (int channel, int rank) 
{
  if (!is_refresh_allowed (channel, rank))

  {
    printf
      ("PANIC : SCHED_ERROR: REFRESH command not issuable in cycle:%lld\n",
       CYCLE_VAL);
    return 0;
  }

  else

  {
    num_issued_refreshes[channel][rank]++;
    long long int cycle = CYCLE_VAL;
    if (dram_state[channel][rank][0].state == PRECHARGE_POWER_DOWN_SLOW)

    {
      for (int b = 0; b < NUM_BANKS; b++)

      {
        dram_state[channel][rank][b].next_act =
          max (cycle + T_XP_DLL + T_RFC,
              dram_state[channel][rank][b].next_act);
        dram_state[channel][rank][b].next_pre =
          max (cycle + T_XP_DLL + T_RFC,
              dram_state[channel][rank][b].next_pre);
        dram_state[channel][rank][b].next_refresh =
          max (cycle + T_XP_DLL + T_RFC,
              dram_state[channel][rank][b].next_refresh);
        dram_state[channel][rank][b].next_powerdown =
          max (cycle + T_XP_DLL + T_RFC,
              dram_state[channel][rank][b].next_powerdown);
      } }

    else if (dram_state[channel][rank][0].state ==
        PRECHARGE_POWER_DOWN_FAST)

    {
      for (int b = 0; b < NUM_BANKS; b++)

      {
        dram_state[channel][rank][b].next_act =
          max (cycle + T_XP + T_RFC,
              dram_state[channel][rank][b].next_act);
        dram_state[channel][rank][b].next_pre =
          max (cycle + T_XP + T_RFC,
              dram_state[channel][rank][b].next_pre);
        dram_state[channel][rank][b].next_refresh =
          max (cycle + T_XP + T_RFC,
              dram_state[channel][rank][b].next_refresh);
        dram_state[channel][rank][b].next_powerdown =
          max (cycle + T_XP + T_RFC,
              dram_state[channel][rank][b].next_powerdown);
      } }

    else if (dram_state[channel][rank][0].state == ACTIVE_POWER_DOWN)

    {
      for (int b = 0; b < NUM_BANKS; b++)

      {
        dram_state[channel][rank][b].next_act =
          max (cycle + T_XP + T_RP + T_RFC,
              dram_state[channel][rank][b].next_act);
        dram_state[channel][rank][b].next_pre =
          max (cycle + T_XP + T_RP + T_RFC,
              dram_state[channel][rank][b].next_pre);
        dram_state[channel][rank][b].next_refresh =
          max (cycle + T_XP + T_RP + T_RFC,
              dram_state[channel][rank][b].next_refresh);
        dram_state[channel][rank][b].next_powerdown =
          max (cycle + T_XP + T_RP + T_RFC,
              dram_state[channel][rank][b].next_powerdown);
      } }

    else			// rank powered up
    {
      int flag = 0;
      for (int b = 0; b < NUM_BANKS; b++)

      {
        if (dram_state[channel][rank][b].state == ROW_ACTIVE)

        {
          flag = 1;
          break;
        }
      }
      if (flag)		// at least a single bank is open
      {
        for (int b = 0; b < NUM_BANKS; b++)

        {
          dram_state[channel][rank][b].next_act =
            max (cycle + T_RP + T_RFC,
                dram_state[channel][rank][b].next_act);
          dram_state[channel][rank][b].next_pre =
            max (cycle + T_RP + T_RFC,
                dram_state[channel][rank][b].next_pre);
          dram_state[channel][rank][b].next_refresh =
            max (cycle + T_RP + T_RFC,
                dram_state[channel][rank][b].next_refresh);
          dram_state[channel][rank][b].next_powerdown =
            max (cycle + T_RP + T_RFC,
                dram_state[channel][rank][b].next_powerdown);
        } }

      else			// everything precharged
      {
        for (int b = 0; b < NUM_BANKS; b++)

        {
          dram_state[channel][rank][b].next_act =
            max (cycle + T_RFC,
                dram_state[channel][rank][b].next_act);
          dram_state[channel][rank][b].next_pre =
            max (cycle + T_RFC,
                dram_state[channel][rank][b].next_pre);
          dram_state[channel][rank][b].next_refresh =
            max (cycle + T_RFC,
                dram_state[channel][rank][b].next_refresh);
          dram_state[channel][rank][b].next_powerdown =
            max (cycle + T_RFC,
                dram_state[channel][rank][b].next_powerdown);
        } }  } for (int b = 0; b < NUM_BANKS; b++)

    {
      dram_state[channel][rank][b].active_row = -1;
      dram_state[channel][rank][b].state = REFRESHING;
    } command_issued_current_cycle[channel] = 1;
    return 1;
  }
}

  void
issue_forced_refresh_commands (int channel, int rank) 
{
  for (int b = 0; b < NUM_BANKS; b++)

  {
    dram_state[channel][rank][b].state = REFRESHING;
    dram_state[channel][rank][b].active_row = -1;
    dram_state[channel][rank][b].next_act =
      next_refresh_completion_deadline[channel][rank];
    dram_state[channel][rank][b].next_pre =
      next_refresh_completion_deadline[channel][rank];
    dram_state[channel][rank][b].next_refresh =
      next_refresh_completion_deadline[channel][rank];
    dram_state[channel][rank][b].next_powerdown =
      next_refresh_completion_deadline[channel][rank];
  } }   void

gather_stats (int channel) 
{
  for (int i = 0; i < NUM_RANKS; i++)

  {
    if (dram_state[channel][i][0].state == PRECHARGE_POWER_DOWN_SLOW)
      stats_time_spent_in_precharge_power_down_slow[channel][i] +=
        PROCESSOR_CLK_MULTIPLIER;

    else if (dram_state[channel][i][0].state == PRECHARGE_POWER_DOWN_FAST)
      stats_time_spent_in_precharge_power_down_fast[channel][i] +=
        PROCESSOR_CLK_MULTIPLIER ;

    else if (dram_state[channel][i][0].state == ACTIVE_POWER_DOWN)
      stats_time_spent_in_active_power_down[channel][i] +=
        PROCESSOR_CLK_MULTIPLIER;

    else

    {
      for (int b = 0; b < NUM_BANKS; b++)
      {
        if (dram_state[channel][i][b].state == ROW_ACTIVE)

        {
          stats_time_spent_in_active_standby[channel][i] +=
            PROCESSOR_CLK_MULTIPLIER;
          break;
        }
      }
      stats_time_spent_in_power_up[channel][i] +=
        PROCESSOR_CLK_MULTIPLIER;
    }
  }
}

  void
print_stats (int channel) 
{
  long long int activates_for_reads = 0;
  long long int activates_for_spec = 0;
  long long int activates_for_writes = 0;
  long long int read_cmds = 0;
  long long int write_cmds = 0;
  for (int c = 0; c < NUM_CHANNELS; c++)

  {
    activates_for_writes = 0;
    activates_for_reads = 0;
    activates_for_spec = 0;
    read_cmds = 0;
    write_cmds = 0;
    for (int r = 0; r < NUM_RANKS; r++)

    {
      for (int b = 0; b < NUM_BANKS; b++)

      {
        activates_for_writes += stats_num_activate_write[c][r][b];
        activates_for_reads += stats_num_activate_read[c][r][b];
        activates_for_spec += stats_num_activate_spec[c][r][b];
        read_cmds += stats_num_read[c][r][b];
        write_cmds += stats_num_write[c][r][b];
      } }  printf ("-------- Channel %d Stats-----------\n", c);
    printf ("Total Reads Serviced :          %-7lld\n",
        stats_reads_completed[c]);
    printf ("Total Writes Serviced :         %-7lld\n",
        stats_writes_completed[c]);
    printf ("Average Read Latency :          %7.5f\n",
        (double) stats_average_read_latency[c]);
    printf ("Average Read Queue Latency :    %7.5f\n",
        (double) stats_average_read_queue_latency[c]);
    printf ("Average Write Latency :         %7.5f\n",
        (double) stats_average_write_latency[c]);
    printf ("Average Write Queue Latency :   %7.5f\n",
        (double) stats_average_write_queue_latency[c]);
    printf ("Read Page Hit Rate :            %7.5f\n",
        ((double)
         (read_cmds - activates_for_reads -
          activates_for_spec) / read_cmds));
    printf ("Write Page Hit Rate :           %7.5f\n",
        ((double) (write_cmds - activates_for_writes) / write_cmds));
    printf ("------------------------------------\n");
  } }  void

update_issuable_commands (int channel) 
{
  for (int rank = 0; rank < NUM_RANKS; rank++)

  {
    for (int bank = 0; bank < NUM_BANKS; bank++)
      cmd_precharge_issuable[channel][rank][bank] =
        is_precharge_allowed (channel, rank, bank);
    cmd_all_bank_precharge_issuable[channel][rank] =
      is_all_bank_precharge_allowed (channel, rank);
    cmd_powerdown_fast_issuable[channel][rank] =
      is_powerdown_fast_allowed (channel, rank);
    cmd_powerdown_slow_issuable[channel][rank] =
      is_powerdown_slow_allowed (channel, rank);
    cmd_refresh_issuable[channel][rank] =
      is_refresh_allowed (channel, rank);
    cmd_powerup_issuable[channel][rank] =
      is_powerup_allowed (channel, rank);
  } }  

// function that updates the dram state and schedules auto-refresh if
// necessary. This is called every DRAM cycle
  void
update_memory () 
{
  for (int channel = 0; channel < NUM_CHANNELS; channel++)

  {

    // make every channel ready to receive a new command
    command_issued_current_cycle[channel] = 0;
    for (int rank = 0; rank < NUM_RANKS; rank++)

    {

      //reset variable
      for (int bank = 0; bank < NUM_BANKS; bank++)
        cas_issued_current_cycle[channel][rank][bank] = 0;

      // clean out the activate record for
      // CYCLE_VAL - T_FAW
      flush_activate_record (channel, rank, CYCLE_VAL);

      // if we are at the refresh completion
      // deadline
      if (CYCLE_VAL == next_refresh_completion_deadline[channel][rank])

      {

        // calculate the next
        // refresh_issue_deadline
        num_issued_refreshes[channel][rank] = 0;
        last_refresh_completion_deadline[channel][rank] = CYCLE_VAL;
        next_refresh_completion_deadline[channel][rank] =
          CYCLE_VAL + 8 * T_REFI;
        refresh_issue_deadline[channel][rank] =
          next_refresh_completion_deadline[channel][rank] - T_RP -
          8 * T_RFC;
        forced_refresh_mode_on[channel][rank] = 0;
        issued_forced_refresh_commands[channel][rank] = 0;
      }

      else if ((CYCLE_VAL == refresh_issue_deadline[channel][rank])
          && (num_issued_refreshes[channel][rank] < 8))

      {

        // refresh_issue_deadline has been
        // reached. Do the auto-refreshes
        forced_refresh_mode_on[channel][rank] = 1;
        issue_forced_refresh_commands (channel, rank);
      }

      else if (CYCLE_VAL < refresh_issue_deadline[channel][rank])

      {

        //update the refresh_issue deadline
        refresh_issue_deadline[channel][rank] =
          next_refresh_completion_deadline[channel][rank] - T_RP - (8 -
              num_issued_refreshes
              [channel]
              [rank])
          * T_RFC;
      }
    }

    // update the variables corresponding to the non-queue
    // variables
    update_issuable_commands (channel);

    // update the request cmds in the queues
    update_read_queue_commands (channel);
    update_write_queue_commands (channel);

    // remove finished requests
    clean_queues (channel);
  }
}


//------------------------------------------------------------
// Calculate Power: It calculates and returns average power used by every Rank on Every 
// Channel during the course of the simulation 
// Units : Time- ns; Current mA; Voltage V; Power mW; 
//------------------------------------------------------------
  float
calculate_power (int channel, int rank, int print_stats_type,
    int chips_per_rank) 
{

  /*
     Power is calculated using the equations from Technical Note "TN-41-01: Calculating Memory System Power for DDR"
     The current values IDD* are taken from the data sheets. 
     These are average current values that the chip will draw when certain actions occur as frequently as possible.
     i.e., the worst case power consumption
Eg: when ACTs happen every tRC
pds_<component> is the power calculated by directly using the current values from the data sheet. 'pds' stands for 
PowerDataSheet. This will the power drawn by the chip when operating under the activity that is assumed in the data 
sheet. This mostly represents the worst case power
These pds_<*> components need to be derated in accordance with the activity that is observed. Eg: If ACTs occur slower
than every tRC, then pds_act will be derated to give "psch_act" (SCHeduled Power consumed by Activate) 
*/ 

  /*------------------------------------------------------------
  // total_power is the sum of of 13 components listed below 
  // Note: CKE is the ClocK Enable to every chip.
  // Note: Even though the reads and write are to a different rank on the same channel, the Pull-Up and the Pull-Down resistors continue 
  // 		to burn some power. psch_termWoth and psch_termWoth stand for the power dissipated in the rank in question when the reads and
  // 		writes are to other ranks on the channel

  psch_act 						-> Power dissipated by activating a row
  psch_act_pdn 				-> Power dissipated when CKE is low (disabled) and all banks are precharged
  psch_act_stby 			-> Power dissipated when CKE is high (enabled) and at least one back is active (row is open)
  psch_pre_pdn_fast  	-> Power dissipated when CKE is low (disabled) and all banks are precharged and chip is in fast power down
  psch_pre_pdn_slow  	-> Power dissipated when CKE is low (disabled) and all banks are precharged and chip is in fast slow  down
  psch_pre_stby 			-> Power dissipated when CKE is high (enabled) and at least one back is active (row is open)
  psch_termWoth 			-> Power dissipated when a Write termiantes at the other set of chips.
  psch_termRoth 			-> Power dissipated when a Read  termiantes at the other set of chips
  psch_termW 					-> Power dissipated when a Write termiantes at the set of chips in question
  psch_dq 						-> Power dissipated when a Read  termiantes at the set of chips in question (Data Pins on the chip are called DQ)
  psch_ref 						-> Power dissipated during Refresh
  psch_rd 						-> Power dissipated during a Read  (does ot include power to open a row)
  psch_wr 						-> Power dissipated during a Write (does ot include power to open a row)

  ------------------------------------------------------------*/ 
  float pds_act;
  float pds_act_pdn;
  float pds_act_stby;
  float pds_pre_pdn_fast;
  float pds_pre_pdn_slow;
  float pds_pre_stby;
  float pds_wr;
  float pds_rd;
  float pds_ref;
  float pds_dq;
  float pds_termW;
  float pds_termRoth;
  float pds_termWoth;
  float psch_act;
  float psch_pre_pdn_slow;
  float psch_pre_pdn_fast;
  float psch_act_pdn;
  float psch_act_stby;
  float psch_pre_stby;
  float psch_rd;
  float psch_wr;
  float psch_ref;
  float psch_dq;
  float psch_termW;
  float psch_termRoth;
  float psch_termWoth;
  float total_chip_power;
  float total_rank_power;
  long long int writes = 0, reads = 0;
  static int print_total_cycles = 0;

  /*----------------------------------------------------
  //Calculating DataSheet Power
  ----------------------------------------------------*/ 
  pds_act = (IDD0 - (IDD3N * T_RAS + IDD2N * (T_RC - T_RAS)) / T_RC) * VDD;
  pds_pre_pdn_slow = IDD2P0 * VDD;
  pds_pre_pdn_fast = IDD2P1 * VDD;
  pds_act_pdn = IDD3P * VDD;
  pds_pre_stby = IDD2N * VDD;
  pds_act_stby = IDD3N * VDD;
  pds_wr = (IDD4W - IDD3N) * VDD;
  pds_rd = (IDD4R - IDD3N) * VDD;
  pds_ref = (IDD5 - IDD3N) * VDD;

  /*----------------------------------------------------
  //On Die Termination (ODT) Power:
  //Values obtained from Micron Technical Note
  //This is dependent on the termination configuration of the simulated configuration
  //our simulator uses the same config as that used in the Tech Note
  ----------------------------------------------------*/ 
  pds_dq = 3.2 * 10;
  pds_termW = 0;
  pds_termRoth = 24.9 * 10;
  pds_termWoth = 20.8 * 11;

  /*----------------------------------------------------
  //Derating worst case power to represent system activity
  ----------------------------------------------------*/ 

  //average_gap_between_activates was initialised to 0. So if it is still
  //0, then no ACTs have happened to this rank.
  //Hence activate-power is also 0
  if (average_gap_between_activates[channel][rank] == 0)

  {
    psch_act = 0;
  }
  else
  {
    psch_act =
      pds_act * T_RC / (average_gap_between_activates[channel][rank]);
  }
  psch_act_pdn =
    pds_act_pdn *
    ((double) stats_time_spent_in_active_power_down[channel][rank] /
     CYCLE_VAL);
  psch_pre_pdn_slow =
    pds_pre_pdn_slow *
    ((double) stats_time_spent_in_precharge_power_down_slow[channel][rank] /
     CYCLE_VAL);
  psch_pre_pdn_fast =
    pds_pre_pdn_fast *
    ((double) stats_time_spent_in_precharge_power_down_fast[channel][rank] /
     CYCLE_VAL);
  psch_act_stby =
    pds_act_stby *
    ((double) stats_time_spent_in_active_standby[channel][rank] / CYCLE_VAL);

  /*----------------------------------------------------
  //pds_pre_stby assumes that the system is powered up and every 
  //row has been precharged during every cycle 
  // In reality, the chip could have been in a power-down mode
  //or a row could have been active. The time spent in these modes 
  //should be deducted from total time
  ----------------------------------------------------*/ 
  psch_pre_stby =
    pds_pre_stby *
    ((double)
     (CYCLE_VAL - stats_time_spent_in_active_standby[channel][rank] -
      stats_time_spent_in_precharge_power_down_slow[channel][rank] -
      stats_time_spent_in_precharge_power_down_fast[channel][rank] -
      stats_time_spent_in_active_power_down[channel][rank])) / CYCLE_VAL;

  /*----------------------------------------------------
  //Calculate Total Reads ans Writes performed in the system
  ----------------------------------------------------*/ 
  for (int i = 0; i < NUM_BANKS; i++)

  {
    writes += stats_num_write[channel][rank][i];
    reads += stats_num_read[channel][rank][i];
  }  
  /*----------------------------------------------------
  // pds<rd/wr> assumes that there is rd/wr happening every cycle
  // T_DATA_TRANS is the number of cycles it takes for one rd/wr
  ----------------------------------------------------*/ 
  psch_wr = pds_wr * (writes * T_DATA_TRANS) / CYCLE_VAL;
  psch_rd = pds_rd * (reads * T_DATA_TRANS) / CYCLE_VAL;

  /*----------------------------------------------------
  //pds_ref assumes that there is always a refresh happening.
  //in reality, refresh consumes only T_RFC out of every t_REFI
  ----------------------------------------------------*/ 
  psch_ref = pds_ref * T_RFC / T_REFI;
  psch_dq = pds_dq * (reads * T_DATA_TRANS) / CYCLE_VAL;
  psch_termW = pds_termW * (writes * T_DATA_TRANS) / CYCLE_VAL;
  psch_termRoth =
    pds_termRoth *
    ((double)
     stats_time_spent_terminating_reads_from_other_ranks[channel][rank] /
     CYCLE_VAL);
  psch_termWoth =
    pds_termWoth *
    ((double)
     stats_time_spent_terminating_writes_to_other_ranks[channel][rank] /
     CYCLE_VAL);
  total_chip_power =
    psch_act + psch_termWoth + psch_termRoth + psch_termW + psch_dq +
    psch_ref + psch_rd + psch_wr + psch_pre_stby + psch_act_stby +
    psch_pre_pdn_fast + psch_pre_pdn_slow + psch_act_pdn;
  total_rank_power = total_chip_power * chips_per_rank;
  double time_in_pre_stby =
    (((double)
      (CYCLE_VAL - stats_time_spent_in_active_standby[channel][rank] -
       stats_time_spent_in_precharge_power_down_slow[channel][rank] -
       stats_time_spent_in_precharge_power_down_fast[channel][rank] -
       stats_time_spent_in_active_power_down[channel][rank])) / CYCLE_VAL);
  if (print_total_cycles == 0)
  {
    printf
      ("\n#-----------------------------Simulated Cycles Break-Up-------------------------------------------\n");
    printf
      ("Note:  1.(Read Cycles + Write Cycles + Read Other + Write Other) should add up to %% cycles during which\n");
    printf
      ("          the channel is busy. This should be the same for all Ranks on a Channel\n");
    printf
      ("       2.(PRE_PDN_FAST + PRE_PDN_SLOW + ACT_PDN + ACT_STBY + PRE_STBY) should add up to 100%%\n");
    printf
      ("       3.Power Down means Clock Enable, CKE = 0. In Standby mode, CKE = 1\n");
    printf
      ("#-------------------------------------------------------------------------------------------------\n");
    printf ("Total Simulation Cycles                      %11lld\n",
        CYCLE_VAL);
    printf
      ("---------------------------------------------------------------\n\n");
    print_total_cycles = 1;
  }
  if (print_stats_type == 0)
  {

    /*
       printf ("%3d %6d %13.2f %13.2f %13.2f %13.2f %15.2f %15.2f %15.2f %13.2f %11.2f \n",\
       channel,\
       rank,\
       (double)reads/CYCLE_VAL,\
       (double)writes/CYCLE_VAL,\
       psch_termRoth,\
       psch_termWoth,\
       ((double)stats_time_spent_in_precharge_power_down_fast[channel][rank]/CYCLE_VAL),\
       ((double)stats_time_spent_in_precharge_power_down_slow[channel][rank]/CYCLE_VAL),\
       ((double)stats_time_spent_in_active_power_down[channel][rank]/CYCLE_VAL),\
       ((double)stats_time_spent_in_active_standby[channel][rank]/CYCLE_VAL),\
       (((double)(CYCLE_VAL - stats_time_spent_in_active_standby[channel][rank]- stats_time_spent_in_precharge_power_down_slow[channel][rank] - stats_time_spent_in_precharge_power_down_fast[channel][rank] - stats_time_spent_in_active_power_down[channel][rank]))/CYCLE_VAL)
       );
       */ 
    printf
      ("Channel %d Rank %d Read Cycles(%%)           %9.2f # %% cycles the Rank performed a Read\n",
       channel, rank, (double) reads * T_DATA_TRANS / CYCLE_VAL);
    printf
      ("Channel %d Rank %d Write Cycles(%%)          %9.2f # %% cycles the Rank performed a Write\n",
       channel, rank, (double) writes * T_DATA_TRANS / CYCLE_VAL);
    printf
      ("Channel %d Rank %d Read Other(%%)            %9.2f # %% cycles other Ranks on the channel performed a Read\n",
       channel, rank,
       ((double)
       stats_time_spent_terminating_reads_from_other_ranks[channel][rank]
       / CYCLE_VAL));
    printf
      ("Channel %d Rank %d Write Other(%%)           %9.2f # %% cycles other Ranks on the channel performed a Write\n",
       channel, rank,
       ((double)
       stats_time_spent_terminating_writes_to_other_ranks[channel][rank]
       / CYCLE_VAL));
    printf
      ("Channel %d Rank %d PRE_PDN_FAST(%%)          %9.2f # %% cycles the Rank was in Fast Power Down and all Banks were Precharged\n",
       channel, rank,
       ((double)
       stats_time_spent_in_precharge_power_down_fast[channel][rank] /
       CYCLE_VAL));
    printf
      ("Channel %d Rank %d PRE_PDN_SLOW(%%)          %9.2f # %% cycles the Rank was in Slow Power Down and all Banks were Precharged\n",
       channel, rank,
       ((double)
       stats_time_spent_in_precharge_power_down_slow[channel][rank] /
       CYCLE_VAL));
    printf
      ("Channel %d Rank %d ACT_PDN(%%)               %9.2f # %% cycles the Rank was in Active Power Down and atleast one Bank was Active\n",
       channel, rank,
       ((double) stats_time_spent_in_active_power_down[channel][rank] /
       CYCLE_VAL));
    printf
      ("Channel %d Rank %d ACT_STBY(%%)              %9.2f # %% cycles the Rank was in Standby and atleast one bank was Active\n",
       channel, rank,
       ((double) stats_time_spent_in_active_standby[channel][rank] /
       CYCLE_VAL));
    printf
      ("Channel %d Rank %d PRE_STBY(%%)              %9.2f # %% cycles the Rank was in Standby and all Banks were Precharged\n",
       channel, rank, time_in_pre_stby);
    printf
      ("---------------------------------------------------------------\n\n");
  }
  else if (print_stats_type == 1)
  {

    /*----------------------------------------------------
    // Total Power is the sum total of all the components calculated above
    ----------------------------------------------------*/ 
    printf
      ("Channel %d Rank %d Background(mw)          %9.2f # depends only on Power Down time and time all banks were precharged\n",
       channel, rank,
       psch_act_pdn + psch_act_stby + psch_pre_pdn_slow +
       psch_pre_pdn_fast + psch_pre_stby);
    printf
      ("Channel %d Rank %d Act(mW)                 %9.2f # power spend bringing data to the row buffer\n",
       channel, rank, psch_act);
    printf
      ("Channel %d Rank %d Read(mW)                %9.2f # power spent doing a Read  after the Row Buffer is open\n",
       channel, rank, psch_rd);
    printf
      ("Channel %d Rank %d Write(mW)               %9.2f # power spent doing a Write after the Row Buffer is open\n",
       channel, rank, psch_wr);
    printf
      ("Channel %d Rank %d Read Terminate(mW)      %9.2f # power dissipated in ODT resistors during Read\n",
       channel, rank, psch_dq);
    printf
      ("Channel %d Rank %d Write Terminate(mW)     %9.2f # power dissipated in ODT resistors during Write\n",
       channel, rank, psch_termW);
    printf
      ("Channel %d Rank %d termRoth(mW)            %9.2f # power dissipated in ODT resistors during Reads  in other ranks\n",
       channel, rank, psch_termRoth);
    printf
      ("Channel %d Rank %d termWoth(mW)            %9.2f # power dissipated in ODT resistors during Writes in other ranks\n",
       channel, rank, psch_termWoth);
    printf
      ("Channel %d Rank %d Refresh(mW)             %9.2f # depends on frequency of Refresh (tREFI)\n",
       channel, rank, psch_ref);
    printf
      ("---------------------------------------------------------------\n");
    printf
      ("Channel %d Rank %d Total Rank Power(mW)    %9.2f # (Sum of above components)*(num chips in each Rank)\n",
       channel, rank, total_rank_power);
    printf
      ("---------------------------------------------------------------\n\n");

    /*

       printf("%3d %11d %16.2f %16.2f %17.2f %13.2f %13.2f %20.2f %21.2f %24.2f %12.2f %13.2f\n",\
       channel,\
       rank,\
       total_rank_power, \
       psch_act_pdn+psch_act_stby+psch_pre_pdn_slow+psch_pre_pdn_fast+psch_pre_stby, \
       psch_act, \
       psch_rd, \
       psch_wr, \
       psch_dq, \
       psch_termW, \
       psch_termRoth, \
       psch_termWoth, \
       psch_ref                                                                                  

       );
       */ 

    /*
       printf("Channel:%d Rank:%d Total background power : %f mW\n", channel, rank, psch_act_pdn+psch_act_stby+psch_pre_pdn_slow+psch_pre_pdn_fast+psch_pre_stby);
       printf("Channel:%d, Rank:%d Total activate power : %f,%f,%d,%f mW\n", channel, rank, psch_act,pds_act,T_RC,average_gap_between_activates[channel][rank]);
       printf("Channel:%d, Rank:%d Total I/O and termination power: %f rd:%f wr:%f dq:%f termW:%f termRoth:%f termWoth:%f mW\n", channel, rank, psch_rd+psch_wr+psch_dq+psch_termW+psch_termRoth+psch_termWoth, psch_rd, psch_wr, psch_dq, psch_termW, psch_termRoth, psch_termWoth);

       printf("Channel:%d, Rank:%d Total refresh power: %f mW\n", channel, rank, psch_ref);
       printf("Channel:%d, Rank:%d Total Rank power: %f mW\n\n", channel, rank, total_rank_power);
       printf("------------------------------------------------\n");
       */ 
  }
  else
  {
    printf
      ("PANIC: FN_CALL_ERROR: In calculate_power(), print_stats_type can only be 1 or 0\n");
    assert (-1);
  }
  return total_rank_power;
}


