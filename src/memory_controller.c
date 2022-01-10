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
#include <string.h>
#include <unistd.h> 
#include <time.h>

char bench[20];
char exp_name[20] = "";

long long int deadctr = 0;
long long int ddctr = 0;
long long int dead_dram = 0;
long long int deadrem = 0;
long long int surplus_dead = 0;
long long int surplus_in_use = 0;
long long int deadarr[100] = {0};
long long int dead_on_path = 0;
long long int dead_on_path_dram = 0;
long long int dead_on_path_arr[100] = {0};

long long int comptime_max;
long long int nvm_tmax = 0;
long long int dram_tmax = 0;
int longest_req;
int longest_dram;
int longest_nvm;
bool nvmt0_set;

long long int ring_G = 0;
long long int ring_round = 0;
long long int ep_round = 0;
long long int touchcount = 0;
long long int missl1wb = 0;
int shuff_dist[LEVEL] = {0};
int shuff_interval[LEVEL] = {0};
int shuf_dif = 0;
int shuf_prev = 0;
int ringacc = 0;
int wl_occ = 0;
bool pause_skip = false;
int count_level[LEVEL] = {0};
int cap_level[LEVEL] = {0};
int s_dist[RING_S+1] = {0};
int allocS_dist[RING_S+1] = {0};

int remote_nvms = 0;

int lifetime_min[LEVEL] = {[0 ... LEVEL-1] = 2147483647};
int lifetime_max[LEVEL] = {0};
long long int lifetime_sum[LEVEL] = {0};
int lifetime_count[LEVEL] = {0};


int count_min[LEVEL] = {[0 ... LEVEL-1] = 2147483647};
int count_max[LEVEL] = {0};
long long int count_sum[LEVEL] = {0};
int count_count[LEVEL] = {0};
int s_under[LEVEL] = {0};

int GREEN_BLOCK = 0;

unsigned long long int dram_leftover = 0;

unsigned long long int online_wait_dram = 0;
unsigned long long int online_wait_nvm = 0;
unsigned long long int evict_wait_dram = 0;
unsigned long long int evict_wait_nvm = 0;
unsigned long long int reshuffle_wait_dram = 0;
unsigned long long int reshuffle_wait_nvm = 0;
unsigned long long int meta_wait_dram = 0;
unsigned long long int meta_wait_nvm = 0;
unsigned long long int online_t0 = 0;
unsigned long long int cur_online = 0;
unsigned long long int evict_t0 =  0;
unsigned long long int cur_evict =  0;
unsigned long long int reshuffle_t0 = 0;
unsigned long long int cur_reshuffle = 0;
unsigned long long int meta_t0 = 0;
unsigned long long int cur_meta = 0;

int cur_dram_served_o = 0;
int cur_nvm_served_o = 0;
int cur_dram_served_e_r = 0;
int cur_dram_served_e_w = 0;
int cur_nvm_served_e_r = 0;
int cur_nvm_served_e_w = 0;
int cur_dram_served_m_r = 0;
int cur_dram_served_m_w = 0;
int cur_nvm_served_m_r = 0;
int cur_nvm_served_m_w = 0;
int cur_dram_served_r_r = 0;
int cur_dram_served_r_w = 0;
int cur_nvm_served_r_r = 0;
int cur_nvm_served_r_w = 0;

int dram_to_serve_o;
int nvm_to_serve_o;
int dram_to_serve_e_r;
int dram_to_serve_e_w;
int nvm_to_serve_e_r;
int nvm_to_serve_e_w;
int dram_to_serve_m_r;
int dram_to_serve_m_w;
int nvm_to_serve_m_r;
int nvm_to_serve_m_w;
int dram_to_serve_r_r;
int dram_to_serve_r_w;
int nvm_to_serve_r_r;
int nvm_to_serve_r_w;

int online_r = 0;
int evict_r = 0;
int reshuffle_r = 0;
int evict_w = 0;
int reshuffle_w = 0;
int w_ended = 0;
int meta_ended = 0;
int r_ended = 0;
int r_ended_o = 0;
int w_ended_dram = 0;
int w_ended_nvm = 0;

int reqcount = 0;
int determineReq = 0;
int detnvm = 0;
int detdram = 0;
long long int determineCycle = 0;

int onvm = 0;
int envm = 0;
int rnvm = 0;
int odram = 0;
int edram = 0;
int rdram = 0;
int mdram = 0;
int lrs_ctr = 0;

unsigned long long int s_underctr = 0;
unsigned long long int s_overctr = 0;
unsigned long long int s_inctr = 0;
unsigned long long int takenctr = 0;
unsigned long long int extendctr = 0;
unsigned long long int inplacectr = 0;

unsigned long long int deadctr_arr[LEVEL] = {0};
// unsigned int ddctr_arr[LEVEL] = {0};
unsigned long long int deadQ_ov[LEVEL] = {0};
unsigned long long int deadQ_empty_s6[LEVEL] = {0};
unsigned long long int deadQ_empty_s7[LEVEL] = {0};

unsigned long long int Q_serve_under[LEVEL] = {0};
unsigned long long int Q_serve_over[LEVEL] = {0};
unsigned long long int cap_Q_full[LEVEL] = {0};
unsigned long long int cap_Q_notfull[LEVEL] = {0};

unsigned int ep_s[LEVEL][RING_S] = {{0}};
unsigned int ep_shuf[MAX_SHUF+2] = {0};
unsigned int stash_hit = 0;
unsigned int dead_encountered[LEVEL] = {0};
unsigned int dead_gathered[LEVEL] = {0};
unsigned int dead_shadowed[LEVEL] = {0};
unsigned int dead_scan[LEVEL] = {0};
unsigned int shad_added[LEVEL] = {0};
unsigned int same_bucket = 0;
unsigned long long int wbuck = 0;





// long long int CYCLE_VAL = 0;

double exe_time = 0;
int gather_var = 5;

int STALE_TH = 0;
int GATHER_START = 0;
char *pargv[5];


long long int cache_clk = 0;

int hitctr = 0;		// # hits on llc
int missctr = 0;	// # misses on llc
int rmiss = 0;	// #  read misses on llc
int wmiss = 0;	// #  write misses on llc
long long int mem_clk = 0;

long long int orig_addr;
long long int orig_cycle; 
int orig_thread; 
int orig_instr; 
long long int orig_pc;
int oram_acc_addr;
int wbshuff = 0;

typedef struct Slot{
  bool isData;     // Data block 1  , PosMap block 0
  bool isReal;       // real 1 , dummy 0
  int addr;         // address of block e.g. A, B
  int label;      // path label e.g. 0 (=> L0)
  bool valid;     // added for ring oram
  DeadState dd;  // the dead block state (state of this physical block not what it contains)
  bool redirect; // a flag that says this slot is pointing to somewhere else
  int remote_index; // physical node of a block that supposed to be here but is somewhere else
  int remote_offset; // physical slot offset of a block that supposed to be here but is somewhere else
  int dead_start;
  int dead_lifetime;
}Slot;

// typedef struct Bucket{
//   Slot *slot;
// }Bucket;

typedef struct Bucket{
  Slot slot[Z];
  char count;  // added for ring oram
  char dumnum; // added for ring oram, # dummy slots available
  char dumval; // added for ring oram, # dummy slots available that are valid
  char dumdead; // added for ring oram, # dummy slots that are dead
  char allctr;
  char greenctr;
  char s;
  int reshuffled;
}Bucket;

typedef struct BucketShell{
  char count;  // added for ring oram
}BucketShell;

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


// typedef struct EntryMet{
//   int stale_addr[STALE_CAP];
//   int stale_path[STALE_CAP];

// }EntryMet;

typedef struct BucketMet{
  Slot slot[STALE_CAP];
}BucketMet;

Queue *oramQ;
Queue *plbQ;
Queue *pathQ;
Queue *deadQ;
Queue *deadQ_arr[LEVEL];
Queue *deadQ_shadow[LEVEL];

int revarr[RING_REV];

bool last_read_served;
bool last_read_deleted;
bool last_req_served;



Bucket GlobTree[NODE];      // global oram tree
int PosMap[BLOCK];          // position map
Slot Stash[STASH_SIZE];     // stash
// int PLB[PLB_SIZE] = {[0 ... PLB_SIZE-1] = -1};   // posmap lookaside buffer
EntryBuf PreBuffer[PREFETCH_BUF_SIZE]; // prefetch buffer
int SubMap[NODE];          // subtree address map

int intended = -1;         // index of intended block in stash
int intended_addr = -1;         // intended block in stash
bool pinFlag = false;     // a flag to indicate whether the intended block should be pinned to the stash or not 
// int trace[TRACE_SIZE] = {0};    // array for pre-reading traces from a file
int candidate[Z] = {[0 ... Z-1] = -1};    // keep index of candidates in stash for write back to a specific node
int dead_slot[Z] = {[0 ... Z-1] = -1};    // keep index of candidates in stash for write back to a specific node
int taken_slot[Z] = {[0 ... Z-1] = -1};    // keep index of candidates in stash for write back to a specific node
bool write_cache_hit = false;
int curr_trace = -1; 	// the current trace address 
int next_trace = 0; 	// the current trace address 
int curr_page = 0; 	// the current page address 
long long int curr_pc = 0; 	// the current pc address 
char curr_offset = 0; 	// the current offset
unsigned long long int curr_footprint = 0; 	// the current footprint of accessed block of the current page 
int stale_cand[STALE_CAP] = {[0 ... STALE_CAP-1] = -1};    // keep index of candidates in stale buffer for write back to a specific metadatanode

BucketMet Metadata[GL_COUNT][META_MAX_SIZE];      // meta data tree for ring oram 

Slot StaleBuffer[STALE_BUF_SIZE];

BucketShell SuperNode[4194304];
BucketShell SuperHoriz1[2097152];
BucketShell SuperHoriz2[4194304];


void pinOn() {pinFlag = true;}    // turn the pin flag on
void pinOff() {pinFlag = false;}  // turn the pin flag off


// rho
typedef struct RhoBucket{
  Slot slot[RHO_Z];
}RhoBucket;

RhoBucket RhoTree[RHO_NODE];      // rho tree
int TagArray[RHO_SET][RHO_WAY];         // rho postion map that store tags ~~~> parallel to tag array label
int TagArrayLabel[RHO_SET][RHO_WAY];     // rho postion map that store the corresponding label~~~>  parallel to tag array
long long int TagArrayLRU[RHO_SET][RHO_WAY];      // rho lru policy for eviction ~~~>  parallel to tag array
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
int dumval_dist[Z] = {0}; // distribution of number of available valid dummy
int dumval_range_dist[3][Z] =  { [0 ... 2] = { [0 ... Z-1] = 0 } }; // distribution of number of available valid dummy across different level range (top, mid, bot)

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
int lingered = 0;
int readctr = 0;
int writectr = 0;
int wskip = 0;
int ringdumctr = 0;
int stalectr = 0;
int stale_flush_ctr = 0;
int stale_discard_ctr = 0;
long long int nvm_norm_r = 0;
long long int dram_norm_r = 0;
long long int nvm_norm_w = 0;
long long int dram_norm_w = 0;
long long int nvm_remote_r = 0;
long long int dram_remote_r = 0;
long long int nvm_inplace_r = 0;
long long int dram_inplace_r = 0;
long long int dram_inplace_w = 0;
long long int dram_inplace_w_remembered = 0;
long long int dram_remote_w = 0;
long long int nvm_inplace_w = 0;
long long int nvm_remote_w = 0;
long long int dram_elselevel = 0;
long long int nvm_elselevel = 0;

long long int remote_under_r = 0;
long long int remote_over_r = 0;
long long int remote_under_w = 0;
long long int remote_over_w = 0;

long long int mem_req_start = 0;
long long int mem_req_latencies = 0;

unsigned long long int cl_interval = 0;

int stale_reduction = 0;

bool dirty_evict = false;

bool print_flag = false;

void printOn() {print_flag = true;}    // turn the pin flag on
void printOff() {print_flag = false;}  // turn the pin flag off

int TIMING_INTERVAL;


long long int plb_hit[H-1] = {0};   // # hits on a0, a1, a2, ...
long long int plbaccess[H-1] = {0};   // # total plb access (hits + misses)
int plb_evict[PLB_SIZE] = {[0 ... PLB_SIZE-1] = -1};  // array to keep address of blocks that are evicted from plb 
int plb_hist[PLB_SIZE] = {0};   // array to track the amount of reuse of blocks that are evicted from plb 
int plb_conflict[PLB_SIZE] = {0};   // array to track number of confilicts that happens on evictions
int plb_trace[PLB_SIZE] = {[0 ... PLB_SIZE-1] = -1}; 
int plb_interval[PLB_SIZE] =  {[0 ... PLB_SIZE-1] = -1}; 
int plb_temp[PLB_SIZE] =  {[0 ... PLB_SIZE-1] = -1}; 

long long int shuff[LEVEL] = {0};
long long int realcount[LEVEL] = {0};
long long int dumcount[LEVEL] = {0};
int supreal[4194304] = {0};
int supdum[4194304] = {0};
long long int shuff_total = 0;
int wb[LEVEL] = {0};
int ref_close[LEVEL] = {0};
int supshuf_total = 0;
int supshuf_horiz1 = 0;
int supshuf_horiz2 = 0;
int supep_horiz1 = 0;
int supep_horiz2 = 0;
int supcount_dist[3*RING_S] = {0};
int suphoriz1_dist[SUP_HORIZ_MAX+1] = {0};
int suphoriz2_dist[SUP_HORIZ_MAX+1] = {0};
int ephoriz1_dist[SUP_HORIZ_MAX+1] = {0};
int ephoriz2_dist[SUP_HORIZ_MAX+1] = {0};

// these are constants used for oram alg, by defualt initialized to oram params unless the tree is switched to rho
int LZ_VAR[LEVEL] = {[0 ... L1] = Z1, [L1+1 ... L2] = Z2, [L2+1 ... L3] = Z3, [L3+1 ... LEVEL-1] = Z4}; 
TreeType TREE_VAR;
int LEVEL_VAR;
int Z_VAR;
int S_INC_VAR;
int STASH_SIZE_VAR;
int OV_THRESHOLD_VAR;  
int BK_EVICTION_VAR;
int EMPTY_TOP_VAR;
int TOP_CACHE_VAR;
int PATH_VAR;
unsigned long long int NVM_ADDR_VAR;
unsigned long long int NVM_ADDR_BYTE;
AccessType ACCESS_VAR;      // to indicate whether a block shoulb be remapped and written back to the path or it should be evicted entirly
EnqueueType ENQUEUE_VAR;    // to indicate whether enqueue to oramq should be regularely added to the tail or head ~~~> head in case of dummy access 
PosType pos_var;
bool SIM_ENABLE_VAR;
bool CACHE_ENABLE_VAR;
bool DEAD_ENABLE_VAR;

// TreeType TREE_VAR = ORAM;
// int LEVEL_VAR = LEVEL;
// int Z_VAR = Z;
// int STASH_SIZE_VAR = STASH_SIZE;
// int OV_THRESHOLD_VAR = OV_THRESHOLD;  
// int BK_EVICTION_VAR = BK_EVICTION;
// int EMPTY_TOP_VAR = EMPTY_TOP;
// int TOP_CACHE_VAR = TOP_CACHE;
// int PATH_VAR = PATH;
// AccessType ACCESS_VAR = REGULAR;      // to indicate whether a block shoulb be remapped and written back to the path or it should be evicted entirly
// EnqueueType ENQUEUE_VAR = TAIL;    // to indicate whether enqueue to oramq should be regularely added to the tail or head ~~~> head in case of dummy access 
// PosType pos_var = POS2;
// bool SIM_ENABLE_VAR = SIM_ENABLE;

int set_start = 0;
int way_start = 0;

int dirty_pointctr = 0;

int oram_effective_pl = 0;

long long int nonmemops_sum = 0;
long long int nonmemops_executed = 0;

long long int lastpath = 0;


int  wl_pos[H-1] = {0};

int glctr[GL_COUNT] = {0};

long double util_sum[LEVEL] = {0}; 
long double util_avg[LEVEL] = {0}; 
long double util_overall[LEVEL] = {0}; 

bool ring_dummy = false;

// void reset_util(){
//   for (int i = 0; i < LEVEL; i++)
//   {
//     util_avg[i] = 0;
//   }
// }


void test_ring(){
  unsigned long long int addr = 0;
  printf("Testing Ring ORAM...\n");
  for (int i = 0; i < TRACE_SIZE; i++)
  {
    addr = rand() % BLOCK;
    // freecursive_access(addr, 'R');
    ring_access(addr);
  }
  exit(0);
}

void reset_shuff_interval(){
  for (int i = 0; i < LEVEL; i++)
  {
    shuff_interval[i] =0;
  }
}


void update_lifetime_stat(int lifetime, int level){
  // if (tracectr > WARMUP_TREE)
  // {
    if (lifetime != 0)
    {
      if (lifetime_min[level] > lifetime ) //&& lifetime != 1) 
      {
        lifetime_min[level] = lifetime;
      }
      if (lifetime_max[level] < lifetime)
      {
        lifetime_max[level] = lifetime;
      }
      lifetime_sum[level] += lifetime;
      lifetime_count[level]++; 
    }
  // }
  
}

void update_count_stat(int count, int level){
  // if (tracectr > WARMUP_TREE)
  // {
    if (count != 0)
    {
      if (count_min[level] > count ) //&& lifetime != 1) 
      {
        count_min[level] = count;
      }
      if (count_max[level] < count)
      {
        count_max[level] = count;
      }
      count_sum[level] += count;
      count_count[level]++; 
    }
  // }
  
}

int calc_space(int * lz){
  int sum = 0;
  for (int i = 0; i < LEVEL; i++)
  {
    sum += lz[i]*pow(2,i);
  }
  return sum; 
}


void print_lz(int* lz){
  int pl = 0;
	for (int i = TOP_CACHE; i < LEVEL; i++)
	{
		printf("%d ", lz[i]);
    if (i >= TOP_CACHE)
    {
		  pl += lz[i];
    }
    
	}
  printf(" ~> PL = %d\n", pl);
}

int find_start(int* bitmap){
  for (int i = LEVEL-1; i >= 0; i--)
  {
    if (bitmap[i] == 1)
    {
      return i;
    }
  }
  return LEVEL-1;
}

void explore_lz(){
  int total = Z*(pow(2,LEVEL)-1);

  for (int k = 1; k < 4; k++)
  {
    int start = LEVEL-1;
    bool has_new_start = true;
    bool reverse_mode = false;
    while (has_new_start)
    {
      // printf("start: %d\n", start);
      int lz[LEVEL] = {[0 ... LEVEL-1] = Z};
      int bitmap[LEVEL] = {0};
      bool chained = true;
      bool ongoing = false;  // whether the reduction has been ongoing
      
      for (int i = start; i >= TOP_CACHE ; i--)
      {
        if (ongoing && !chained)
        {
          break;
        }
        
        lz[i] -= k;
        if (total - calc_space(lz) >= 0.01*total)
        {
          lz[i] += k ;
          if(ongoing)
          {
            chained = false;
            start = find_start(bitmap)-1;
          }
        }
        else
        {
          bitmap[i] = 1;
          ongoing = true;
        }
        if (i == TOP_CACHE && chained)
        {
          // has_new_start = false;
          reverse_mode = true;
        }
      }
      // print_lz(lz);
      if (reverse_mode)
      {
        for (int j = TOP_CACHE; j < LEVEL; j++)
        {
          lz[j] --;
          if (total - calc_space(lz) >= 0.01*total || lz[j] == 0)
          {
            lz[j]++;
            has_new_start = false;
            print_lz(lz);
            break;
          }
        }
      }
      else
      {
        print_lz(lz);
      }
    }
  }
    
  
  exit(0);
}


void print_array(int * arr, int size, FILE *fp){
  for (int i = 0; i < size; i++)
  {
    fprintf(fp, "arr[%d],%d\n", i, arr[i]);
  }
  // printf("\n");
}

void print_super_node(int * arr, char * exp, char * bench, char * realdum){
  FILE *fp;
  char supfile[100];
  sprintf(supfile, "supnode-%s-%s-%s.csv", exp, bench, realdum);
  fp = fopen(supfile,"w+");

  for (int i = 0; i < 4194304; i++)
  {
    fprintf(fp, "%d\n", arr[i]);
  }

  fclose(fp);
}

void print_array_double(long double * arr, int size, FILE *fp){
  for (int i = 0; i < size; i++)
  {
    fprintf(fp, "arr[%d],%Lf\n", i, arr[i]);
  }
}

bool is_nvm_addr(int addr){
  if (!NVM_ENABLE)
  {
    return false;
  }

  if (addr >= NVM_ADDR_VAR && addr < SLOT)
  {
    return true;
  }
  return false;
}


bool is_nvm_addr_byte(long long int physical_address){
    if (!NVM_ENABLE)
    {
      return false;
    }
    
    unsigned long long int data_addr_byte = (unsigned long long int) DATA_ADDR_SPACE << (int)log2(BLOCK_SIZE);
    unsigned long long int metadata_nvm_byte = DATA_ADDR_SPACE + (pow(2, NVM_START)-1)*1;
    metadata_nvm_byte =  (unsigned long long int) metadata_nvm_byte << (int)log2(BLOCK_SIZE);
    if ((physical_address >= NVM_ADDR_BYTE && physical_address <=  data_addr_byte) )// || physical_address >= metadata_nvm_byte)
    {
      return true;
    }
    return false;
}

void var_init(){
  TREE_VAR = ORAM;
  LEVEL_VAR = LEVEL;
  Z_VAR = Z;
  STASH_SIZE_VAR = STASH_SIZE;
  OV_THRESHOLD_VAR = OV_THRESHOLD;  
  // OV_THRESHOLD_VAR = STASH_SIZE - (oram_effective_pl + Z);  
  BK_EVICTION_VAR = BK_EVICTION;
  EMPTY_TOP_VAR = EMPTY_TOP;
  TOP_CACHE_VAR = TOP_CACHE;
  PATH_VAR = PATH;
  ACCESS_VAR = REGULAR;      // to indicate whether a block should be remapped and written back to the path or it should be evicted entirly
  ENQUEUE_VAR = TAIL;    // to indicate whether enqueue to oramq should be regularely added to the tail or head ~~~> head in case of dummy access 
  pos_var = POS2;
  SIM_ENABLE_VAR = SIM_ENABLE;
  NVM_ADDR_VAR = (pow(2, NVM_START)-1)*Z_VAR;
  NVM_ADDR_BYTE = NVM_ADDR_VAR << (int)log2(BLOCK_SIZE);
  DEAD_ENABLE_VAR = DEAD_ENABLE;
  S_INC_VAR = DYNAMIC_S ? S_INC : 0;
}

unsigned long long int byte_addr(long long int physical_addr){
  // unsigned long long int mask = pow(2,)
  
  unsigned long long int addr = (unsigned long long  int)(physical_addr & (0x7fffffff));  // 4/15/2021 for l=25, for l=24 go back to 0x7fffffff
  // unsigned long long int addr = (unsigned long long  int)(physical_addr & (0xffffffff));  // 4/15/2021 for l=25, for l=24 go back to 0x7fffffff
  // unsigned long long int addr = physical_addr;  
  unsigned long long int max = ((unsigned long long int)(BLOCK-1)<<((unsigned long long int)log2(BLOCK_SIZE))) | (unsigned long long int)(BLOCK_SIZE-1);
  // printf("addr: %lld\n", addr);
  // printf("max: %lld\n", max);
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

unsigned int block_addr(unsigned long long int caddr){
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
    if (limit < 0) {
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


bool  Enqueue(Queue *pQueue, Element *item) {
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


bool remove_dead(Queue *pQueue, int key1, int key2){
  Element *it = pQueue->head;
  Element *forehand = pQueue->head;

  
  
  for (int i = 0; i < pQueue->size; i++)
  {
    if (it->index == key1 && it->offset == key2)
    {
      pQueue->size--;
      if (pQueue->head->index == key1 && pQueue->head->offset == key2)
      {
        pQueue->head = pQueue->head->prev;

      }
      else if (pQueue->tail->index == key1 && pQueue->tail->offset == key2)
      {
        pQueue->tail = forehand;
      }
      else
      {
        forehand->prev = it->prev;
      }
      
      return true;
    }
    forehand = it;
    it = it->prev;
  }
  return false;
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

void insert_oramQ(long long int addr, long long int cycle, int thread, int instr, long long int pc, char type, bool last_read, bool nvm_access, char op_type, bool beginning, bool ending, int level) {
  addr = addr << (int)log2(BLOCK_SIZE);
  reqcount++;
  Element *pN = (Element*) malloc(sizeof (Element));
  pN->addr = addr;
  pN->cycle = cycle; 
  pN->thread = thread; 
  pN->instr = instr; 
  pN->pc = pc; 
  pN->type = type;
  pN->oramid = (RING_ENABLE)? ringctr :(TREE_VAR == RHO) ? rhoctr : oramctr;
  // pN->oramid = (RING_ENABLE && op_type == 'r')? shuff_total : pN->oramid;
  pN->oramid = (ENQUEUE_VAR == HEAD) ? -1 : pN->oramid;   // in case of dummy access set oram id to -1
  pN->tree = TREE_VAR;
  pN->orig_addr = orig_addr;
  pN->last_read = (!RING_ENABLE) ? false : last_read;
  pN->nvm_access = nvm_access;
  pN->op_type = op_type;
  pN->beginning = (ENQUEUE_VAR == HEAD) ? false : beginning;
  pN->ending = (ENQUEUE_VAR == HEAD) ? false : ending;
  pN->reqid = reqcount;
  pN->level = level;
  
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

void switch_cache_enable_to(bool tf){
  CACHE_ENABLE_VAR = tf;
}

void switch_dead_enable_to(bool tf){
  DEAD_ENABLE_VAR = tf;
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


int  calc_tri(int label,  int l){
  int sum = 0;
  for(int i = 0; i < l; i++)
    sum += pow(2,i);

  return sum;
}

int  calc_super(int label,  int l){

  int index = -1;

  // int a = pow(2,LEVEL-1)/pow(2,l);
  index = floor(label/pow(2,(LEVEL_VAR-l-1)));

  return index;
}

int calc_horiz(int index, int label, int i){
  int hind = (index - calc_tri(label, i))/2;
  // int hind = (index - calc_tri(label, i));
  // hind = (hind >= pow(2, i-1)) ? hind - pow(2, i-1) : hind;
  return hind;
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

void set_to_serves(){

  if (RING_ENABLE)
  {
    dram_to_serve_o = NVM_START - TOP_CACHE;
    nvm_to_serve_o = LEVEL - NVM_START;
    dram_to_serve_e_r = (NVM_START - TOP_CACHE)*(RING_Z); 
    // dram_to_serve_e_w = (NVM_START - TOP_CACHE)*(Z); 
    nvm_to_serve_e_r = (LEVEL - NVM_START)*(RING_Z);
    // nvm_to_serve_e_w = (LEVEL - NVM_START)*(Z);
    dram_to_serve_r_r = 1*(RING_Z);
    dram_to_serve_r_w = 1*(Z);
    nvm_to_serve_r_r = 1*(RING_Z);
    nvm_to_serve_r_w = 1*(Z);
    dram_to_serve_m_r = (LEVEL - TOP_CACHE)*(1); 
    dram_to_serve_m_w = (LEVEL - TOP_CACHE)*(1); 

    for (int i = TOP_CACHE_VAR; i < LEVEL; i++)
    {
      if (in_nvm(i))
      {
        nvm_to_serve_e_w += LZ[i];
      }
      else
      {
        dram_to_serve_e_w += LZ[i];
      }
    }
    
    if (DYNAMIC_S && DEAD_ENABLE)
    {
      dram_to_serve_e_w = (LEVEL - TOP_CACHE)*(RING_Z+1);
      dram_to_serve_r_w = 1*(RING_Z+1);
    }
    
    // nvm_to_serve_m_r = (LEVEL - NVM_START)*(1);
    // nvm_to_serve_m_w = (LEVEL - NVM_START)*(1);

    
    // cur_dram_served_e_w = dram_to_serve_e_w;
    // cur_nvm_served_e_w = nvm_to_serve_e_w;
    // cur_dram_served_m_w = dram_to_serve_m_w;
    // // cur_nvm_served_m_w = 0;
    // cur_dram_served_r_w = dram_to_serve_r_w;
    // cur_nvm_served_r_w = nvm_to_serve_r_w;
  }
  else
  {
    for (int i = TOP_CACHE_VAR; i < LEVEL; i++)
    {
      dram_to_serve_e_r += LZ[i]; 
      dram_to_serve_e_w += LZ[i];
    }

    nvm_to_serve_e_r = 0 ; 
    nvm_to_serve_e_w = 0;
  }
  



}

void oram_alloc(){
  STALE_TH = STALE_BUF_SIZE-1;
  GATHER_START = TOP_CACHE + DEAD_GATHER_OFFSET;
  GREEN_BLOCK = CB_ENABLE ? CB_GREEN_MAX : 0;

  if (!NVM_ENABLE)
  {
    NVM_START = LEVEL;
  }
  
  set_to_serves();


  for (int i = 0; i < LEVEL; i++)
	{
		path_length += LZ[i];
	}

  for (int i = CAP_LEVEL; i < LEVEL; i++)
  {
    sub_cap += LZ[i]*pow(2,i-(CAP_LEVEL));
    
  }

  for (int i = 0; i < 4194304; i++)
  {
    SuperNode[i].count = 0;
    SuperHoriz2[i].count = 0;
  }
  for (int i = 0; i < 2097152; i++)
  {
    SuperHoriz1[i].count = 0;
  }
  
  
  for (int i = 0 ; i < NODE; i++) 
  {
    int l = calc_level(i);  
     GlobTree[i].count = 0;
     GlobTree[i].dumdead = 0;
     GlobTree[i].allctr = 0;
     GlobTree[i].greenctr = 0;
     GlobTree[i].s = LS[l];
     GlobTree[i].reshuffled = 0;
    for (int k = 0; k < Z; ++k)
    {
      GlobTree[i].slot[k].addr = -1;
      GlobTree[i].slot[k].label = -1;
      GlobTree[i].slot[k].isReal = false;
      GlobTree[i].slot[k].isData = false;
      GlobTree[i].slot[k].valid = true;  // ??? to be revised
      GlobTree[i].slot[k].dd = REFRESHED;  
      GlobTree[i].slot[k].redirect = false;  
      GlobTree[i].slot[k].dead_start = 0;  
      GlobTree[i].slot[k].dead_lifetime = 0; 
      if (k < LZ[l])
      {
        GlobTree[i].dumnum++;
        GlobTree[i].dumval++;
      }
      if (k >= LZ[l] && !in_nvm(l) && l >= GATHER_START)
      {
        GlobTree[i].slot[k].valid = false;
        GlobTree[i].slot[k].dd = DEAD;  
        surplus_dead++;
      }
      
      
    }
  }
  for (int h = 0; h < GL_COUNT; h++)
  {
    for (int i = 0; i < META_MAX_SIZE; i++)
    {
      for (int k = 0; k < STALE_CAP; k++)
      {
        Metadata[h][i].slot[k].addr = -1;
        Metadata[h][i].slot[k].label = -1;
        Metadata[h][i].slot[k].isReal = false;
        Metadata[h][i].slot[k].isData = false;
      }
    }
    STALE_TH -= GL_CAP[h];
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
  deadQ = ConstructQueue(DEADQ_SIZE);

  for (int i = 0; i < RING_REV; i++)
  {
    revarr[i] = reverse_lex(i);
  }

  for (int i = LEVEL-1; i >= GATHER_START; i--)
  {
    // int constant = (i < 16) ? 1.7 : 1.5;
    double constant = 1.5;
    int qs = (int)floor(pow(constant, i));
    qs = DEADQ_SIZE;
    deadQ_arr[i] = ConstructQueue(qs);
    deadQ_shadow[i] = ConstructQueue(SHADQ_SIZE);
    // printf(" deadQ[%d]  ~> size: %d\n", i, qs);
  }

  for (int i = 0; i < LEVEL; i++)
  {
    cap_level[i] = (LZ[i]*pow(2,i));
  }
  

}

void set_reshuffle_w(int level){
  if (in_nvm(level))
  {
    nvm_to_serve_r_w = LZ[level];
  }
  else
  {
    dram_to_serve_r_w = LZ[level];
  }
}

int calc_deadQ_size(){
  int sum = 0;
  int end = DYNAMIC_S ? LEVEL : NVM_START;
  for (int i = GATHER_START; i < end; i++)
  {
    sum += deadQ_arr[i]->size;
  }
  return sum;
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

void record_util_snap()
{
  for (int i = 0; i < LEVEL; i++)
  {
    util_avg[i] = (double)count_level[i]/cap_level[i];
  }
}

void reset_util_level(){
  for (int i = 0; i < LEVEL; i++)
  {
    util_avg[i] = 0;
    util_sum[i] = 0;
  }
  cl_interval = 0;
}

void record_util_level(){
  cl_interval++;
  for (int i = 0; i < LEVEL; i++)
  {
    // util_avg[i] = util_sum[i]/cl_interval;
    util_overall[i] = util_sum[i]/cl_interval;
    util_sum[i] += (double)count_level[i]/cap_level[i];
  }
}

void print_count_level(int rounds, PrintType print){
  if (print != NONE)
  {
    printf("\n");
  }
  
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
    

    if (print == SNAP)
    {
      printf("%f\n", counter/(LZ[l]*pow(2,l)));
    }
    else if (print == TMP)
    {
      printf("%Lf\n", util_avg[l]/rounds);
    }
    else if (print == ALL)
    {
      util_sum[l] += (counter/(LZ[l]*pow(2,l)));
      printf("%Lf\n", util_sum[l]/rounds);
    }
    else if (print == NONE)
    {
      util_sum[l] += (counter/(LZ[l]*pow(2,l)));
      util_avg[l] += (counter/(LZ[l]*pow(2,l)));
    }
      
  }
  
}

// read trace file in fill in the trace array 
// void init_trace(){
//   for (int i = 0; i < TRACE_SIZE; i++)
//   {
//     FILE *fp;
//     char c[100];
//     fp = fopen("trace.txt","r");
//     fscanf(fp,"%[^\n]", c);

//     int i = 0;
//     while (fgets(c, 100, fp) != NULL)
//     {
//       // printf("%s", c);
//       int addr;
//       sscanf(c, "%d", &addr);
//       if (addr > pow(2,24) -1)
//       {
//         c[0] = '0';
//         c[1] = '0';
//         c[2] = '0';
//         sscanf(c, "%d", &addr);
//       }
//       trace[i] = addr;
//       i++;
//       if (i == TRACE_SIZE)
//       {
//         break;
//       }
      
//     }
//     fclose(fp);
    
//   }
// }


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


void retrieve_stale(int label){
  for (int h = GL_COUNT-1; h >= 0; h--)
  {
    int gv = GL[h];
    int index = calc_index(label, gv);
    index = gl_index(index, h);

    stale_access(index, h, 'R');

    for (int j = 0; j < GL_CAP[h]; j++)
    {
      // int target = stash_label>>(LEVEL_VAR-1-i);
      // bool on_path = (Metadata[index].slot[j].label == label) || 
      // bool can_clear = Metadata[index].slot[j].isReal && on_path
      if (Metadata[h][index].slot[j].isReal)
      {
        if(add_stale_buf(Metadata[h][index].slot[j]) != -1)
        {
          Metadata[h][index].slot[j].isReal = false;
          Metadata[h][index].slot[j].addr = -1;
          Metadata[h][index].slot[j].label = -1;
          glctr[h]--;
        }
        else
        {
          printf("ERROR: discard stale: trace %d buffer overflow!  @ %d\n", tracectr, stalectr);
          print_oram_stats();
          exit(1);
        }
      }
    }
  }
}


void discard_stale(int label){


  for (int i = LEVEL-1; i >= 0; i--)
  {
    int index = calc_index(label, i);
    // int mask = label>>(LEVEL_VAR-1-i);
    // int target = stale_label>>(LEVEL_VAR-1-i);
    for (int j = 0; j < LZ[i]; j++)
    {
      if (GlobTree[index].slot[j].isReal)
      {
        for (int k = 0; k < STALE_BUF_SIZE; k++)
        {
          if (StaleBuffer[k].isReal && StaleBuffer[index].addr == GlobTree[index].slot[j].addr  && StaleBuffer[index].label == GlobTree[index].slot[j].label)
          {
            stale_discard_ctr++;
            remove_stale_buf(get_stale_buf(StaleBuffer[index].addr));  // remove stale info from stale buffer
            GlobTree[index].slot[j].isReal = false;                   // remove stale block from the oram tree
            break;
          }
        }
      }
    }
  }
  

}



void read_path(int label){

    // printf("\nread path @ trace %d\n", tracectr);
    int gi = -1;
    bool last_read = false;
    

    if (META_ENABLE)
    {
      retrieve_stale(label);
      discard_stale(label);
    }
    
    // int start = RING_ENABLE ? EMPTY_TOP_VAR : LEVEL_VAR - 1;
    // int end = RING_ENABLE ?

    for(int i = LEVEL_VAR-1; i >= EMPTY_TOP_VAR; i--)
    // for(int i = EMPTY_TOP_VAR; i < LEVEL_VAR; i++)
    {
      // printf("\nread path %d level %d\n", label, i);
      // print_path(0);
      if (!SKIP_ENABLE || i <= SKIP_L1 || i >= SKIP_L2)
      {
        int index = calc_index(label, i);
        if (RING_ENABLE)
        {
          read_bucket(index, i, 'e');
          continue;
        }
        
        // if (RING_ENABLE)
        // {
          int reqmade = 0;
          int dum_cand[Z] = {0};
          int cand_ind = 0;
        // }

        int slotCount = DYNAMIC_S ? Z : LZ_VAR[i];  

        if (slotCount < 0 || slotCount > Z)
        {
          printf("ERROR: read evict path slot count %d out of range!\n", slotCount);
          exit(1);
        }
        
        for(int j = 0; j < slotCount; j++)
        {
          // printf("j: %d \n", j);
          gi++;
          if (i >= TOP_CACHE_VAR)
          {
            int mem_addr;
            

            if(!RING_ENABLE || GlobTree[index].slot[j].isReal)
            {
              mem_addr = (!SUBTREE_ENABLE) ? (index*Z_VAR+j): (TREE_VAR == ORAM)? SubMap[index]+j : RhoSubMap[index]+j;
              if (TREE_VAR == ORAM && STL_ENABLE && SUBTREE_ENABLE && NUM_CHANNELS_SUBTREE >  1)
              {
                int gi_prime = gi + (LEVEL-TOP_CACHE)*Z - oram_effective_pl;
                int i_prime = floor(gi_prime/Z) + 1 + L1;
                int j_prime = gi_prime % Z;
                int index_prime = calc_index(label, i_prime);
                mem_addr = SubMap[index_prime] + j_prime;
              }
              if (RING_ENABLE)
              {
                mem_addr = calc_mem_addr(index, j, 'R');
                GlobTree[index].slot[j].valid = false;
              }
              
              
              // bool nvm_access = is_nvm_addr(mem_addr);
              bool nvm_access = in_nvm(i);

              reqmade++;
              if (RING_ENABLE)
              {
                if (i == TOP_CACHE_VAR && reqmade == RING_Z)
                // if (i == LEVEL_VAR-1 && reqmade == RING_Z)
                {
                  last_read = true;
                  // printf("reqmade is true\n");
                }
              }
              else
              {
                last_read = (i == TOP_CACHE_VAR && j == LZ_VAR[i] - 1);
              }
              
              


              if (SIM_ENABLE_VAR)
              {
                // printf("ep mem addr: %d   @ L%d  j: %d \n", mem_addr, i, j);

                // bool beginning = (i ==  LEVEL_VAR-1 && reqmade == 1);
                // bool beginning = RING_ENABLE ? (i ==  TOP_CACHE_VAR && reqmade == 1) : (i ==  LEVEL_VAR-1 && reqmade == 1);
                bool beginning = (i ==  LEVEL_VAR-1 && reqmade == 1);
                bool ending = false;
                char op_type = 'e';
                insert_oramQ(mem_addr, orig_cycle, orig_thread, orig_instr, orig_pc, 'R', last_read, nvm_access, op_type, beginning, ending, i);
              }
              
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
                if (i <= TOP_BOUNDARY)
                {
                  topctr++;
                }
                else if (i <= MID_BOUNDARY)
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
                count_level[i]--;
                // GlobTree[index].dumval++;
              }
              else
              {
                printf("ERROR: read: trace %d stash overflow!  @ %d\n", tracectr, stashctr);
                printf("stash leftover %d\n", stash_leftover);
                printf("stash removed  %d\n", stash_removed);
                printf("fill hit  %d\n", fillhit);
                printf("fill miss  %d\n", fillmiss);
                export_csv(pargv);
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
        if (RING_ENABLE && i >= TOP_CACHE_VAR)
        {
          int reqcont = reqmade;
          for (int k = 0; k < RING_Z-reqmade-GREEN_BLOCK; k++)
          {
            int ri = -1;
            int sd = -1;
            while (sd == -1)
            {
              // printf("@%d in ep\n", tracectr);

              ri = rand() % cand_ind;
              sd = dum_cand[ri];
            }

            reqcont++;

            if (i == TOP_CACHE_VAR && reqcont == RING_Z)
            // if (i == LEVEL_VAR-1 && reqcont == RING_Z)
            {
              // printf("reqcont is true\n");
              last_read = true;
            }
            
            int mem_addr = calc_mem_addr(index, sd, 'R');
            GlobTree[index].slot[sd].valid = false;

            // bool nvm_access = is_nvm_addr(mem_addr);
            bool nvm_access = in_nvm(i);
            if (SIM_ENABLE_VAR)
            {
                // printf("ep mem addr: %d   @ L%d  j: %d \n", mem_addr, i, sd);

              // bool beginning = (i ==  TOP_CACHE_VAR && reqcont == 1);
              bool beginning = (i ==  LEVEL_VAR-1 && reqcont == 1);
              bool ending = false;
              char op_type = 'e';

              insert_oramQ(mem_addr, orig_cycle, orig_thread, orig_instr, orig_pc, 'R', last_read, nvm_access, op_type, beginning, ending, i);
              /* code */
            }
            
            // printf("%d: slot %d accessed ~> dummy? %s\n", k, sd, GlobTree[index].slot[sd].isReal?"no":"yes");
            dum_cand[ri] = -1;
          }
        }
        for (int j = 0; j < slotCount; j++)
        {
          if (RING_ENABLE && GlobTree[index].slot[j].valid)
          {
            calc_mem_addr(index, j, 'R');
            GlobTree[index].slot[j].valid = false;

          }
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

void print_candidate(){
  printf("candidates: ");
  for (int i = 0; i < Z; i++)
  {
    printf("%d    ", candidate[i]);
  }
  printf("\n");
  
}


int calc_gl(int level){
  for (int i = GL_COUNT-1; i >= 0; i--)
  {
    if (level >= GL[i])
    {
      return GL[i];
    }
  }
  return -1;
}

int calc_ring_s(int index, int level){
  if (DYNAMIC_S)
  {
    return GlobTree[index].s;
  }
  else
  {
    return LS[level];
  }
  
}

int gl_index(int index, int h){
  return index - (pow(2,GL[h]) - 1);
}

int calc_stale_addr(int index, int h){

  int i = 0;
  int level = GL[i];
  int sum = 0;
  while (level < GL[h])
  {
    sum += pow(2, GL[i])*(GL_CAP[i]/(int)log2(CACHE_LINE_SIZE));
    level = GL[i++];
  }
  int addr = sum + index*(GL_CAP[h]/(int)log2(CACHE_LINE_SIZE));

  return addr;
}


void stale_access(int index, int h, char type){
    int cl_count =  GL_CAP[h]/(int)log2(CACHE_LINE_SIZE);
    int node_addr = DATA_ADDR_SPACE + META_ADDR_SPACE + calc_stale_addr(index, h);
    int last_read = false;
    for (int k = 0; k < cl_count; k++)
    {
      if (GL[h] >= TOP_CACHE_VAR && SIM_ENABLE_VAR)
      {
        if (type == 'R' && GL[h] == TOP_CACHE_VAR && k == cl_count -1)  // $ GL[h] == TOP_CACHE_VAR should chabge if GL[1] is not TOP_CACHE (i.e. 10)
        {
          last_read = true;
        }
        int mem_addr = node_addr + k;
        bool beginning = false;
        bool ending = false;
        char op_type = 's';
        insert_oramQ(mem_addr, orig_cycle, orig_thread, orig_instr, orig_pc, type, last_read, false, op_type, beginning, ending, GL[h]);
      }
    }
}

void flush_stale(int label){
  for (int h = GL_COUNT-1; h >= 0; h--)
  {
    int gv = GL[h];
    for (int i = 0; i < STALE_CAP; i++)
    {
      stale_cand[i] = -1;
    }
    
    int c = 0;
    int mask = label>>(LEVEL_VAR-1-gv);
    for(int k = 0; k < STALE_BUF_SIZE; k++)
    {
      bool spot_real =  StaleBuffer[k].isReal;
      if (spot_real)
      {
        int stale_label = StaleBuffer[k].label;
        int target = stale_label>>(LEVEL_VAR-1-gv);
        if((((target)^mask) == 0))
        {
          stale_cand[c] = k;
          c++; 
          if (c == GL_CAP[h])
          {
            break;
          }
        }
      }
    }

    int index = calc_index(label, gv);
    index = gl_index(index, h);

    stale_access(index, h, 'W');
    
    for (int j = 0; j < GL_CAP[h]; j++)
    {

      if (stale_cand[j] != -1)
      {
        Metadata[h][index].slot[j].addr = StaleBuffer[stale_cand[j]].addr;
        Metadata[h][index].slot[j].label = StaleBuffer[stale_cand[j]].label;
        Metadata[h][index].slot[j].isReal = true;
        Metadata[h][index].slot[j].isData = false;
        glctr[h]++;
        remove_stale_buf(stale_cand[j]);
        stale_flush_ctr++;
      }
    }

    
  }
  

  

}

void write_path(int label){

    // printf("\nwrite path @ trace %d\n", tracectr);
  
  int gi = -1;

  if (META_ENABLE)
  {
    flush_stale(label);
  }
  
  

  for(int i = LEVEL_VAR-1; i >= EMPTY_TOP_VAR; i--)
  {
    
    
    if (!SKIP_ENABLE || i <= SKIP_L1 || i >= SKIP_L2)
    {
      int index = calc_index(label, i);
      if (RING_ENABLE)
      {
        ep_s[i][(int)GlobTree[index].count]++;
        int shuf = GlobTree[index].reshuffled;
        if (shuf > MAX_SHUF)
        {
          shuf = MAX_SHUF+1;
        }
        
        if (i == LEVEL-1)
        {
          ep_shuf[shuf]++;
        }
        
        GlobTree[index].reshuffled = 0;

        write_bucket(index, label, i, 'e');
        if (SUPER_ENABLE && is_super_level(i))
        {
          write_bucket(calc_super_in_tree(index), label, i, 'e');
        }
        continue;
      }
      int addr = 0;
      if (GlobTree[index].count >= LS[i]-1)
      {
        ref_close[i]++;
      }
      update_count_stat(GlobTree[index].count, i);
      GlobTree[index].count = 0; // for ring oram evict path
      // GlobTree[index].s = LS[i];

      if (i >= LEVEL-2)
      {
        int sind = (i == LEVEL-2) ? calc_super(label, i) : calc_super(label, i-1);
        SuperNode[sind].count = 0;
        int hind = calc_horiz(index, label, i);
        

        if (i == LEVEL-2)
        {
          supep_horiz1 += SuperHoriz1[hind].count;
          ephoriz1_dist[(int)SuperHoriz1[hind].count]++;
          SuperHoriz1[hind].count = 0;
        }
        else
        {
          supep_horiz2 += SuperHoriz2[hind].count;
          ephoriz2_dist[(int)SuperHoriz2[hind].count]++;
          SuperHoriz2[hind].count = 0;
        }
      }
      deadctr -= GlobTree[index].dumdead;
      deadctr_arr[i] -= GlobTree[index].dumdead;
      if (i < NVM_START && i >= TOP_CACHE_VAR)
      {
        dead_dram -= GlobTree[index].dumdead;
      }
      
      GlobTree[index].dumdead = 0;
      GlobTree[index].dumval = Z;

      reset_candidate();
      pick_candidate(index, label, i);
      int stt_cand = -1;


      for(int j = 0; j < LZ_VAR[i]; j++)
      {
        GlobTree[index].slot[j].valid = true;  // added for ring oram
        if (GlobTree[index].slot[j].dead_start != 0 && tracectr > DD_SATURATE)
        {
          int lifetime = ringctr - GlobTree[index].slot[j].dead_start;
          GlobTree[index].slot[j].dead_start = 0;
          update_lifetime_stat(lifetime, i);
        }
        
        

        gi++;
        
        int mem_addr = calc_mem_addr(index, j, 'W');

        if (i >= TOP_CACHE_VAR  && SIM_ENABLE_VAR)
        {
          addr = (!SUBTREE_ENABLE) ? (index*Z_VAR+j): (TREE_VAR == ORAM)? SubMap[index]+j : RhoSubMap[index]+j;
          addr = (RING_ENABLE && DEAD_ENABLE_VAR) ? mem_addr : addr;
          if (TREE_VAR == ORAM && STL_ENABLE && SUBTREE_ENABLE && NUM_CHANNELS_SUBTREE >  1)
          {
            int gi_prime = gi + (LEVEL-TOP_CACHE)*Z - oram_effective_pl;
            int i_prime = floor(gi_prime/Z) + 1 + L1;
            int j_prime = gi_prime % Z;
            int index_prime = calc_index(label, i_prime);
            addr = SubMap[index_prime] + j_prime;
          }
          // bool nvm_access = is_nvm_addr(mem_addr);
          bool nvm_access = in_nvm(i);
          bool beginning = false;
          bool ending = (i == TOP_CACHE_VAR && j == LZ_VAR[i]-1);
          bool last_read = (i == TOP_CACHE_VAR && j == LZ_VAR[i]-1);
          // last_read = false;
          char op_type = 'e';
          insert_oramQ (addr, orig_cycle, orig_thread, orig_instr, 0, 'W', last_read, nvm_access, op_type, beginning, ending, i);
        }



          if (STT_ENABLE && TREE_VAR == ORAM)
          {
            stt_cand = stt_candidate(label, i);
            if (stt_cand != -1 && (stt_cand != intended_addr || !pinFlag))  
            {
              if (!RING_ENABLE)
              {
                sttctr++;
                GlobTree[index].slot[j].addr = stt_cand;
                GlobTree[index].slot[j].label = PosMap[stt_cand];
                GlobTree[index].slot[j].isReal = true;
                GlobTree[index].slot[j].isData = true;
                stt_invalidate(stt_cand);

                GlobTree[index].dumnum--;
                GlobTree[index].dumval--;
                count_level[i]++;

                continue;
              }
            }
          }
          

          if (candidate[j] != -1) 
          {
            if (!RING_ENABLE)
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
                count_level[i]++;
              }

              remove_from_stash(candidate[j]);
            }

            

          }

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
    if (!LLC_DIRTY || pinFlag)
    {
      PosMap[addr] = label;   // $$$ remember to exclude current path later on
    }
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
      if (!LLC_DIRTY || pinFlag)
      {
        Stash[index].label = label;
      }
      else
      {
        remove_from_stash(index);
      }
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



  if (!META_ENABLE)
  {
    if (RING_ENABLE && WSKIP_ENABLE)
    {
      if (s.label != PosMap[s.addr])
      {
        linger_discard++;
        wl_occ--;
        return STASH_SIZE_ORG;
      }
    }
  }
  
  
  

  if (s.addr == 0)
  {
    // printf("exiting due to 0 added to stash!\n");
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




int add_stale_buf(Slot s){

  // if (s.addr == 0)
  // {
  //   printf("exiting due to 0 added to stash!\n");
  //   // exit(1);
  // }
  
  for(int i = 0; i < STALE_BUF_SIZE; i++ )
  {
    if(!StaleBuffer[i].isReal)
    {

      StaleBuffer[i].addr = s.addr;
      StaleBuffer[i].label = s.label;
      StaleBuffer[i].isReal = true;
      StaleBuffer[i].isData = true;
      
      stalectr++;

      if (!s.isReal)
      {
        printf("ERROR: add to stash: dummy added @ trace %d\n", tracectr);
        print_stash();
        exit(1);
      }
      return i;
    }
    
    
  }
  return -1;
}


void remove_stale_buf(int index){
  stalectr--;
  StaleBuffer[index].isReal = false;
}


int get_stale_buf(int addr){
  for(int k= 0; k < STALE_BUF_SIZE; k++)
  {
    if (StaleBuffer[k].addr == addr && StaleBuffer[k].isReal)
    {
      return k;
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
  // FILE *tifrep; 
  int nonmemops;
  char opertype;
  long long int taddr;
  long long int instrpc;


  tif = fopen(argv[2], "r");
  // tifrep = fopen(argv[2], "r");

  int addr;
  int label;
  unsigned long long int trace_max = 4000000001;

  if (RAND_ENABLE)
  {
    trace_max = 10000001;
  }
  

  for(unsigned long long int i = 0; i < trace_max; i++)
  {
    if (!RAND_ENABLE)
    {
      // if (fgets(newstr,64,tif)) {
      //   if (sscanf(newstr,"%d %c %Lx %Lx",&nonmemops,&opertype,&taddr,&instrpc) < 1) {
      //     printf("Panic.  Poor trace format.\n");
      //     print_oram_stats();
      //     exit(1);
      //     }
      //   addr = block_addr(byte_addr(taddr));
      // }
      // else if (fgets(newstr,64,tifrep)) {
      //   if (sscanf(newstr,"%d %c %Lx %Lx",&nonmemops,&opertype,&taddr,&instrpc) < 1) {
      //     printf("Panic.  Poor trace format.\n");
      //     print_oram_stats();
      //     exit(1);
      //     }
      //   addr = block_addr(byte_addr(taddr));
      // }
      if (i < 3700000000)
      {
        if (!fgets(newstr,64,tif)) {
          fclose(tif);
          tif = fopen(argv[2], "r");
          fgets(newstr,64,tif);
        }
        else 
        {
          if (sscanf(newstr,"%d %c %Lx %Lx",&nonmemops,&opertype,&taddr,&instrpc) < 1) {
            printf("Panic.  Poor trace format.\n");
            print_oram_stats();
            exit(1);
            }
          addr = block_addr(byte_addr(taddr));
        }
      }
      else
      {
        addr = rand() % BLOCK;
      }
    }
    else
    {
      addr = rand() % BLOCK;
    }
    

    label = PosMap[addr];


    record_util_level();
    record_util_snap();

    // if (true) //(!RAND_ENABLE)
    // {
    //   if (i <= 10000000 )
    //   {
    //     if (i % 4000000 == 0 )
    //     {
    //       export_intermed(exp_name, i/1000000, util_avg);
    //       // reset_util_level();
    //     }
    //   }
    //   else if(i <= 90000000 )
    //   {
    //     if (i % 20000000 == 0 )
    //     {
    //       export_intermed(exp_name, i/1000000, util_avg);
    //       // reset_util_level();
    //     }
    //   }
    //   else if(i <= 300000000 )
    //   {
    //     if (i % 50000000 == 0 )
    //     {
    //       export_intermed(exp_name, i/1000000, util_avg);
    //       // reset_util_level();
    //     }
    //   }
    //   else if(i <= 400000000 )
    //   {
    //     if (i % 10000000 == 0 )
    //     {
    //       export_intermed(exp_name, i/1000000, util_avg);
    //       // reset_util_level();
    //     }

    //     if (i == 400000000 )
    //     {
    //      export_intermed(exp_name, 401, util_overall);
    //     }

    //   }
    // }

    if (i % 100000000 == 0)
    {
       export_intermed(exp_name, i/1000000, util_avg, 100);
    }

    if (i % 50000000 == 0)
    {
       export_intermed(exp_name, i/1000000, util_avg, 50);
    }

    if (i % 20000000 == 0)
    {
       export_intermed(exp_name, i/1000000, util_avg, 20);
    }

    if (i % 1000000000 == 0)
    {
       export_intermed(exp_name, i/1000000, util_overall, i % 1000000000);
    }
    
    
    
    

  
    
    
    
    


    stash_dist[stashctr]++;

    if (label == -1)
    {
      printf("ERROR: block label not found in pos map!\n");
      print_oram_stats();
      exit(1);
    }
    

    if (true) // (!RAND_ENABLE)
    {
      oramctr++;
      read_path(label);
      
      remap_block(addr);

      write_path(label);
    }
    else
    {
      freecursive_access(addr, 'R');
    }
    
    

    
    while (BK_EVICTION && bk_evict_needed())
    {
      // printf("\nbefore bk evict @ stash %d\n", stashctr);
      int randpath = rand() % PATH;
      read_path(randpath);
      write_path(randpath);
      bkctr++;
      // printf("after bk evict @ stash %d\n", stashctr);
    } 

    
    fflush(stdout);


  }

  printf("finished.\n");
  switch_sim_enable_to(true);
  export_csv(argv);
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
    printf("ERROR: block %d label not found in pos map! ~> label %x \n", addr, label);
    print_oram_stats();
    exit(1);
  }
    
  read_path(label);
  
  remap_block(addr);

  write_path(label);

}

// Freecursive 4.2.4 ORAM access algorithm
void freecursive_access(int addr, char type){
  
  // bool posneeded = false;


  if (stash_contain(addr))      // check if the block is already in the stash
  {
    stash_hit++;
    return;
  }

  // printf("%d  %c %s dirty evict\n", addr, type, dirty_evict?"is ":"not"); 

  
  if (LLC_DIRTY && dirty_evict)
  {
    // printf("freecursive: llc dirty add to stash %d\n", addr); 
    int pl = rand() % PATH_VAR;
    PosMap[addr] = pl;
    Slot s = {.addr = addr , .label = pl, .isReal = true, .isData = true};
    int ats = add_to_stash(s);
    if(ats == -1)
    {
      printf("ERROR: freecursive: llc dirty stash overflow!   @ %d\n", stashctr); 
      print_oram_stats();
      exit(1);
    }
  }
  

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
        unsigned long long int ai = addr/pow(X,i);
        unsigned long long int tag = concat(i, ai);  // tag = i || ai  (bitwise concat)

        // printf("@ trace %d  i: %d   ai: %x    tag: %x\n", tracectr, i, ai, tag);

        
        if (plb_access(tag) || buffer_contain(tag))  // PLB hit
        {
          
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
        // posneeded = true;
        int ai = addr/pow(X,i_saved);
        int tag = concat(i_saved, ai);  // tag = i || ai  (bitwise concat)


        if (tag == addr)
        {
          return;
        }
      

        if (!stash_contain(tag)) // access oram tree iff block does not exist in the stash
        {
          unsigned long long int caddr = tag << ((unsigned long long int) log2(BLOCK_SIZE));

          if (LLC_DIRTY)
          {
            if (cache_access(caddr, 'R'))
            {
              Slot mi = {.addr = tag , .label = -1, .isReal = true, .isData = false};
              int as = add_to_stash(mi);
              if(as == -1)
              {
                printf("ERROR: freecursive: cache exist stash overflow!   @ %d\n", stashctr); 
                print_oram_stats();
                exit(1);
              }
            }
          }
          



          cache_invalidate(caddr);
          // printf("cache invalidated block %d\n", tag);
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
          if (WSKIP_ENABLE && type == 'W'  && stashctr < SKIP_LIMIT-1 /*&& wl_occ < WL_CAP*/)
          {
            wl_pos[i_saved]++;
          }
          
          
        }

          int si;
          int victim = plb_fill(tag);
          if( victim != -1)
          {

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
            stt_invalidate(tag);
          }
          
          
        
        
        i_saved--;
      }

    }


  // printf("freecursuve: b4 last oram access (data): %d\n", addr);
  // oram_access(addr);  // STEP 3   Data block access
  if (RING_ENABLE)
  {
    Slot sl = {.addr = addr , .label = PosMap[addr], .isReal = true, .isData = true};

    if (WSKIP_ENABLE && type == 'W'  && stashctr < SKIP_LIMIT && stalectr <= STALE_TH  /*!pause_skip && !posneeded && wl_occ < WL_CAP*/)
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
        printf("ERROR: freecursive: write skip: stash overflow!   @ %d\n", stashctr); 
        print_oram_stats();
        exit(1);
      }
      lingered++;
      wskip++;
      wl_occ++;

      
      int asb = add_stale_buf(sl);
      if (asb == -1)
      {
        printf("ERROR: freecursive: write skip: stale buffer overflow!   @ %d\n", stalectr); 
        print_oram_stats();
        exit(1);
        // pause_skip = true;
      }
      

      
    }
    else
    {
      ring_access(addr);
    }
    
    
  }
  else
  {
    if (!LLC_DIRTY || !dirty_evict)
    {
      oram_access(addr);
    }
    
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
            addr[numc] = byte_addr(addr[numc]);

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
            //  printf("test oram: no cache trace: %d  \n", tracectr_test);
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
      addr[numc] = byte_addr(addr[numc]); 
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




  // printf("\n............... Test ORAM Stats ...............\n");
  printf("trace test ctr %d\n", tracectr_test);
  print_shuff_stat();
  export_csv(argv);
  exit(0);

}




void random_test(char * argv[]){
  int i = 0;
  while (i < TRACE_SIZE)
  {
  srand(time(NULL)); 
   int addr = rand() % BLOCK;
   invoke_oram(addr, 0, 0, 0, 0, 'R');
   i++;
  }
  export_csv(argv);
  exit(0);
  
}


void random_trace(){
  if (chdir("/mnt/c/random") != 0)  
  {
    perror("chdir() to /mnt/c/random failed"); 
  }
  for (int k = 1; k < 14; k++)
  {
    FILE *fp;
    char filename[100];
    int ind = k;
    sprintf(filename, "random%d", ind);
    fp = fopen(filename,"w+");
    for (int i = 0; i < 20000000; i++)
    {
      int nonmemops = rand() % 300;
      char opertype = ((rand()%2) == 1) ? 'R' : 'W';
      long long int addr = rand() % BLOCK;
      int offset = rand() % BLOCK_SIZE;
      addr = addr << ((unsigned int)log2(BLOCK_SIZE)) | offset;
      long long int instrpc = 0;
      fprintf(fp, "%d %c 0x%Lx 0x%Lx\n", nonmemops, opertype, addr, instrpc);
    }
  }
  

 exit(0);  
}

void invoke_oram(long long int physical_address, long long int arrival_time, int thread_id, int instruction_id, long long int instruction_pc, char type) {
    
  invokectr++;
  // bool now = false;
  // if (tracectr == 22300000)
  // {
  //   printf("@invoke %d \n", tracectr);
  //   now = true;
  // }
  
  // printf("trace %d  deadQ size %d\n", tracectr, deadQ->size);
  
  orig_addr = physical_address;
  orig_cycle = arrival_time; 
  orig_thread = thread_id; 
  orig_instr = instruction_id; 
  orig_pc = instruction_pc;

  // if (now)
  // {
  //   printf("invoke... %d \n", tracectr);
  // }
  
  // if (invokectr >= 0 && RING_ENABLE)
  // {
  //   if (invokectr % 100000 == 0)
  //   {
  //      int ind = 0;
  //     deadarr[ind] = deadctr;
  //     dead_on_path_arr[ind] = (int)dead_on_path/ring_evictctr;
  //   }
  // }

  //  if (now)
  // {
  //   printf("invoke... %d \n", tracectr);
  // }
  

  // int flip = rand() % 2;
  // if (flip)
  // {
  //   type = 'R';
  // }
  

  if (type == 'W')
  {
    writectr++;
  }
  else if(type == 'R')
  {
    readctr++;
  }
  
  

  //  if (now)
  // {
  //   printf("invoke... %d \n", tracectr);
  // }
  // int addr = (int)(physical_address & (BLOCK-1));
  unsigned int addr = block_addr(physical_address);
  // printf("invoke oram: physical addr: %lld\n", addr);
  // printf("invoke oram: b4 freecursive call addr: %d\n", addr);
  //  if (now)
  // {
  //   printf("invoke... %d \n", tracectr);
  // }

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
      // printf("rho @ trace %d  addr %d   label %d \n", tracectr, addr, label);
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

  // printf("oram @ trace %d  addr %d\n", tracectr, addr);
 
  switch_tree_to(ORAM);

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

  // if (label == 16569402)
  // {
  //   printf("@ trace %d  label %d     addr %d\n", tracectr, label, addr);
  // }
  


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

    //  if (label == 16569402)
    // {
    //   printf("@ trace %d  label %d     addr %d\n", tracectr, label, addr);
    // }
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
  switch_tree_to(ORAM); // added 6/17/2021
  


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

    if (LLC_DIRTY)
    {
      // printf("freecursive: llc dirty add to stash %d\n", addr); 
      // int pl = rand() % PATH_VAR;
      // PosMap[addr] = pl;
      Slot s = {.addr = addr , .label = label, .isReal = true, .isData = true};
      int ats = add_to_stash(s);
      if(ats == -1)
      {
        printf("ERROR: early writeback: llc dirty stash overflow!   @ %d\n", stashctr); 
        print_oram_stats();
        exit(1);
      }
      pinOn();
    }

    switch_tree_to(ORAM);     // switch to oram tree 
    switch_enqueue_to(HEAD);
    read_path(label);
    remap_block(addr);
    write_path(label);
    switch_enqueue_to(TAIL);  // switch back to normal tail enqueue

     if (LLC_DIRTY)
     {
       pinOff();
     }

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

int shuf_calc(){
  int sum = 0;
  for (int i = 0; i < 10; i++)
  {
    sum += shuff[i];
  }
  return sum;
}

void ring_access(int addr){
  // int before = stashctr;

  int shuf_cur = shuf_calc();
  shuf_dif = shuf_cur - shuf_prev;

  // printf("%d\n", shuf_dif);

  ringacc++;

  

  int label;

  
  if (ring_dummy && DUMMY_ENABLE)
  {
    ringdumctr++;
    label = rand() % PATH;
  }
  else
  {
    ringctr++;
    label = PosMap[addr];
  }
  
  

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

  if (!ring_dummy)
  {
    remap_block(addr);
  }

  // printf("@> remap block  trace %d\n", tracectr);

  ring_round = (ring_round + 1) % RING_A; 

  // int rl;

  bool ep_cond = DYNAMIC_EP ? (shuf_dif >= DEP_TH) : (ring_round == 0);

  // if (ring_round == 0)
  if (ep_cond)
  {
    // printf("\n@- evict %d\n", stalectr);
    int dead_b4 = deadctr;
    int dead_b4_dram = dead_dram;
    int b4 = stalectr;
    ring_evict_path(label);
    stale_reduction += b4 - stalectr;

    dead_on_path += dead_b4 - deadctr;
    dead_on_path_dram += dead_b4_dram - dead_dram;
    // printf("@> evict %d\n", stalectr);
    
    // to be removed
    // ring_evictctr++;
    // rl = reverse_lex(ring_G);
    // read_path(rl);

    // printf("%d\n", ringacc);
    shuf_prev = shuf_cur;
  }
  
  
  ring_early_reshuffle(label);


  // to be removed
  // if (ring_round == 0)
  // {
  //   write_path(rl);
  //   ring_G++;
  // }

  // int diff = stashctr - before;
  // if (diff > 1)
  // {
  //   printf("%d\n", diff);
  // }
  
  // printf("\n@> %d\n", ringctr);
  // for (int i = GATHER_START; i < LEVEL; i++)
  // {
  //   printf("deadQ[%d]: %d\n", i, deadQ_arr[i]->size);
  // }
  

}

void bin(unsigned int n)
{
    unsigned i;
    for (i = 1 << (LEVEL-1); i > 0; i = i / 2)
        (n & i) ? printf("1") : printf("0");
}



// void bucket_meta_access(int index){
//   int mem_addr = DATA_ADDR_SPACE + index;
//   // bool nvm_access = is_nvm_addr(mem_addr);
//   // bool nvm_access = in_nvm(i);
//   bool beginning = false;
//   bool ending = false;
//   char op_type = 'b';
//   insert_oramQ(mem_addr, orig_cycle, orig_thread, orig_instr, orig_pc, 'R', false, false, op_type, beginning, ending);
//   insert_oramQ(mem_addr, orig_cycle, orig_thread, orig_instr, orig_pc, 'W', false, false, op_type, beginning, ending);
// }

void metadata_access(int label, char type){
  int last_read = false;
  for (int i = LEVEL-1; i >= 0; i--)
  {
     if (i >= TOP_CACHE_VAR && SIM_ENABLE_VAR)
    {
      if (i == TOP_CACHE_VAR ) // && type == 'R')
      {
        last_read = true;
      }
      int mem_addr = DATA_ADDR_SPACE + calc_index(label, i);
      // bool nvm_access = is_nvm_addr(mem_addr);
      bool nvm_access = in_nvm(i);
      nvm_access = false;   // assume all metadta is in dram, comment this line if intend otherwise
      bool beginning = (type == 'R') && (i == LEVEL-1);
      bool ending = (type == 'W') && (i == TOP_CACHE_VAR);
      char op_type = 'm';
      insert_oramQ(mem_addr, orig_cycle, orig_thread, orig_instr, orig_pc, type, last_read, nvm_access, op_type, beginning, ending, i);
    }
  }

}



// gathering dead blk info into the queue
void gather_dead(int index, int i){
  // if it's not last level (leaf level) add to deadq
  int upperBound = DYNAMIC_S ? LEVEL : NVM_START;
  if (i < upperBound && i >= GATHER_START)
  {
    // printf("deadQ[%d] : %d\n", i, deadQ_arr[i]->size);
    int start = SURONLY_ENABLE ? LZ[i] : 0 ;
    int end = DYNAMIC_S ? LZ[i] : Z;
    // int cap = i < 15 ? 1 : 1; 
    // int cap = 10;
    for (int j = start; j < end; j++)
    {
      // if (!GlobTree[index].slot[j].isReal)
      if (GlobTree[index].slot[j].dd == DEAD)
      {
        dead_encountered[i]++;
        if (GlobTree[index].slot[j].dd == DEAD && GlobTree[index].allctr < REMOTE_CAP) //&& GlobTree[index].count < 6) 
        {
          Element *db = (Element*) malloc(sizeof (Element));
          db->index = index;
          db->offset = j;


          // if (deadQ->size >= deadQ->limit)
          // {
          //   // Dequeue(deadQ);
          //   Element *oldest = Dequeue(deadQ);
          //   GlobTree[oldest->index].slot[oldest->offset].dd = DEAD;
          //   GlobTree[oldest->index].allctr--;
          //   free(oldest);
          // }
          // else
          // {
          //   deadQ_ov[i]++;
          // }
          // Enqueue(deadQ, db);
          // dead_gathered[i]++;
          // deadrem++;
          // GlobTree[index].slot[j].dd = REMEMBERED;
          // GlobTree[index].allctr++;


          if (deadQ_arr[i]->size < deadQ_arr[i]->limit)
          {
            GlobTree[index].slot[j].dd = REMEMBERED;
            GlobTree[index].allctr++;

            deadrem++;
            Enqueue(deadQ_arr[i] , db);
            dead_gathered[i]++;
          }
          else
          {
            if (deadQ_shadow[i]->size < deadQ_shadow[i]->limit)
            {
              Enqueue(deadQ_shadow[i] , db);
              dead_shadowed[i]++;
            }
            else
            {
              deadQ_ov[i]++;
              free(db);
              break;
            }

          }
                    
        }
        else if(GlobTree[index].allctr == REMOTE_CAP)
        {
          if (deadQ_arr[i]->size < deadQ_arr[i]->limit)
          {
            cap_Q_notfull[i]++;
          }
          else
          {
            cap_Q_full[i]++;
          }
          
        }
      }
    }
  }
}

bool scan_entire_level(int level, int original_bucket, int * index, int * offset){

  int start = calc_index(0, level);
  int end = calc_index(PATH-1, level);

  for (int i = start; i <= end; i++)
  {

    if (i != original_bucket)
    {
      for (int j = 0; j < LZ[level]; j++)
      {
        if (GlobTree[i].allctr < REMOTE_CAP && GlobTree[i].slot[j].dd == DEAD)
        {
          *index = i;
          *offset = j;
          return true;
        }
      }
      
    }
  }
  return false;
  
}

void remote_invalidate(int index, int offset){
  if (!GlobTree[index].slot[offset].redirect)  // the slot is dedicated to the containing block and no redirecting is needed 
  {
    GlobTree[index].slot[offset].dd = DEAD;  // mark the slot as dead so that it can be used by others as well
  }
  else // this slot is being either reserved in queue or used by others and a remote slot is holding the block we want to invalidate
  {
    int index_redir = GlobTree[index].slot[offset].remote_index;
    int offset_redir = GlobTree[index].slot[offset].remote_offset;
    GlobTree[index_redir].slot[offset_redir].dd = DEAD; // invalidate the slot farther away that physically contains the current block 
  }
}

int remote_allocate(int index, int offset){
  // Element *cand;
  int i = -1;
  int j = -1;
  int level = calc_level(index);
  // int input_level = level;

  // if (in_nvm(level))
  // {
  //   level = NVM_START-1;
  // }
  if (deadQ_arr[level]->size == 0)
  {
    if (offset == 10)
    {
      deadQ_empty_s6[level]++;
    }
    else if(offset == 11)
    {
      deadQ_empty_s7[level]++;
    }
    
  }

  // printf("level %d \n", level);
  // preferred level to look for dead blk
  if (offset < LZ_VAR[level] || deadQ_arr[level]->size > 0 * deadQ_arr[level]->limit)
  {
    while (deadQ_arr[level]->size > 0)
    {
      // printf("here\n");
      Element *cand = Dequeue(deadQ_arr[level]);
      int i_tmp = cand->index;
      int j_tmp = cand->offset;
      // ??? taken condition to be deleted, a slot in queue not supposed to be taken ever
      bool taken = (GlobTree[i_tmp].slot[j_tmp].dd == ALLOCATED) || (GlobTree[i_tmp].slot[j_tmp].dd == REFRESHED);
    
      if (!taken)
      {
        // printf("break\n");
        deadrem--;
        i = cand->index;
        j = cand->offset;
        if (j >= LZ[level])
        {
          surplus_in_use++;
        }
        
        free(cand);
        break;
      }
      else
      {
        printf("ERROR: remote allocate block in queue is taken! REFRESHED? %d  ALLOCATED? %d\n", (GlobTree[i_tmp].slot[j_tmp].dd == REFRESHED), (GlobTree[i_tmp].slot[j_tmp].dd == ALLOCATED));
        exit(1);
      }

      // printf("SAME level taken %s  %s %d\n", taken?"yes":"on", (GlobTree[i_tmp].slot[j_tmp].dd == ALLOCATED)?"ALLOCATED": (GlobTree[i_tmp].slot[j_tmp].dd == REFRESHED)?"REFERESHED":"unknown", tracectr);
      // printf("hereeeeeeeeeeee taken %s  %s %d\n", taken?"yes":"on", (GlobTree[i_tmp].slot[j_tmp].dd == ALLOCATED)?"ALLOCATED":"unknown", tracectr);
      free(cand);
    }
  }




    // if not found look for dead blk at every level else and start from the bottom 
  // if (i == -1 && j == -1 && offset < LZ[level])
  // {
   
  //   for (int l = LEVEL-1; l >= GATHER_START; l--)
  //   {
  //     if (l != level)
  //     {
  //       while (deadQ_arr[l]->size != 0)
  //       {
  //         // printf("here\n");
  //         Element *cand = Dequeue(deadQ_arr[l]);
  //         int i_tmp = cand->index;
  //         int j_tmp = cand->offset;
  //         bool taken = (GlobTree[i_tmp].slot[j_tmp].dd == ALLOCATED) || (GlobTree[i_tmp].slot[j_tmp].dd == REFRESHED);
        
  //         if (!taken)
  //         {
  //           // printf("break\n");
  //           deadrem--;
  //           i = cand->index;
  //           j = cand->offset;
  //           free(cand);
  //           break;
  //         }
  //         // printf("ESLE level taken %s  %s %d\n", taken?"yes":"on", (GlobTree[i_tmp].slot[j_tmp].dd == ALLOCATED)?"ALLOCATED": (GlobTree[i_tmp].slot[j_tmp].dd == REFRESHED)?"REFERESHED":"unknown", tracectr);


  //         free(cand);
  //       }
  //       if (i != -1 && j != -1)
  //       {
  //         if (j >= LZ[l])
  //         {
  //           surplus_in_use++;
  //         }
  //         break;
  //       }
  //     }
      
  //   }
  // }

  if (i == -1 && j == -1)
  {
    bool found = scan_entire_level(level, index, &i, &j);
    if (found && (i == -1 || j == -1))
    {
      printf("ERROR: remote allocate scan found but inputs not set!\n");
      exit(1);
    }
    else if(found)
    {
      dead_scan[level]++;
    }
  }
  

  if (i != -1 && j != -1)
  {
    if (offset < LZ_VAR[level])
    {
      Q_serve_under[level]++;
    }
    else
    {
      Q_serve_over[level]++;
    }
    
    GlobTree[i].slot[j].dd = ALLOCATED;
    ddctr--;
    // ddctr_arr[level]--;
    // GlobTree[i].allctr++;
    if (offset == i)
    {
      same_bucket++;
    }
    
    GlobTree[index].slot[offset].redirect = true;
    GlobTree[index].slot[offset].remote_index = i;
    GlobTree[index].slot[offset].remote_offset = j;
    int mem_addr = i*Z_VAR + j;
    // bucket_meta_access(i);
    return mem_addr;
  }
  return -1;
}

// int remote_allocate(int index, int offset){
//   // Element *cand;
//   int i = -1;
//   int j = -1;
//   int level = calc_level(index);
//   int input_level = level;

//   if (in_nvm(level))
//   {
//     level = NVM_START-1;
//   }
  

//   // preferred level to look for dead blk
//   while (deadQ_arr[level]->size != 0)
//   {
//     // printf("here\n");
//     Element *cand = Dequeue(deadQ_arr[level]);
//     int i_tmp = cand->index;
//     int j_tmp = cand->offset;
//     // ??? taken condition to be deleted, a slot in queue not supposed to be taken ever
//     bool taken = (GlobTree[i_tmp].slot[j_tmp].dd == ALLOCATED) || (GlobTree[i_tmp].slot[j_tmp].dd == REFRESHED);
  
//     if (!taken)
//     {
//       // printf("break\n");
//       deadrem--;
//       i = cand->index;
//       j = cand->offset;
//       if (j >= LZ[level])
//       {
//         surplus_in_use++;
//       }
      
//       free(cand);
//       break;
//     }

//     // printf("SAME level taken %s  %s %d\n", taken?"yes":"on", (GlobTree[i_tmp].slot[j_tmp].dd == ALLOCATED)?"ALLOCATED": (GlobTree[i_tmp].slot[j_tmp].dd == REFRESHED)?"REFERESHED":"unknown", tracectr);
//     // printf("hereeeeeeeeeeee taken %s  %s %d\n", taken?"yes":"on", (GlobTree[i_tmp].slot[j_tmp].dd == ALLOCATED)?"ALLOCATED":"unknown", tracectr);
//     free(cand);
//   }

//   // if not found look for dead blk at every level else and start from the bottom 
//   if (i == -1 && j == -1)
//   {
//     if (in_nvm(input_level))
//     {
//       dram_elselevel++;
//     }
//     else
//     {
//       nvm_elselevel++;
//     }
    
//     for (int l = NVM_START-1; l >= GATHER_START; l--)
//     {
//       if (l != level)
//       {
//         while (deadQ_arr[l]->size != 0)
//         {
//           // printf("here\n");
//           Element *cand = Dequeue(deadQ_arr[l]);
//           int i_tmp = cand->index;
//           int j_tmp = cand->offset;
//           bool taken = (GlobTree[i_tmp].slot[j_tmp].dd == ALLOCATED) || (GlobTree[i_tmp].slot[j_tmp].dd == REFRESHED);
        
//           if (!taken)
//           {
//             // printf("break\n");
//             deadrem--;
//             i = cand->index;
//             j = cand->offset;
//             free(cand);
//             break;
//           }
//           // printf("ESLE level taken %s  %s %d\n", taken?"yes":"on", (GlobTree[i_tmp].slot[j_tmp].dd == ALLOCATED)?"ALLOCATED": (GlobTree[i_tmp].slot[j_tmp].dd == REFRESHED)?"REFERESHED":"unknown", tracectr);


//           free(cand);
//         }
//         if (i != -1 && j != -1)
//         {
//           if (j >= LZ[l])
//           {
//             surplus_in_use++;
//           }
//           break;
//         }
//       }
      
//     }
//   }
  
  

//   if (i != -1 && j != -1)
//   {
//     GlobTree[i].slot[j].dd = ALLOCATED;
//     // GlobTree[i].allctr++;
//     GlobTree[index].slot[offset].redirect = true;
//     GlobTree[index].slot[offset].remote_index = i;
//     GlobTree[index].slot[offset].remote_offset = j;
//     int mem_addr = i*Z_VAR + j;
//     // bucket_meta_access(i);
//     return mem_addr;
//   }
//   return -1;
// }

int inplace_allocate(int index, int offset){
  // if (GlobTree[index].slot[offset].dd == REMEMBERED)
  // {
  //   printf("inplace alloc REMEMBERED\n");
  // }
  // else if (GlobTree[index].slot[offset].dd == ALLOCATED)
  // {
  //   printf("inplace alloc ALLOCATED\n");
  // }
  // else if (GlobTree[index].slot[offset].dd == DEAD)
  // {
  //   printf("inplace alloc DEAD\n");
  // }
  // else if (GlobTree[index].slot[offset].dd == REFRESHED)
  // {
  //   printf("inplace alloc REFRESHED\n");
  // }
  int mem_addr = index*Z_VAR + offset;
  GlobTree[index].slot[offset].redirect = false;
  GlobTree[index].slot[offset].dd = REFRESHED;
  ddctr--;
  return mem_addr;
}


int inplace_access(int index, int offset){
  int mem_addr = index*Z_VAR + offset;
  // if (!GlobTree[index].slot[offset].valid)
  // {
  //   printf("inplace access invalid block\n");
  // }
  
  GlobTree[index].slot[offset].dd = DEAD;
  // GlobTree[index].slot[offset].valid = false;
  // ddctr++;
  return mem_addr;
}

int remote_access(int index, int offset, int level){
  int index_redir = GlobTree[index].slot[offset].remote_index;
  int offset_redir = GlobTree[index].slot[offset].remote_offset;
  int mem_addr = index_redir*Z_VAR + offset_redir;
  GlobTree[index].slot[offset].redirect = false; // ??? added 4/13/2021 7:53 pm
  GlobTree[index_redir].slot[offset_redir].dd = DEAD; // invalidate the slot farther away that physically contains the current block 
  GlobTree[index_redir].allctr--;
  
  // GlobTree[index].slot[offset].valid = false;
  // ddctr++;
  // ddctr_arr[level]++;
  // bucket_meta_access(index_redir);

  if (offset_redir >= LZ[level])
  {
    surplus_in_use--;
  }

  if (level >= GATHER_START)
  {
    int rind = index_redir;
    int roff = offset_redir;
    if (GlobTree[rind].slot[roff].dd == DEAD && GlobTree[rind].allctr < REMOTE_CAP)
    {
      Element *db = (Element*) malloc(sizeof (Element));
      db->index = rind;
      db->offset = roff;

      if (deadQ_arr[level]->size < deadQ_arr[level]->limit)
      {
        GlobTree[rind].slot[roff].dd = REMEMBERED;
        GlobTree[rind].allctr++;

        deadrem++;
        Enqueue(deadQ_arr[level] , db);
        dead_gathered[level]++;
      }
      else
      {
        if (deadQ_shadow[level]->size < deadQ_shadow[level]->limit)
        {
          Enqueue(deadQ_shadow[level] , db);
          dead_shadowed[level]++;
        }
        else
        {
          deadQ_ov[level]++;
          free(db);
          // break;
        }

      }
      
    }
  }
  
  return mem_addr;
}

bool in_nvm(int level){
  if (NVM_ENABLE && level >= NVM_START)
  {
    return true;
  }
  return false;
}



int calc_mem_addr(int index, int offset, char type)
 {
  int mem_addr = 0;
  int level = calc_level(index);

  // if (type == 'W')
  // {
  // }

  // printf("%d %c\n", level, type);
  


  if (level < TOP_CACHE)
  {
    return 0;
  }

  if (!DEAD_ENABLE_VAR)
  {
    if (type == 'R')
    {
      dram_norm_r++;
    }
    else if (type == 'W')
    {
      dram_norm_w++;
    }
    mem_addr = index*Z_VAR + offset;
  }
  else if (type == 'R')
  {
    if (GlobTree[index].slot[offset].redirect)
    {
      mem_addr = remote_access(index, offset, level);

      dram_remote_r++;
      if (offset < LZ_VAR[level])
      {
        remote_under_r++;
      }
      else
      {
        remote_over_r++;
      }

      
    }
    else
    {
      mem_addr = inplace_access(index, offset);
      // ddctr_arr[level]++;


      dram_inplace_r++;
    }
  }
  else if (type == 'W')
  {
    if (offset < LZ[level]) // for slots within original Z + S (5 + 5)
    {
      if (GlobTree[index].slot[offset].dd == DEAD || GlobTree[index].slot[offset].dd == REFRESHED) 
      {
        // if (GlobTree[index].slot[offset].dd == DEAD)
        // {
        //   ddctr_arr[level]--;
        // }
        mem_addr = inplace_allocate(index, offset);
        dram_inplace_w++;
      }
      else if (GlobTree[index].slot[offset].dd == ALLOCATED || GlobTree[index].slot[offset].dd == REMEMBERED) // the case that redirect needed ~> find another dead blk to fill in from the queue
      {
        // if (deadQ->size == 0)
        // {
        //   printf("ERROR: calc mem addr queue empty!\n");
        //   export_csv(pargv);
        //   exit(1);
        // }

        mem_addr = remote_allocate(index, offset);
        // if (mem_addr == -1)
        // {
        //   printf("ERROR: calc mem addr @ level %d no available cand in queue!\n", level);
        //   export_csv(pargv);
        //   exit(1);
        // }
        if (mem_addr != -1)
        {
          dram_remote_w++;
          remote_under_w++;
        }
      }
    }
    else  // for slots beyond original Z + S (5 + 5)
    {
      mem_addr = -1;
      // printf("level %d\n", level);
      // if ( (remote_nvms < REMOTE_ALLOC_RATIO*(dead_dram + surplus_dead ))  &&  (level == NVM_START) && calc_deadQ_size() > 840) // && tracectr > 5000000 stop remote allocate if dead blk allocated to leaf is more than a threshold
      // {
        mem_addr = remote_allocate(index, offset);
      // }
      
      if (mem_addr != -1)
      {
        dram_remote_w++;
        remote_over_w++;
      }

    }
    
    
  }
  else
  {
    printf("ERROR: calc mem addr %d %d %c\n", index, offset, type);
    export_csv(pargv);
    exit(1);
  }
  return mem_addr;
}


// int calc_mem_addr(int index, int offset, char type)
//  {
//   int mem_addr = 0;
//   int level = calc_level(index);

//   // if (type == 'W')
//   // {
//   // }

//   // printf("%d %c\n", level, type);
  


//   if (level < TOP_CACHE)
//   {
//     return 0;
//   }

//   if (!DEAD_ENABLE_VAR)
//   {
//     if (type == 'R')
//     {
//       nvm_norm_r = in_nvm(level) ? nvm_norm_r+1 : nvm_norm_r;
//       dram_norm_r = (!in_nvm(level)) ? dram_norm_r+1 : dram_norm_r;
//     }
//     else if (type == 'W')
//     {
//       nvm_norm_w = in_nvm(level) ? nvm_norm_w+1 : nvm_norm_w;
//       dram_norm_w = (!in_nvm(level)) ? dram_norm_w+1 : dram_norm_w;
//     }
//     mem_addr = index*Z_VAR + offset;
//   }
//   else if (type == 'R')
//   {
//     if (GlobTree[index].slot[offset].redirect)
//     {
//       mem_addr = remote_access(index, offset, level);

//       nvm_remote_r = in_nvm(level) ? nvm_remote_r+1 : nvm_remote_r;
//       dram_remote_r = (!in_nvm(level)) ? dram_remote_r+1 : dram_remote_r;
//       remote_nvms = in_nvm(level) ? remote_nvms-1 : remote_nvms;
      
//     }
//     else
//     {
//       mem_addr = inplace_access(index, offset);

//       nvm_inplace_r = in_nvm(level) ? nvm_inplace_r+1 : nvm_inplace_r;
//       dram_inplace_r = (!in_nvm(level)) ? dram_inplace_r+1 : dram_inplace_r;
//     }
//   }
//   else if (type == 'W')
//   {
//     if (!in_nvm(level)) // for levels in dram
//     {
//       if (GlobTree[index].slot[offset].dd == DEAD || GlobTree[index].slot[offset].dd == REFRESHED) 
//       {
//         mem_addr = inplace_allocate(index, offset);
//         dram_inplace_w++;
//       }
//       // else if (GlobTree[index].slot[offset].dd == REMEMBERED)  // use this dead blk and remove it from the queue? $$$ no removing for now
//       // {
//       //   mem_addr = inplace_allocate(index, offset);
//       //   // bool discarded = remove_dead(deadQ, index, offset);
//       //   // if (!discarded)
//       //   // {
//       //   //   printf("ERROR: calc mem addr remembered block not found in the queue!\n");
//       //   //   export_csv(pargv);
//       //   //   exit(1);
//       //   // }
        
//       //   dram_inplace_w_remembered++;
//       // }
//       else if (GlobTree[index].slot[offset].dd == ALLOCATED || GlobTree[index].slot[offset].dd == REMEMBERED) // the case that redirect needed ~> find another dead blk to fill in from the queue
//       {
//         // if (deadQ->size == 0)
//         // {
//         //   printf("ERROR: calc mem addr queue empty!\n");
//         //   export_csv(pargv);
//         //   exit(1);
//         // }

//         mem_addr = remote_allocate(index, offset);
//         if (mem_addr == -1)
//         {
//           printf("ERROR: calc mem addr @ level %d no available cand in queue!\n", level);
//           export_csv(pargv);
//           exit(1);
//         }
//         dram_remote_w++;
//       }
//     }
//     else  // for the levels in nvm
//     {
//       mem_addr = -1;
//       // printf("level %d\n", level);
//       if ( (remote_nvms < REMOTE_ALLOC_RATIO*(dead_dram + surplus_dead ))  &&  (level == NVM_START) && calc_deadQ_size() > 840) // && tracectr > 5000000 stop remote allocate if dead blk allocated to leaf is more than a threshold
//       {
//         mem_addr = remote_allocate(index, offset);
//       }
      
//       if (mem_addr == -1)
//       {
//         if (GlobTree[index].slot[offset].dd == DEAD || GlobTree[index].slot[offset].dd == REFRESHED)
//         {
//           mem_addr = inplace_allocate(index, offset);
//         }
//         else
//         {
//           printf("ERROR: calc mem addr nvm level allocation failed!\n");
//           export_csv(pargv);
//           exit(1);
//         }
//         nvm_inplace_w++;
//       }
//       else
//       {
//         nvm_remote_w++;
//         remote_nvms++;
//       }

//     }
    
    
//   }
//   else
//   {
//     printf("ERROR: calc mem addr %d %d %c\n", index, offset, type);
//     export_csv(pargv);
//     exit(1);
//   }
//   return mem_addr;
// }


void ring_read_path(int label, int addr){
  // Element *pN = (Element*) malloc(sizeof (Element));
  // pN->addr = label;
  // Enqueue(pathQ, pN);

  if (SIM_ENABLE_VAR)
  {
    metadata_access(label, 'R');
    metadata_access(label, 'W');
  }
  


  // for (int i = 0; i < LEVEL; i++)
  for (int i = LEVEL_VAR-1; i >= EMPTY_TOP_VAR; i--)
  {
    int index = calc_index(label, i);

    // if (GlobTree[index].allctr > 6)
    // {
    //   printf("%d\n", GlobTree[index].allctr);
    // }
    

    int slotCount = DYNAMIC_S ? Z : LZ_VAR[i];  

    if (slotCount < 0 || slotCount > Z)
    {
      printf("ERROR: read path slot count %d out of range!\n", slotCount);
      exit(1);
    }
    
    int offset = rand() % slotCount; 
    while (GlobTree[index].slot[offset].isReal || !GlobTree[index].slot[offset].valid )
    {
      // printf("@%d in ring read S: %d   count: %d\n", tracectr, GlobTree[index].s, GlobTree[index].count);
      offset = rand() % slotCount;
      
    }


    
    
    if (!ring_dummy)
    {
      for(int j = 0; j < LZ_VAR[i]; j++)
      {
        if (GlobTree[index].slot[j].isReal && GlobTree[index].slot[j].addr == addr)
        {
          // printf("\n offset trace %d\n", tracectr);
          
          offset = j;

          // profiling
          if (i <= TOP_BOUNDARY)
          {
            topctr++;
          }
          else if (i <= MID_BOUNDARY)
          {
            midctr++;
          }
          else
          {
            botctr++;
          }
          break;
        }
      }

    // if cb enabled and runout of dummies and this bucket is not returning the block of interest, go pick a real block as a green block
    if (CB_ENABLE && GlobTree[index].count >= LS[i] && !GlobTree[index].slot[offset].isReal)
    {
      while (!GlobTree[index].slot[offset].valid)
      {
        offset = rand() % LZ_VAR[i];
        // printf("trace: %d   dummy: %d\n", tracectr, ring_dummy);
      }
    }


      if (GlobTree[index].slot[offset].isReal)
      {
        realcount[i]++;
        if (i >= LEVEL-2)
        {
          int sind = (i == LEVEL-2) ? calc_super(label, i) : calc_super(label, i-1);
          supreal[sind]++;
        }
        
        
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
          export_csv(pargv);
          print_oram_stats();
          exit(1);
        }
      }
      else
      {
        dumcount[i]++;
        if (i >= LEVEL-2)
        {
          int sind = (i == LEVEL-2) ? calc_super(label, i) : calc_super(label, i-1);
          supdum[sind]++;
        }
      }
    }
    



    if (!GlobTree[index].slot[offset].isReal)
    {
      GlobTree[index].dumval--;
    }
     

    bool last_read = false;

    // deal with redirection
    // if (DEAD_ENABLE)
    // {
    //   remote_invalidate(index, offset);
    // }


    int mem_addr = calc_mem_addr(index, offset, 'R');

    ring_invalidate(index, offset);     // invalidate the block (no matter the block is physically here or somewhere else)
    GlobTree[index].dumdead++;
    deadctr_arr[i]++;
    // ddctr_arr[i]++;
    

    

    
    

    if (i >= TOP_CACHE_VAR && SIM_ENABLE_VAR)
    {
    // printf("ring read mem addr: %d   @ L%d  j: %d \n", mem_addr, i, offset);

      // if (i == LEVEL-1)
      if (i == TOP_CACHE_VAR)
      {
        last_read = true;
      }
      // bool nvm_access = is_nvm_addr(mem_addr);
      bool nvm_access = in_nvm(i);
      // bool beginning = (i == TOP_CACHE_VAR);
      bool beginning = (i == LEVEL_VAR-1);
      // bool ending = (i == LEVEL-1);
      bool ending = (i == TOP_CACHE_VAR);
      char op_type = 'o';
      insert_oramQ(mem_addr, orig_cycle, orig_thread, orig_instr, orig_pc, 'R', last_read, nvm_access, op_type, beginning, ending, i);
    }

    GlobTree[index].count++;
    if (i >= LEVEL-2)
    {
      int sind = (i == LEVEL-2) ? calc_super(label, i) : calc_super(label, i-1);
      SuperNode[sind].count++;
      int hind = calc_horiz(index, label, i);
      if (i == LEVEL-2)
      {
        SuperHoriz1[hind].count++;
      }
      else
      {
        SuperHoriz2[hind].count++;
      }
      
    }

    if (!GlobTree[index].slot[offset].isReal)
    {
      // GlobTree[index].dumdead++;
      deadctr++;
      if (i < NVM_START && i >= TOP_CACHE_VAR)
      {
        dead_dram++;
      }
    }


     
  }
}


int calc_overlap(int pathA, int pathB){
  int count = 0;
  int rootpos = LEVEL-1;
  for (int i = rootpos-1-1; i >= 0; i--)
  {
    int a = (pathA>>i)&0x01;
    int b = (pathB>>i)&0x01;
    if (a == b)
    {
      count++;
    }
    else
    {
      return count;
    }
  }
  return count;
}





void ring_evict_path(int label){
  // printf("\nevict path trace %d\n", tracectr);





  

  lingered--;

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



  // if (ring_evictctr == 1)
  // {
  //   lastpath = label;
  // }

  // if (stashctr < 100)
  // {
  //   int min = LEVEL;
  //   for (int i = 0; i < pathQ->size; i++)
  //   {
  //     Element *pN = Dequeue(pathQ);
  //     int temp = pN->addr;
  //     int ov = calc_overlap(temp, lastpath);
  //     // printf("%d\n", ov);
  //     if ( ov < min)
  //     {
  //       min = ov;
  //       lastpath = temp;
  //     }
  //   }

  //   label = lastpath;
  // }
  
  
  

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
  
  if (SIM_ENABLE_VAR)
  {
    metadata_access(label, 'R');
    metadata_access(label, 'W');
  }
  
  

  ring_G++;
  int b4 = stashctr;
  read_path(label);
  write_path(label);
  wbctr += b4 - stashctr;


  // ring_early_reshuffle(label);

}

int calc_range(int i){
  if (i <= TOP_BOUNDARY)
  {
    return 0;
  }
  else if (i <= MID_BOUNDARY)
  {
    return 1;
  }
  else
  {
    return 2;
  }
  
  return -1;
}

int detect_inplace_available(int index, int level){
  for (int i = 0; i < Z; i++)
  {
    dead_slot[i] = -1;
    taken_slot[i] = -1;
  }
  
  if (DEAD_ENABLE && DYNAMIC_S)
  {
    int dead = 0;
    int k = 0;
    int h = 0;
    for (int j = 0; j < LZ_VAR[level]; j++)
    {
      if (GlobTree[index].slot[j].dd == DEAD || GlobTree[index].slot[j].dd == REFRESHED)
      {
        // printf("L%d: dead[%d]: %d\n", level, k, j);
        dead++;
        dead_slot[k] = j;
        k++;
      }
      else
      {
        taken_slot[h] = j;
        h++;
      }
    }
    return dead;
  }

  for (int i = 0; i < LZ_VAR[level]; i++)
  {
    dead_slot[i] = i;
  }
  

  return LZ_VAR[level];
  
}

int calc_super_in_tree(int index){
  if(index % 2 == 0){
    return index-1;
  }
  else{
    return index+1;
  }

}

bool super_node_need_reshuffle(int index){
  int level = calc_level(index);

  if (!SUPER_ENABLE || level < SUPER_LEVEL)
  {
    return false;
  }
  int adj = calc_super_in_tree(index);
  int touched = GlobTree[index].count + GlobTree[adj].count;
  if (touched >= SUPER_S)
  {
    return true;
  }
  return false;
}


bool is_super_level(int level){
  if (level >= SUPER_LEVEL)
  {
    return true;
  }
  return false;
}

void write_bucket(int index, int label, int level, char op_type){
  wbuck++;
  int allocated = 0;

  if (level == LEVEL - 1 && DEAD_ENABLE_VAR && tracectr > 62000000)
  {
    // printf("@%d W> %d, ", ringctr, deadQ_arr[level]->size);
  }

  reset_candidate();
  pick_candidate(index, label, level);

  int available = detect_inplace_available(index, level);

  int min = 1;

  if (available < RING_Z + min)
  {
    // printf("ERROR: write bucket @ trace %d  level %d  only %d available less than %d!\n", tracectr, level, available, RING_Z + min);
    // exit(1);
  }

  // GlobTree[index].s = available - RING_Z;
  // printf("available %d    s %d\n", available, GlobTree[index].s);
  int real = 0;

  for (int k = 0; k < available; k++)
  {
    int j = dead_slot[k];
    if (j == -1)
    {
      printf("ERROR: write bucket dead slot not available!\n");
      exit(1);
    }
    
    GlobTree[index].slot[j].valid = true;
    allocated++;

    if (GlobTree[index].slot[j].dead_start != 0 && tracectr > DD_SATURATE)
    {
      int lifetime = ringctr - GlobTree[index].slot[j].dead_start;
      GlobTree[index].slot[j].dead_start = 0;
      update_lifetime_stat(lifetime, level);
    }

    int mem_addr = calc_mem_addr(index, j, 'W');

    // printf("%c  mem addr: %d   @ L%d  j: %d \n", op_type, mem_addr, level, j);

    if (mem_addr == -1)
    {
     
      printf("ERROR: write bucket calc mem addr not successful!\n");
      exit(1);
    }

    if (level >= TOP_CACHE_VAR)
    {
      inplacectr++;
    }
    

    if (level >= TOP_CACHE_VAR && SIM_ENABLE_VAR)
    {
      
      // printf("@ level %d  mem addr: %d\n", level,  mem_addr);
      // bool nvm_access = is_nvm_addr(mem_addr);
      bool nvm_access = in_nvm(level);
      bool beginning = false;
      bool ending = (j == LZ_VAR[level]-1);
      bool last_read = (j == LZ_VAR[level]-1);
      insert_oramQ(mem_addr, orig_cycle, orig_thread, orig_instr, orig_pc, 'W', last_read, nvm_access, op_type, beginning, ending, level);
    }

    
    if (candidate[real] != -1 && real < RING_Z) // GlobTree[index].dumnum > GlobTree[index].s)
    {
      // if (tracectr >= 39796059)
      // {
      //   print_candidate();
      //   printf("%c %lld k=%d: cand[%d]: %d\n", op_type, wbuck, k, candidate[k], Stash[candidate[k]].addr);
      // }
      GlobTree[index].slot[j].addr = Stash[candidate[real]].addr;
      GlobTree[index].slot[j].label = Stash[candidate[real]].label;
      GlobTree[index].slot[j].isReal = true;
      GlobTree[index].slot[j].isData = true;
      GlobTree[index].dumnum--;
      GlobTree[index].dumval--;
      real++;

      remove_from_stash(candidate[real-1]);
    }
    else
    {
      if (level >= TOP_CACHE_VAR)
      {
        s_inctr++;
      }
      
    }
  }

  if (level >= GATHER_START && RING_ENABLE && DYNAMIC_S && DEAD_ENABLE_VAR )//&& op_type == 'e')
  {
    for (int h = 0; h < LZ_VAR[level] - available; h++)
    {
      takenctr++;
      int j = taken_slot[h];
      int mem_addr = calc_mem_addr(index, j, 'W');
      if (mem_addr != -1)
      {
        s_underctr++;
        GlobTree[index].slot[j].valid = true;
        allocated++;
        if (level >= TOP_CACHE_VAR && SIM_ENABLE_VAR)
        {
          // bool nvm_access = is_nvm_addr(mem_addr);
          bool nvm_access = in_nvm(level);
          bool beginning = false;
          // bool ending = (j == LZ_VAR[i]-1);
          // bool last_read = (j == LZ_VAR[i]-1);
          bool ending = false;
          bool last_read = false;
          insert_oramQ(mem_addr, orig_cycle, orig_thread, orig_instr, orig_pc, 'W', last_read, nvm_access, op_type, beginning, ending, level);
        }

        if (candidate[real] != -1 && real < RING_Z) // GlobTree[index].dumnum > GlobTree[index].s)
        {
          // if (tracectr >= 39796059)
          // {
          //   print_candidate();
          //   printf("%c %lld real=%d: cand[%d]: %d\n", op_type, wbuck, real, candidate[real], Stash[candidate[real]].addr);
          // }
          GlobTree[index].slot[j].addr = Stash[candidate[real]].addr;
          GlobTree[index].slot[j].label = Stash[candidate[real]].label;
          GlobTree[index].slot[j].isReal = true;
          GlobTree[index].slot[j].isData = true;
          GlobTree[index].dumnum--;
          GlobTree[index].dumval--;
          real++;

          remove_from_stash(candidate[real-1]);
        }
        else
        {
          // GlobTree[index].s++;
        }

      }
      else
      {
        break;
      }
    }
    for (int j = LZ_VAR[level]; j < LZ_VAR[level] + S_INC; j++)
    {
      extendctr++;
      int mem_addr = calc_mem_addr(index, j, 'W');
      if (mem_addr != -1)
      {
        s_overctr++;
        GlobTree[index].slot[j].valid = true;
        allocated++;
        if (level >= TOP_CACHE_VAR && SIM_ENABLE_VAR)
        {
          // bool nvm_access = is_nvm_addr(mem_addr);
          bool nvm_access = in_nvm(level);
          bool beginning = false;
          // bool ending = (j == LZ_VAR[i]-1);
          // bool last_read = (j == LZ_VAR[i]-1);
          bool ending = false;
          bool last_read = false;
          insert_oramQ(mem_addr, orig_cycle, orig_thread, orig_instr, orig_pc, 'W', last_read, nvm_access, op_type, beginning, ending, level);
        }
        // GlobTree[index].s++;
      }
      else
      {
        break;
      }
    }
  }

  GlobTree[index].count = 0;
  deadctr -= GlobTree[index].dumdead;
  deadctr_arr[level] -= GlobTree[index].dumdead;

  if (level < NVM_START && level >= TOP_CACHE_VAR)
  {
    dead_dram -= GlobTree[index].dumdead;
  }
  GlobTree[index].dumdead = 0;
  // wb[level] += stashb4 - stashctr;

  GlobTree[index].s = allocated - RING_Z;

  int curS = GlobTree[index].s;
  
  if (curS > 0 && curS <= RING_S )
  {
    if (level >= GATHER_START)
    {
      allocS_dist[curS]++;
      if (curS < LS[level])
      {
        s_under[level]++;
      }
    }
  }
  else
  {
    printf("ERROR: write bucket @ L%d cur S %d out of range!\n", level, curS);
    export_csv(pargv);
    exit(1);
  }
  
  if (level == LEVEL - 1 && DEAD_ENABLE_VAR && tracectr > 62000000)
  {
    // printf("%d      shuf: %lld  dead encounter: %d\n", deadQ_arr[level]->size, shuff[LEVEL-1], dead_encountered[LEVEL-1]);
  }

  if (level >= GATHER_START && DEAD_ENABLE_VAR) // && op_type == 'e')
  {
    // while (deadQ_arr[level]->size > 0)
    // {
    //   Element * ds = Dequeue(deadQ_arr[level]);
    //   GlobTree[ds->index].slot[ds->offset].dd = DEAD;
    //   GlobTree[ds->index].allctr--;
    // }
    
    while (deadQ_arr[level]->size < deadQ_arr[level]->limit && deadQ_shadow[level]->size > 0)
    {
      Element * ds = Dequeue(deadQ_shadow[level]);
      if (GlobTree[ds->index].slot[ds->offset].dd == DEAD)
      {
        shad_added[level]++;
        GlobTree[ds->index].slot[ds->offset].dd = REMEMBERED;
        GlobTree[ds->index].allctr++;
        Enqueue(deadQ_arr[level], ds);
      }
      else
      {
        free(ds);
      }
    }

  }
  
}


void read_bucket(int index, int i, char op_type){
    int reqmade = 0;
    int dum_cand[Z] = {0};
    int cand_ind = 0;
    // print_oram_stats();
      // printf("trace %d\n", tracectr);
      // printf("i %d\n", i);
      // printf("index %d\n", index);
      // printf("heloooooo\n");
      // printf("flush ctr %d\n", stale_flush_ctr);
    
    GlobTree[index].dumval = Z;

    int valnum = GlobTree[index].dumval;
    dumval_dist[valnum]++;
    dumval_range_dist[calc_range(i)][valnum]++;
    if (op_type == 'r')
    {
      shuff[i]++;
      shuff_total++;
      shuff_interval[i]++;
      // shufcount++;
      GlobTree[index].reshuffled++;
    }

    int slotCount = DYNAMIC_S ? Z : LZ_VAR[i];  

    if (slotCount < 0 || slotCount > Z)
    {
      printf("ERROR: reshuffle slot count %d out of range!\n", slotCount);
      exit(1);
    }
    
    for (int j = 0; j < slotCount; j++)
    {

      if (GlobTree[index].slot[j].isReal)
      {
        // if (op_type == 'e')
        // {
        //   GlobTree[index].slot[j].valid = false;
        // }
        
        reqmade++;
        if (i >= TOP_CACHE_VAR && SIM_ENABLE_VAR)
        {
          int mem_addr = calc_mem_addr(index, j, 'R');
          // bool nvm_access = is_nvm_addr(mem_addr);
              // printf("reshuffle mem addr: %d   @ L%d  j: %d \n", mem_addr, i, j);

          bool nvm_access = in_nvm(i);
          bool beginning = (reqmade == 1);
          bool ending = false;
          bool last_read = (reqmade == RING_Z);
          insert_oramQ(mem_addr, orig_cycle, orig_thread, orig_instr, orig_pc, 'R', last_read, nvm_access, op_type, beginning, ending, i);
        }
      }
      
      if (GlobTree[index].slot[j].isReal)
      {
        // printf("j: %d real\n", j);
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
          // GlobTree[index].dumval++;
        }
        else
        {
          printf("ERROR: ring read: trace %d stash overflow!  @ %d\n", tracectr, stashctr);
          export_csv(pargv);
          print_oram_stats();
          exit(1);
        }
      }
      else if(GlobTree[index].slot[j].valid)
      {
        // printf("\ncand ind %d\n", cand_ind);
        dum_cand[cand_ind] = j;
        // printf("valid filling dum_cand[%d] %d\n", cand_ind, dum_cand[cand_ind]);
        cand_ind++;
      }
    }

      int reqcont = reqmade;
      for (int k = 0; k < RING_Z-reqmade-GREEN_BLOCK; k++)
      {
        int ri = -1;
        int sd = -1;
        while (sd == -1)
        {
          // printf("@%d in reshuffle\n", tracectr);

          ri = rand() % cand_ind;
          sd = dum_cand[ri];
        }
        reqcont++;
          
        // if (op_type == 'e')
        // {
        //   GlobTree[index].slot[sd].valid = false;
        // }
        if (i >= TOP_CACHE_VAR && SIM_ENABLE_VAR)
        {
              // printf("reshuffle mem addr: %d   @ L%d  j: %d \n", mem_addr, i, sd);
          int mem_addr = calc_mem_addr(index, sd, 'R');

            // bool nvm_access = is_nvm_addr(mem_addr);
            bool nvm_access = in_nvm(i);
            bool beginning = (reqcont == 1);
            bool ending = false;
            bool last_read = (reqcont == RING_Z);
            insert_oramQ(mem_addr, orig_cycle, orig_thread, orig_instr, orig_pc, 'R', last_read, nvm_access, op_type, beginning, ending, i);
            // printf("%d: slot %d accessed ~> dummy? %s\n", k, sd, GlobTree[index].slot[sd].isReal?"no":"yes");
        }
        dum_cand[ri] = -1;
      }
      
      if (op_type == 'e')
      {
        for (int j = 0; j < slotCount; j++)
        {
          if (RING_ENABLE && GlobTree[index].slot[j].valid)
          {
            calc_mem_addr(index, j, 'R');
            GlobTree[index].slot[j].valid = false;

          }
        }
      }
      
     

    // if (i == LEVEL - 1 && DEAD_ENABLE_VAR && tracectr > 62000000)
    // {
    //   // printf("@%d R> %d, ", ringctr, deadQ_arr[i]->size);
    // }
    


    // if (RING_ENABLE && DEAD_ENABLE_VAR && op_type == 'r') //DEAD_ENABLE && tracectr >= (DD_SATURATE - 1000000))
    // {
    //   gather_dead(index, i);
    // }

    // if (i == LEVEL - 1 && DEAD_ENABLE_VAR && tracectr > 62000000)
    // {
    //   // printf("%d      shuf: %lld  dead encounter: %d\n", deadQ_arr[i]->size, shuff[LEVEL-1], dead_encountered[LEVEL-1]);
    // }


}


void ring_early_reshuffle(int label){
  // printf("reshuffle trace %d\n", tracectr);
  // int shufcount = 0;
  // int stashb4 = stashctr;
  // for (int i = 0; i < LEVEL; i++)
  for (int i = LEVEL-1; i > 0; i--)
  {
    int index = calc_index(label, i);
    // int reqmade = 0;
    // int dum_cand[Z] = {0};
    // int cand_ind = 0;
    // print_oram_stats();
      // printf("trace %d\n", tracectr);
      // printf("i %d\n", i);
      // printf("index %d\n", index);
      // printf("heloooooo\n");
      // printf("flush ctr %d\n", stale_flush_ctr);

    if (i >= LEVEL-2)
    {
      int sind = (i == LEVEL-2) ? calc_super(label, i) : calc_super(label, i-1);
      if (SuperNode[sind].count >= 3*RING_S - 1)
      {
        SuperNode[sind].count = 0;
        supshuf_total++;
      }
      int hind = calc_horiz(index, label, i);
      if (i == LEVEL-2)
      {
        if (SuperHoriz1[hind].count >= SUP_HORIZ_MAX)
        {
          SuperHoriz1[hind].count = 0;
          supshuf_horiz1++;
        }
      }
      else
      {
        if (SuperHoriz2[hind].count >= SUP_HORIZ_MAX)
        {
          SuperHoriz2[hind].count = 0;
          supshuf_horiz2++;
        }
      }
    }  

    int curS = calc_ring_s(index, i); 
    if (GlobTree[index].count >= curS + GREEN_BLOCK)    // || i < TOP_CACHE  || i >= LEVEL-2 
    {
      if (curS > 0 && curS <= RING_S )
      {
        if (i >= GATHER_START)
        {
          s_dist[curS]++;
        }
        
      }
      else
      {
        printf("ERROR: early reshuffle cur S %d our of range!\n", curS);
        exit(1);
      }

      read_bucket(index, i, 'r');
      // write phase: 
      write_bucket(index, label, i, 'r');
      if (SUPER_ENABLE && is_super_level(i))
      {
        write_bucket(calc_super_in_tree(index), label, i, 'r');
      }
      

     
      if (i >= LEVEL-2)
      {
        int sind = (i == LEVEL-2) ? calc_super(label, i) : calc_super(label, i-1);
        if (SuperNode[sind].count < 3*RING_S  && SuperNode[sind].count >= 0)
        {
          int cid = SuperNode[sind].count;
          supcount_dist[cid]++;
        }
        else
        {
          printf("ERROR: reshuffle sup count %d beyond range! \n", SuperNode[sind].count);
          exit(1);
        }

        int hind = calc_horiz(index, label, i);
        if (i == LEVEL-2)
        {
          if (SuperHoriz1[hind].count <= SUP_HORIZ_MAX && SuperHoriz1[hind].count >= 0)
          {
            suphoriz1_dist[(int)SuperHoriz1[hind].count]++;
          }
          else
          {
            printf("ERROR: reshuffle sup horiz1 count %d beyond range! \n", SuperHoriz1[hind].count );
            exit(1);
          }
        }
        else
        {
          if (SuperHoriz2[hind].count <= SUP_HORIZ_MAX && SuperHoriz2[hind].count >= 0)
          {
            suphoriz2_dist[(int)SuperHoriz2[hind].count]++;
          }
          else
          {
            printf("ERROR: reshuffle sup horiz2 count %d beyond range! \n", SuperHoriz1[hind].count);
            exit(1);
          }
        }
        
      } 


    }

    if (i == LEVEL - 1 && DEAD_ENABLE_VAR && tracectr > 62000000)
    {
      // printf("@%d R> %d, ", ringctr, deadQ_arr[i]->size);
    }
    


    if (RING_ENABLE && DEAD_ENABLE_VAR) //DEAD_ENABLE && tracectr >= (DD_SATURATE - 1000000))
    {
      gather_dead(index, i);
    }

    if (i == LEVEL - 1 && DEAD_ENABLE_VAR && tracectr > 62000000)
    {
      // printf("%d      shuf: %lld  dead encounter: %d\n", deadQ_arr[i]->size, shuff[LEVEL-1], dead_encountered[LEVEL-1]);
    }

  }

  // for (int i = LEVEL-1; i > 0; i--)
  // {
  //   int index = calc_index(label, i);
  //   if (GlobTree[index].count >= LS[i] /* || i < TOP_CACHE  || i >= LEVEL-2 */)
  //   {
  //     reset_candidate();
  //     pick_candidate(index, label, i);

  //     for (int j = 0; j < LZ_VAR[i]; j++)
  //     {
  //       GlobTree[index].slot[j].valid = true;
  //       if (i >= TOP_CACHE_VAR && SIM_ENABLE_VAR)
  //       {
  //         int mem_addr = index*Z_VAR + j;
  //         insert_oramQ(mem_addr, orig_cycle, orig_thread, orig_instr, orig_pc, 'W', false);
  //       }
  //       if (candidate[j] != -1 && GlobTree[index].dumnum > LS[i])
  //       {
  //         // printf("cand[%d]: %d\n", candidate[j], Stash[candidate[j]].addr);
  //         GlobTree[index].slot[j].addr = Stash[candidate[j]].addr;
  //         GlobTree[index].slot[j].label = Stash[candidate[j]].label;
  //         GlobTree[index].slot[j].isReal = true;
  //         GlobTree[index].slot[j].isData = true;
  //         GlobTree[index].dumnum--;


  //         remove_from_stash(candidate[j]);
  //       }
  //     }
      
  //     GlobTree[index].count = 0;
  //   }
  // }
  
  // wbshuff += stashb4 - stashctr;
  // shuff_dist[shufcount % LEVEL]++;

  

}



void ring_invalidate(int index, int offset){
   if (!GlobTree[index].slot[offset].valid)
  {
    printf("invalidate an invalid block @ %d\n", tracectr);
    exit(1);
  }
  GlobTree[index].slot[offset].valid = false;
  if (GlobTree[index].slot[offset].dead_start == 0)
  {
    GlobTree[index].slot[offset].dead_start = ringctr;
  }
  
}



void print_shuff_stat(){
  printf("\nreshuffle count of each level \n");
  int sum = 0;
  for (int i = 0; i < LEVEL; i++)
  {
    printf("%lld\n", shuff[i]);
    sum += shuff[i];
  }
  printf("\n%d\n", sum);
}

void print_ref_close_stat(){
  printf("\nrefresh close to threshold count of each level \n");
  int sum = 0;
  for (int i = 0; i < LEVEL; i++)
  {
    printf("%d\n", ref_close[i]);
    sum += ref_close[i];
  }
  printf("\n%d\n", sum);
}


void print_wb_stat(){
  printf("\nwb during reshuffle of each level \n");
  int sum = 0;
  for (int i = 0; i < LEVEL; i++)
  {
    printf("%d\n", wb[i]);
    sum += wb[i];
  }
  printf("\n%d\n", sum);
}

void print_shuff_dist(){
  printf("\ndistribution of each reshuffle count \n");
  int sum = 0;
  for (int i = 0; i < LEVEL; i++)
  {
    printf("%d\n", shuff_dist[i]);
    sum += shuff_dist[i];
  }
  printf("\n%d\n", sum);
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

void print_lifetime_stat(FILE *fp){
  for (int i = 0; i < LEVEL; i++)
  {
    fprintf(fp, "lifetime_min[%d],%d\n", i, lifetime_min[i]);
  }
  for (int i = 0; i < LEVEL; i++)
  {
    fprintf(fp, "lifetime_max[%d],%d\n", i, lifetime_max[i]);
  }
  for (int i = 0; i < LEVEL; i++)
  {
    fprintf(fp, "lifetime_avg[%d],%f\n", i, (double)lifetime_sum[i]/lifetime_count[i]);
  }
}

void print_count_stat(FILE *fp){
  for (int i = 0; i < LEVEL; i++)
  {
    fprintf(fp, "count_min[%d],%d\n", i, count_min[i]);
  }
  for (int i = 0; i < LEVEL; i++)
  {
    fprintf(fp, "count_max[%d],%d\n", i, count_max[i]);
  }
  for (int i = 0; i < LEVEL; i++)
  {
    fprintf(fp, "count_avg[%d],%f\n", i, (double)count_sum[i]/count_count[i]);
  }
}


void export_intermed(char exp_name[], unsigned long long int ind, long double *arr, unsigned long long int suffix){
  FILE *fp;
  // char *filename = (char *)malloc(sizeof(char)*100);
  // filename = "";
  char filename[100];

  if (chdir("log") != 0)  
  {
    perror("chdir() to log failed"); 
  }

  // char str[20];
  // sprintf(str, "%d", ind);
  // strcpy( filename, str );

  // // printf("filename "": %s\n", filename);
  // filename = strcat(exp_name, "-");
  // // printf("filename argv: %s\n", filename);
  // filename = strcat(filename, str);
  // // printf("filename str: %s\n", filename);
  // filename = strcat(filename, ".csv");
  // // printf("filename csv: %s\n", filename);

  sprintf(filename, "%s%lld-%lld.csv", exp_name, suffix, ind);
  // printf("filename: %s\n", filename);


  fp = fopen(filename,"w+");
  
  fprintf(fp,"series,%lld\n", ind);
  // print_array(arr, LEVEL, fp);
  print_array_double(arr, LEVEL, fp);

  // free(filename);
  
  fclose(fp);


}

void reset_profile_counters(){
  invokectr = 0;
  oramctr = 0;
  dummyctr = 0;
  pos1_access = 0;
  pos2_access = 0;
  plb_hit[0] = 0; 
  plb_hit[1] = 0; 
  plb_hit[2] = 0; 
  plbaccess[0] = 0;
  plbaccess[1] = 0;
  plbaccess[2] = 0;
  bkctr = 0;
  // hitctr = 0;
  // missctr = 0;
  evictctr = 0;
  rho_hit = 0;
  rhoctr = 0;
  rho_dummyctr = 0;
  rho_bkctr = 0;
  earlyctr = 0;
  dirty_pointctr = 0;
  cache_dirty = 0;
  ptr_fail = 0;
  search_fail = 0;
  pinctr = 0;
  unpinctr = 0;
  precase = 0;
  sttctr = 0;
  stash_leftover = 0;
  stash_removed = 0;
  fillhit = 0;
  fillmiss = 0;
  topctr = 0;
  midctr = 0;
  botctr = 0;
  ring_evictctr = 0;
  // stashctr = 0;
  stash_cont = 0;
  linger_discard = 0;
  ringctr = 0;
  wbctr = 0;
  writectr = 0;
  wskip = 0;
  mem_req_latencies = 0;
  // nonmemops_sum = 0;
  missl1wb = 0;
  wbshuff = 0;
  ringdumctr = 0;
  wl_pos[1] = 0;
  wl_pos[2] = 0;
  stalectr = 0;
  stale_flush_ctr = 0;
  stale_discard_ctr = 0;
  stale_reduction = 0;
  // deadctr = 0;
  // dead_on_path = 0;
  // dead_on_path_dram = 0;
  dram_norm_r = 0;
  nvm_norm_r = 0;
  dram_norm_w = 0;
  nvm_norm_w = 0;
  dram_inplace_r = 0;
  dram_remote_r = 0;
  nvm_inplace_r = 0;
  nvm_remote_r = 0;
  dram_inplace_w = 0;
  dram_remote_w = 0;
  nvm_inplace_w = 0;
  nvm_remote_w = 0;
  // remote_nvms = 0;
  dram_elselevel = 0;
  nvm_elselevel = 0;
  // surplus_dead = 0;
  surplus_in_use = 0;
  // rmiss = 0;
  // wmiss = 0;
  deadrem = 0;
  // nonmemops_executed = 0;
  // dead_dram = 0;
  for (int i = 0; i < LEVEL; i++)
  {
    shuff[i] = 0;
  }
  for (int i = 0; i < RING_S+1; i++)
  {
    s_dist[i] = 0;
  }
  for (int i = 0; i < RING_S+1; i++)
  {
    allocS_dist[i] = 0;
  }
  for (int i = 0; i < LEVEL; i++)
  {
    s_under[i] = 0;
  }

  s_underctr = 0;
  s_overctr = 0;
  s_inctr = 0;
  takenctr = 0;
  extendctr = 0;
  inplacectr = 0;
  stash_hit = 0;

}


void export_csv(char * argv[]){


  FILE *fp;
  char *filename;
  // if (chdir("log") != 0)  
  // {
  //   perror("chdir() to log failed"); 
  // }

  if (chdir("../oram/log") != 0)  
  {
    perror("chdir() to ../oram/log failed"); 
  }


  filename = "";
  filename = strcat(argv[3], "-");
  filename = strcat(filename, bench);
  filename = strcat(filename, ".csv");
  
  // printf("file: %s\n", filename);
  // sprintf(filename,"%s-%s.csv",argv[3], bench);

  fp = fopen(filename,"w+");

  int shuffctr = 0; 
  int shuffctr_tc = 0; 
  int shuffctr_dram = 0; 
  int shuffctr_nvm = 0; 
  for (int i = 0; i < LEVEL; i++)
  {
    shuffctr += shuff[i];
    if (i >= TOP_CACHE)
    {
      shuffctr_tc += shuff[i];
      if (i < NVM_START)
      {
        shuffctr_dram += shuff[i];
      }
      else
      {
        shuffctr_nvm += shuff[i];
      }
      
    }
  }

  fprintf(fp,"Benchmark,%s\n", bench);
  // fprintf(fp,"Experiment,%s\n", exp_name);
  fprintf(fp,"exe_time,%f\n", exe_time);
  fprintf(fp,"CYCLE_VAL,%lld\n", CYCLE_VAL);
  fprintf(fp,"tracectr,%d\n", tracectr);
  // fprintf(fp, "mem_clk,%lld\n", mem_clk);
  fprintf(fp, "invokectr,%d\n", invokectr);
  fprintf(fp, "oramctr,%d\n", oramctr);
  fprintf(fp, "dummyctr,%d\n", dummyctr);
  fprintf(fp, "pos1_access,%d\n", pos1_access);
  fprintf(fp, "pos2_access,%d\n", pos2_access);
  // fprintf(fp, "plb_hit0,%f%%\n", 100*(double)plb_hit[0]/plbaccess[0]);
  // fprintf(fp, "plb_hit1,%f%%\n", 100*(double)plb_hit[1]/plbaccess[1]);
  // fprintf(fp, "plb_hit2,%f%%\n", 100*(double)plb_hit[2]/plbaccess[2]);
  // fprintf(fp, "plb_hit0,%lld\n", plb_hit[0]);
  // fprintf(fp, "plb_hit1,%lld\n", plb_hit[1]);
  // fprintf(fp, "plb_hit2,%lld\n", plb_hit[2]);
  // fprintf(fp, "plbaccess0,%lld\n", plbaccess[0]);
  // fprintf(fp, "plbaccess1,%lld\n", plbaccess[1]);
  // fprintf(fp, "plbaccess2,%lld\n", plbaccess[2]);
  // fprintf(fp, "oramQ_size,%d\n", oramQ->size);
  // fprintf(fp, "Bk_Evict,%f%%\n", 100*(double)bkctr/(oramctr+bkctr));
  fprintf(fp, "Bk_Evict,%d\n", bkctr);
  fprintf(fp, "Cache_Hit,%f%%\n", 100*(double)hitctr/(hitctr+missctr));
  fprintf(fp, "Cache Evict,%f%%\n", 100*(double)evictctr/(missctr));
  fprintf(fp, "hitctr,%d\n", hitctr);
  fprintf(fp, "missctr,%d\n", missctr);
  // fprintf(fp, "rho_hit,%f%%\n", 100*(double)rho_hit/(invokectr));
  // fprintf(fp, "rhoctr,%d\n", rhoctr);
  // fprintf(fp, "rho_dummyctr,%d\n", rho_dummyctr);
  // fprintf(fp, "rho_bkctr,%f%%\n", 100*(double)rho_bkctr/rho_hit);
  fprintf(fp, "earlyctr,%d\n", earlyctr);
  // fprintf(fp, "dirty_pointctr,%d\n", dirty_pointctr);
  // fprintf(fp, "cache_dirty,%d\n", cache_dirty);
  // fprintf(fp, "ptr_fail,%d\n", ptr_fail);
  // fprintf(fp, "search_fail,%d\n", search_fail);
  // fprintf(fp, "pinctr,%d\n", pinctr);
  // fprintf(fp, "unpinctr,%d\n", unpinctr);
  // fprintf(fp, "precase,%d\n", precase);
  // fprintf(fp, "sttctr,%d\n", sttctr);
  // fprintf(fp, "stash_leftover,%d\n", stash_leftover);
  // fprintf(fp, "stash_removed,%d\n", stash_removed);
  // fprintf(fp, "fillhit,%d\n", fillhit);
  // fprintf(fp, "fillmiss,%d\n", fillmiss);
  fprintf(fp, "topctr,%f%%\n", 100*(double)topctr/(topctr+midctr+botctr));
  fprintf(fp, "midctr,%f%%\n", 100*(double)midctr/(topctr+midctr+botctr));
  fprintf(fp, "botctr,%f%%\n", 100*(double)botctr/(topctr+midctr+botctr));
  fprintf(fp, "stashctr,%d\n", stashctr);
  // fprintf(fp, "stash_cont,%d\n", stash_cont);
  // fprintf(fp, "linger_discard,%d\n", linger_discard);
  fprintf(fp, "ringctr,%d\n", ringctr);
  fprintf(fp, "ring_evictctr,%d\n", ring_evictctr);
  fprintf(fp, "shuffctr,%d\n", shuffctr);
  fprintf(fp, "shuff_tc+,%d\n", shuffctr_tc);
  fprintf(fp, "shuffctr_dram,%d\n", shuffctr_dram);
  fprintf(fp, "shuffctr_nvm,%d\n", shuffctr_nvm);

  // fprintf(fp, "wbctr,%d\n", wbctr);
  // fprintf(fp, "writectr,%d\n", writectr);
  // fprintf(fp, "wskip,%d\n", wskip);
  // fprintf(fp, "mem_req_late,%f\n", (double)mem_req_latencies/(invokectr));
  fprintf(fp, "nonmemops_sum,%lld\n", nonmemops_sum);
  // fprintf(fp, "missl1wb,%lld\n", missl1wb);
  // fprintf(fp, "missl1wb_rate,%f%%\n", 100*(double)missl1wb/missctr);
  // fprintf(fp, "wbshuff,%d\n", wbshuff);
  // fprintf(fp, "ringdumctr,%d\n", ringdumctr);
  // fprintf(fp, "wl_pos1,%d\n", wl_pos[1]);
  // fprintf(fp, "wl_pos2,%d\n", wl_pos[2]);
  // fprintf(fp, "stalectr,%d\n", stalectr);
  // fprintf(fp, "stale_flush_ctr,%d\n", stale_flush_ctr);
  // fprintf(fp, "stale_discard_ctr,%d\n", stale_discard_ctr);
  // fprintf(fp, "stale_reduction,%d\n", stale_reduction);
  // for (int i = 0; i < GL_COUNT; i++)
  // {
  //   fprintf(fp, "glctr[%d],%f%%\n", i, 100*(double)glctr[i]/(pow(2,GL[i])*GL_CAP[i]));
  // }
  // fprintf(fp, "STALE_BUF,%d\n", STALE_BUF_SIZE);
  // fprintf(fp, "STALE_CAP,%d\n", STALE_CAP);
  fprintf(fp, "deadctr,%lld\n", deadctr);
  // for (int i = 0; i < 31; i++)
  // {
  //   fprintf(fp, "%dm,%lld\n", i*10, deadarr[i]);
  // }
  if (RING_ENABLE)
  {
    // fprintf(fp, "dead_on_path,%d\n", (int)dead_on_path/ring_evictctr);
    // fprintf(fp, "dead_on_path_dram,%d\n", (int)dead_on_path_dram/ring_evictctr);
  }
  
  // for (int i = 0; i < 31; i++)
  // {
  //   fprintf(fp, "%dm,%lld\n", i*10, dead_on_path_arr[i]);
  // }
  // for (int i = 0; i < Z; i++)
  // {
  //   fprintf(fp, "dumval[%d],%d\n", i,dumval_dist[i]);
  // }
  // for (int i = 0; i < 3; i++)
  // {
  //   for (int j = 0; j < Z; j++)
  //   {
  //     fprintf(fp, "dumval[%d][%d],%d\n", i, j, dumval_range_dist[i][j]);
  //   }
  // }
  fprintf(fp, "dram_norm_r,%lld\n", dram_norm_r);
  fprintf(fp, "nvm_norm_r,%lld\n", nvm_norm_r);
  fprintf(fp, "dram_norm_w,%lld\n", dram_norm_w);
  fprintf(fp, "nvm_norm_w,%lld\n", nvm_norm_w);
  fprintf(fp, "dram_inplace_r,%lld\n", dram_inplace_r);
  fprintf(fp, "dram_remote_r,%lld\n", dram_remote_r);
  fprintf(fp, "nvm_inplace_r,%lld\n", nvm_inplace_r);
  fprintf(fp, "nvm_remote_r,%lld\n", nvm_remote_r);
  fprintf(fp, "dram_inplace_w,%lld\n", dram_inplace_w);
  fprintf(fp, "dram_remote_w,%lld\n", dram_remote_w);
  fprintf(fp, "nvm_inplace_w,%lld\n", nvm_inplace_w);
  fprintf(fp, "nvm_remote_w,%lld\n", nvm_remote_w);
  // fprintf(fp, "dram_inplace_w_remembered,%lld\n", dram_inplace_w_remembered);
  // fprintf(fp, "deadQ-size,%d\n", deadQ->size);
  fprintf(fp, "remote_drams,%lld\n", dram_remote_w - dram_remote_r);
  fprintf(fp, "remote_nvms,%d\n", remote_nvms);

  // fprintf(fp, "dram_elselevel,%lld\n", dram_elselevel);
  // fprintf(fp, "nvm_elselevel,%lld\n", nvm_elselevel);
  fprintf(fp, "surplus_dead,%lld\n", surplus_dead);
  fprintf(fp, "surplus_in_use,%lld\n", surplus_in_use);
  fprintf(fp, "rmiss,%d\n", rmiss);
  fprintf(fp, "wmiss,%d\n", wmiss);
  // fprintf(fp, "deadrem,%lld\n", deadrem);


  // for (int i = 0; i < LEVEL; i++)
  // {
  //   fprintf(fp, "shuff[%d],%lld\n", i, shuff[i]);
  // }

  fprintf(fp, "nonmemops_executed,%lld\n", nonmemops_executed);
  fprintf(fp, "dead_dram,%lld\n", dead_dram);
  // print_lifetime_stat(fp);
  
  // print_count_stat(fp);
  // long long int all_r = 0;
  // long long int all_w = 0;

  // long long int dram_r = 0;
  // long long int dram_w = 0;

  // for (int c = 0; c < NUM_CHANNELS; c++)
  // {
  //   all_r += stats_reads_completed[c];
  //   all_w += stats_writes_completed[c];
  //   if (c < NUM_CHANNELS - NVM_CHANNEL)
  //   {
  //     dram_r += stats_reads_completed[c];
  //     dram_w += stats_writes_completed[c];
  //   }
    
  //   fprintf(fp, "Reads_chan[%d],%-7lld\n", c, stats_reads_completed[c]);
  //   fprintf(fp, "Writes_chan[%d],%-7lld\n", c, stats_writes_completed[c]);
  //   fprintf(fp, "R_Latency_chan[%d],%7.5f\n", c, (double) stats_average_read_latency[c]);
  //   fprintf(fp, "R_Q_Latency_chan[%d],%7.5f\n", c, (double) stats_average_read_queue_latency[c]);
  //   fprintf(fp, "W_Latency_chan[%d],%7.5f\n", c, (double) stats_average_write_latency[c]);
  //   fprintf(fp, "W_Q_Latency_chan[%d],%7.5f\n", c, (double) stats_average_write_queue_latency[c]);
  // }

  // fprintf(fp, "R_NVM/DRAM_T,%f\n", (double)stats_average_read_latency[NUM_CHANNELS-1]/stats_average_read_latency[NUM_CHANNELS-2]);
  // fprintf(fp, "W_NVM/DRAM_T,%f\n", (double)stats_average_write_latency[NUM_CHANNELS-1]/stats_average_write_latency[NUM_CHANNELS-2]);
  
  // fprintf(fp, "All_R,%lld\n", all_r);
  // fprintf(fp, "All_W,%lld\n", all_w);

  // fprintf(fp, "dram_R,%lld\n", dram_r);
  // fprintf(fp, "dram_W,%lld\n", dram_w);

  // fprintf(fp, "DRAM/NVM_R#,%f\n", (double)dram_r/(all_r - dram_r));
  // fprintf(fp, "DRAM/NVM_W#,%f\n", (double)dram_w/(all_w - dram_w));

  // fprintf(fp, "online_wait_dram,%lld\n", online_wait_dram);
  // fprintf(fp, "online_wait_nvm,%lld\n", online_wait_nvm);
  // fprintf(fp, "evict_wait_dram,%lld\n", evict_wait_dram);
  // fprintf(fp, "evict_wait_nvm,%lld\n", evict_wait_nvm);
  // fprintf(fp, "reshuffle_wait_dram,%lld\n", reshuffle_wait_dram);
  // fprintf(fp, "reshuffle_wait_nvm,%lld\n", reshuffle_wait_nvm);
  // fprintf(fp, "meta_wait_dram,%lld\n", meta_wait_dram);

  // int dramchan = NUM_CHANNELS-NVM_CHANNEL;
  // int dramchan = NUM_CHANNELS;
  // int draml = NVM_START - TOP_CACHE;
  // int nvml = LEVEL - NVM_START;

  // fprintf(fp, "online_wait_dram_perAcc,%f\n", (double)online_wait_dram/((ringctr*draml)/NUM_CHANNELS));
  // fprintf(fp, "online_wait_nvm_perAcc,%f\n", (double)online_wait_nvm/((ringctr*nvml)/ceil((double)nvml/NUM_CHANNELS)));
  // fprintf(fp, "evict_wait_dram_perAcc,%f\n", (double)evict_wait_dram/((ring_evictctr*draml*(2*RING_Z+RING_S))/NUM_CHANNELS));
  // fprintf(fp, "evict_wait_nvm_perAcc,%f\n", (double)evict_wait_nvm/((ring_evictctr*nvml*(2*RING_Z+RING_S))/NUM_CHANNELS));
  // fprintf(fp, "reshuffle_wait_dram_perAcc,%f\n", (double)reshuffle_wait_dram/((shuffctr_dram*1*(2*RING_Z+RING_S))/NUM_CHANNELS));
  // fprintf(fp, "reshuffle_wait_nvm_perAcc,%f\n", (double)reshuffle_wait_nvm/((shuffctr_nvm*1*(2*RING_Z+RING_S))/NUM_CHANNELS));
  // fprintf(fp, "meta_wait_dram_perAcc,%f\n", (double)meta_wait_dram/(((ring_evictctr+ringctr)*(nvml+draml)*2)/NUM_CHANNELS));

  // fprintf(fp, "odram,%d\n", odram);
  // fprintf(fp, "onvm,%d\n", onvm);
  // fprintf(fp, "envm,%d\n", envm);
  // fprintf(fp, "edram,%d\n", edram);
  // fprintf(fp, "rnvm,%d\n", rnvm);
  // fprintf(fp, "rdram,%d\n", rdram);
  // fprintf(fp, "mdram,%d\n", mdram);
  // fprintf(fp, "lrs_ctr,%d\n", lrs_ctr);
  fprintf(fp, "deadQs,%d\n", calc_deadQ_size());
  // fprintf(fp, "rmpki,%f\n", (double)rmiss/(nonmemops_sum/1000));
  // fprintf(fp, "wmpki,%f\n", (double)wmiss/(nonmemops_sum/1000));
  // fprintf(fp, "supshuf_total,%d\n", supshuf_total);
  // fprintf(fp, "supshuf_horiz1,%d\n", supshuf_horiz1);
  // fprintf(fp, "supshuf_horiz2,%d\n", supshuf_horiz2);
  // fprintf(fp, "supep_horiz1,%d\n", supep_horiz1);
  // fprintf(fp, "supep_horiz2,%d\n", supep_horiz2);

  // fprintf(fp, "online_r,%d\n", online_r);
  // fprintf(fp, "evict_r,%d\n", evict_r);
  // fprintf(fp, "reshuffle_r,%d\n", reshuffle_r);
  // fprintf(fp, "evict_w,%d\n", evict_w);
  // fprintf(fp, "reshuffle_w,%d\n", reshuffle_w);
  // fprintf(fp, "w_ended,%d\n", w_ended);
  // fprintf(fp, "r_ended,%d\n", r_ended);
  // fprintf(fp, "r_ended_o,%d\n", r_ended_o);
  // fprintf(fp, "w_ended_dram,%d\n", w_ended_dram);
  // fprintf(fp, "w_ended_nvm,%d\n", w_ended_nvm);
  // fprintf(fp, "meta_ended,%d\n", meta_ended);

   for (int i = 0; i < LEVEL; i++)
  {
    fprintf(fp, "shuff[%d],%lld\n", i, shuff[i]);
  }

  for (int i = 0; i < LEVEL; i++)
  {
    fprintf(fp, "realcount[%d],%lld\n", i, realcount[i]);
  }

   for (int i = 0; i < LEVEL; i++)
  {
    fprintf(fp, "dumcount[%d],%lld\n", i, dumcount[i]);
  }


  //  for (int i = 0; i < 3*RING_S; i++)
  // {
  //   fprintf(fp, "supcount_dist[%d],%d\n", i, supcount_dist[i]);
  // }
  //  for (int i = 0; i < SUP_HORIZ_MAX+1; i++)
  // {
  //   fprintf(fp, "suphoriz1_dist[%d],%d\n", i, suphoriz1_dist[i]);
  // }
  //  for (int i = 0; i < SUP_HORIZ_MAX+1; i++)
  // {
  //   fprintf(fp, "suphoriz2_dist[%d],%d\n", i, suphoriz2_dist[i]);
  // }

  // for (int i = 0; i < SUP_HORIZ_MAX+1; i++)
  // {
  //   fprintf(fp, "ephoriz1_dist[%d],%d\n", i, ephoriz1_dist[i]);
  // }
  // for (int i = 0; i < SUP_HORIZ_MAX+1; i++)
  // {
  //   fprintf(fp, "ephoriz2_dist[%d],%d\n", i, ephoriz2_dist[i]);
  // }
  for (int i = 0; i < RING_S+1; i++)
  {
    fprintf(fp, "s_dist[%d],%d\n", i, s_dist[i]);
  }
  for (int i = 0; i < RING_S+1; i++)
  {
    fprintf(fp, "allocS_dist[%d],%d\n", i, allocS_dist[i]);
  }

  fprintf(fp, "s_underctr,%lld\n", s_underctr);
  fprintf(fp, "s_overctr,%lld\n", s_overctr);
  fprintf(fp, "s_inctr,%lld\n", s_inctr);
  fprintf(fp, "inplacectr,%lld\n", inplacectr);
  fprintf(fp, "takenctr,%lld\n", takenctr);
  fprintf(fp, "extendctr,%lld\n", extendctr);
  for (int i = GATHER_START; i < LEVEL; i++)
  {
    fprintf(fp, "s_under[%d],%d\n", i, s_under[i]);
  }
  for (int i = 0; i < LEVEL; i++)
  {
    fprintf(fp, "deadctr_arr[%d],%lld\n", i, deadctr_arr[i]);
  }
  for (int i = GATHER_START; i < LEVEL; i++)
  {
    fprintf(fp, "deadQ_ov[%d],%lld\n", i, deadQ_ov[i]);
  }
  for (int i = GATHER_START; i < LEVEL; i++)
  {
    fprintf(fp, "deadQ_empty_s6[%d],%lld\n", i, deadQ_empty_s6[i]);
  }
  for (int i = GATHER_START; i < LEVEL; i++)
  {
    fprintf(fp, "deadQ_empty_s7[%d],%lld\n", i, deadQ_empty_s7[i]);
  }
  for (int i = GATHER_START; i < LEVEL; i++)
  {
    fprintf(fp, "Q_serve_under[%d],%lld\n", i, Q_serve_under[i]);
  }
  for (int i = GATHER_START; i < LEVEL; i++)
  {
    fprintf(fp, "Q_serve_over[%d],%lld\n", i, Q_serve_over[i]);
  }
  for (int i = GATHER_START; i < LEVEL; i++)
  {
    fprintf(fp, "cap_Q_full[%d],%lld\n", i, cap_Q_full[i]);
  }
  for (int i = GATHER_START; i < LEVEL; i++)
  {
    fprintf(fp, "cap_Q_notfull[%d],%lld\n", i, cap_Q_notfull[i]);
  }
  for (int i = GATHER_START; i < LEVEL; i++)
  {
    fprintf(fp, "dead_gathered[%d],%d\n", i, dead_gathered[i]);
  }
  // for (int i = TOP_CACHE_VAR; i < LEVEL; i++)
  // {
  //   for (int j = 0; j < RING_S; j++)
  //   {
  //     fprintf(fp, "ep_s[%d][%d],%d\n", i, j, ep_s[i][j]);
  //   }
  // }
  // for (int i = 0; i < MAX_SHUF + 2; i++)
  // {
  //   fprintf(fp, "ep_shuf[%d],%d\n", i, ep_shuf[i]);
  // }

  fprintf(fp, "stash_hit,%d\n", stash_hit);

  for (int i = GATHER_START; i < LEVEL; i++)
  {
    fprintf(fp, "dead_encountered[%d],%d\n", i, dead_encountered[i]);
  }
  for (int i = GATHER_START; i < LEVEL; i++)
  {
    fprintf(fp, "dead_shadowed[%d],%d\n", i, dead_shadowed[i]);
  }
  for (int i = GATHER_START; i < LEVEL; i++)
  {
    fprintf(fp, "dead_scan[%d],%d\n", i, dead_scan[i]);
  }
  for (int i = GATHER_START; i < LEVEL; i++)
  {
    fprintf(fp, "shad_added[%d],%d\n", i, shad_added[i]);
  }
  fprintf(fp, "same_bucket,%d\n", same_bucket);
  fprintf(fp, "remote_under,%lld\n", remote_under_w - remote_under_r);
  fprintf(fp, "remote_over,%lld\n", remote_over_w - remote_over_r);
  // for (int i = GATHER_START; i < LEVEL; i++)
  // {
  //   fprintf(fp, "ddctr_arr[%d],%d\n", i, ddctr_arr[i]);
  // }

  // print_lifetime_stat(fp);

  // char real[5] = "real";
  // char dum[5] = "dum";
  // print_super_node(supreal, exp_name, bench, real);
  // print_super_node(supdum, exp_name, bench, dum);
  
  fclose(fp);
}


void print_oram_stats(){

  // print_count_level();

  // print_shuff_stat();

  // print_ref_close_stat();
  // print_stash();

  // print_wb_stat();

  // print_shuff_dist();

  int shuffctr = 0; 
  for (int i = TOP_CACHE; i < LEVEL; i++)
  {
    shuffctr += shuff[i];
  }
  

  // printf("\n\n\n\n............... ORAM Stats ...............\n\n");
  // printf("Execution Time (s)       %f\n", exe_time);
  // printf("Total Cycles             %lld \n", CYCLE_VAL);
  // printf("Trace Size               %d\n", tracectr);
  // printf("Mem Cycles               %lld\n", mem_clk);
  // printf("Invoke Mem               %d\n", invokectr);
  // printf("ORAM Access              %d\n", oramctr);
  // printf("ORAM Dummy               %d\n", dummyctr);
  // printf("Pos1 Access              %d\n", pos1_access);
  // printf("Pos2 Access              %d\n", pos2_access);
  // printf("PLB pos0 hit             %f%%\n", 100*(double)plb_hit[0]/plbaccess[0]);
  // printf("PLB pos1 hit             %f%%\n", 100*(double)plb_hit[1]/plbaccess[1]);
  // printf("PLB pos2 hit             %f%%\n", 100*(double)plb_hit[2]/plbaccess[2]);
  // printf("PLB pos0 hit             %lld\n", plb_hit[0]);
  // printf("PLB pos1 hit             %lld\n", plb_hit[1]);
  // printf("PLB pos2 hit             %lld\n", plb_hit[2]);
  // printf("PLB pos0 acc             %lld\n", plbaccess[0]);
  // printf("PLB pos1 acc             %lld\n", plbaccess[1]);
  // printf("PLB pos2 acc             %lld\n", plbaccess[2]);
  // printf("oramQ Size               %d\n", oramQ->size);
  // printf("Bk Evict                 %f%%\n", 100*(double)bkctr/oramctr);
  // printf("Bk Evict                 %d\n", bkctr);
  // printf("Cache Hit                %f%%\n", 100*(double)hitctr/(hitctr+missctr));
  // printf("Cache Evict              %f%%\n", 100*(double)evictctr/(missctr));
  // printf("Rho Hit                  %f%%\n", 100*(double)rho_hit/(invokectr));
  // printf("Rho Access               %d\n", rhoctr);
  // printf("Rho  Dummy               %d\n", rho_dummyctr);
  // printf("Rho Bk Evict             %f%%\n", 100*(double)rho_bkctr/rho_hit);
  // printf("Early WB                 %d\n", earlyctr);
  // printf("Early WB Pointer         %d\n", dirty_pointctr);
  // printf("Cache Dirty              %d\n", cache_dirty);
  // printf("ptr fail                 %d\n", ptr_fail);
  // printf("search fail              %d\n", search_fail);
  // printf("pin ctr                  %d\n", pinctr);
  // printf("unpin ctr                %d\n", unpinctr);
  // printf("prefetch case            %d\n", precase);
  // printf("STT Cand                 %d\n", sttctr);
  // printf("Stash leftover           %d\n", stash_leftover);
  // printf("Stash removed            %d\n", stash_removed);
  // printf("fill hit                 %d\n", fillhit);
  // printf("fill miss                %d\n", fillmiss);
  // printf("Top hit                  %f%%\n", 100*(double)topctr/(topctr+midctr+botctr));
  // printf("Mid hit                  %f%%\n", 100*(double)midctr/(topctr+midctr+botctr));
  // printf("Bot hit                  %f%%\n", 100*(double)botctr/(topctr+midctr+botctr));
  // printf("Ring evict               %d\n", ring_evictctr);
  // printf("Stash occ                %d\n", stashctr);
  // printf("Stash Contain            %d\n", stash_cont);
  // printf("Linger Discard           %d\n", linger_discard);
  // printf("Ring shuff 10+           %d\n", shuffctr);
  // printf("Ring acc                 %d\n", ringctr);
  // printf("EP writeback             %d\n", wbctr);
  // printf("W request                %d\n", writectr);
  // printf("W skipped                %d\n", wskip);
  // printf("Mem req latency          %f\n", (double)mem_req_latencies/(invokectr));
  // printf("Nonmemops                %lld\n", nonmemops_sum);
  // printf("Miss L1 shad             %lld\n", missl1wb);
  // printf("Miss L1 ratio            %f%%\n", 100*(double)missl1wb/missctr);
  // printf("Shuff wb                 %d\n", wbshuff);
  // printf("Ring dummy               %d\n", ringdumctr);
  // printf("WL Pos1 acc              %d\n", wl_pos[1]);
  // printf("WL Pos2 acc              %d\n", wl_pos[2]);
  // printf("flush ctr                %d\n", stale_flush_ctr );
  // printf("discard ctr              %d\n", stale_discard_ctr );
  // prinf("Path Latency Avg         %f\n", path_access_latency_avg);
}


bool is_nvm_channel(int channel){
  if (channel >= (NUM_CHANNELS - NVM_CHANNEL))
  {
    return true;
  }
  
  return false;
}

void update_ddr_timing_param(int channel){
  bool nvm = is_nvm_channel(channel);
  nvm = false;

  T_RCD        = nvm ?   44*NVM_LATENCY      :        44;                  // 88
  T_RP         = nvm ?   44      :        44;  // 60 ~ 5 / 528 // 240

  T_CAS        = nvm ?    44*NVM_LATENCY      :        44;
  T_RC         = nvm ?   156      :       156;
  T_RAS        = nvm ?   112      :       112;

  T_RRD        = nvm ?    20      :        20;                  // 44

  T_FAW        = nvm ?   128      :       128;
  T_WR         = nvm ?    48      :        48;
  T_WTR        = nvm ?    24      :        24;
  T_RTP        = nvm ?    24      :        24;
  T_CCD        = nvm ?    16      :        16;
  T_RFC        = nvm ?   352      :       352;
  T_REFI       = nvm ? 24960      :     24960;
  T_CWD        = nvm ?    20      :        20;
  T_RTRS       = nvm ?     8      :         8;
  T_PD_MIN     = nvm ?    16      :        16;
  T_XP         = nvm ?    20      :        20;
  T_XP_DLL     = nvm ?    80      :        80;
  T_DATA_TRANS = nvm ?    16      :        16;
}


void adjust_ddr(long long int addr){
  bool nvm = is_nvm_addr_byte(addr) && NVM_ENABLE;
  // nvm = true;

  T_RCD = nvm ?   44*NVM_LATENCY      :        44;                 
  // T_CAS        = nvm ?   44*NVM_LATENCY      :        44;
  // T_WR        = nvm ?   48*NVM_LATENCY      :        48;
  // T_WTR        = nvm ?   24*NVM_LATENCY      :        24;
  T_RP = nvm ?   44*NVM_LATENCY      :        44;
  // T_RRD = nvm ?    20*NVM_LATENCY      :        20;                  // 44


  // if (nvm)
  // {
    // printf("@ % lld T_RCD  %d, T_CAS %d\n", CYCLE_VAL, T_RCD, T_CAS);
  // }
  
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
  // long long int blkaddr;
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
  // blkaddr = input_a;
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

  // if (NVM_ENABLE)
  // {
  //   int cur_chan = this_a->channel;
  //   int dram_chan = NUM_CHANNELS - NVM_CHANNEL;
  //   unsigned long long int data_addr_byte = (unsigned long long int) DATA_ADDR_SPACE << (int)log2(BLOCK_SIZE);
  //   unsigned long long int metadata_nvm_byte = DATA_ADDR_SPACE + (pow(2, NVM_START)-1)*1;
  //   metadata_nvm_byte =  (unsigned long long int) metadata_nvm_byte << (int)log2(BLOCK_SIZE);
  //   if ((physical_address >= NVM_ADDR_BYTE && physical_address <=  data_addr_byte) )// || physical_address >= metadata_nvm_byte)
  //   {
  //     this_a->channel = NUM_CHANNELS - NVM_CHANNEL + (cur_chan % NVM_CHANNEL);
  //   }
  //   else
  //   {
  //     this_a->channel = blkaddr % dram_chan;
  //   }
  // }
  
  return (this_a);
}


// Function to create a new request node to be inserted into the read
// or write queue.
  void *
init_new_node (long long int physical_address, long long int arrival_time,
    optype_t type, int thread_id, int instruction_id,
    long long int instruction_pc, int oramid, TreeType tree, bool last_read, bool nvm_access, char op_type, bool beginning, bool ending, bool last_req, int reqid) 
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
    new_node->last_read = last_read;
    new_node->last_req = last_req;
    new_node->nvm_access = nvm_access;
    new_node->beginning = beginning;
    new_node->ending = ending;
    new_node->op_type = op_type;
    new_node->reqid = reqid;
    new_node->countdown = (!is_nvm_addr_byte(physical_address)) ? 0 : (type == READ) ? 60*NVM_LATENCY : 64*NVM_LATENCY*2 ;
    // new_node->countdown = 0;
    // if (nvm_access)
    // {
    //   printf("nvm   ");
    // }
    // else
    // {
    //   printf("dram  ");
    // }
    
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
    // printf("channel %d\n", this_node_addr->channel);
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
    int instruction_id, long long int instruction_pc, int oramid, TreeType tree, bool last_read, bool nvm_access, char op_type, bool beginning, bool ending, bool last_req, int reqid) 
{
  optype_t this_op = READ;

  //get channel info
  dram_address_t * this_addr = calc_dram_addr (physical_address);
  int channel = this_addr->channel;
  free (this_addr);
  stats_reads_seen[channel]++;
  request_t * new_node =
    init_new_node (physical_address, arrival_time, this_op, thread_id,
        instruction_id, instruction_pc, oramid, tree, last_read, nvm_access, op_type, beginning, ending, last_req, reqid);
  LL_APPEND (read_queue_head[channel], new_node);
  read_queue_length[channel]++;

  //UT_MEM_DEBUG("\nCyc: %lld New READ:%lld Core:%d Chan:%d Rank:%d Bank:%d Row:%lld RD_Q_Length:%lld\n", CYCLE_VAL, new_node->id, new_node->thread_id, new_node->dram_addr.channel,  new_node->dram_addr.rank,  new_node->dram_addr.bank,  new_node->dram_addr.row, read_queue_length[channel]);
  return new_node;
}


// Insert a new write to the write queue
request_t * insert_write (long long int physical_address,
    long long int arrival_time, int thread_id,
    int instruction_id, int oramid, TreeType tree, bool nvm_access, char op_type, bool beginning, bool ending, bool last_req, bool last_read, int reqid) 
{
  optype_t this_op = WRITE;
  dram_address_t * this_addr = calc_dram_addr (physical_address);
  int channel = this_addr->channel;
  free (this_addr);
  stats_writes_seen[channel]++;
  request_t * new_node =
    init_new_node (physical_address, arrival_time, this_op, thread_id,
        instruction_id, 0, oramid, tree, last_read, nvm_access, op_type, beginning, ending, last_req, reqid);
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
    // if (curr->countdown > 0)
    // {
    //   curr->countdown--;
    //   continue;
    // }
    

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
    // if (curr->countdown > 0)
    // {
    //   curr->countdown--;
    //   continue;
    // }

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


void set_comptime_max(request_t * request){
  long long int t = request->completion_time;
  int req = request->reqid;
  
  if (t > comptime_max)
  {
    comptime_max = t;
    longest_req = req; 
    
  }
  
  if (request->nvm_access)
  {
    if (t > nvm_tmax)
    {
      nvm_tmax = t;
      longest_nvm = req;
    }
  }
  else
  {
    if (t > dram_tmax)
    {
      dram_tmax = t;
      longest_dram = req;
    }

  }
  
}

void update_served_count(request_t * request){
  set_comptime_max(request);
  if(request->operation_type == READ)
  {
    // printf("in read \n");
    if (request->op_type == 'o' )//&& request->oramid == cur_online)
      {
        // printf("in online\n");
        online_r++;
        if (request->nvm_access)
        {
          if (cur_nvm_served_o == nvm_to_serve_o)
          {

            // cur_nvm_served_o = 0;
            // printf("o %d nvm ends %lld  @ %lld  started %lld\n", request->oramid, request->completion_time, CYCLE_VAL, online_t0);
          }
          else
          {
            cur_nvm_served_o++;
            if (cur_nvm_served_o == nvm_to_serve_o)
            {
              // online_wait_nvm += nvm_tmax - online_t0;
              detnvm = longest_nvm;
            // online_wait_nvm += request->completion_time - online_t0;
            // online_wait_nvm += CYCLE_VAL - online_t0;
              /* code */
            }
            
            
          }
        }
        else
        {
          if (cur_dram_served_o == dram_to_serve_o)
          {
            r_ended_o++;
            // cur_dram_served_o = 0;
            // printf("o %d dram ends %lld  @ %lld started %lld\n", request->oramid, request->completion_time, CYCLE_VAL, online_t0);
          }
          else
          {
            cur_dram_served_o++;
            if (cur_dram_served_o == dram_to_serve_o)
            {
              // online_wait_dram += dram_tmax - online_t0;
              detdram = longest_dram;
            // online_wait_dram += request->completion_time - online_t0;
            // online_wait_dram += CYCLE_VAL - online_t0;
              /* code */
            }
            
          }
        }
        
      }
      else if (request->op_type == 'e' )//&& request->oramid == cur_evict)
      {
        // printf("in evict\n");
        evict_r++;
        if (request->nvm_access)
        {
          if (cur_nvm_served_e_r == nvm_to_serve_e_r)
          {
            // evict_wait_nvm += request->completion_time - evict_t0;
            // cur_nvm_served_e_r = 0;
          }
          else
          {
            cur_nvm_served_e_r++;
          }
        }
        else
        {
          if (cur_dram_served_e_r == dram_to_serve_e_r)
          {
            r_ended++;
            // printf("curwrite %d   oramid %d     curaccess %lld \n", cur_dram_served_e_w, request->oramid, cur_evict);
            // evict_wait_dram += request->completion_time - evict_t0;
            // cur_dram_served_e_r = 0;
          }
          else
          {
            cur_dram_served_e_r++;
          }
        }

      }
      else if (request->op_type == 'r' ) //&& request->oramid == cur_reshuffle)
      {
        // printf("in reshuffle\n");
        reshuffle_r++;

        if (request->nvm_access)
        {
          if (cur_nvm_served_r_r == nvm_to_serve_r_r)
          {
            // reshuffle_wait_nvm += request->completion_time - reshuffle_t0;
            // cur_nvm_served_r_r = 0;
          }
          else
          {
            cur_nvm_served_r_r++;
          }
        }
        else
        {
          if (cur_dram_served_r_r == dram_to_serve_r_r)
          {
            // reshuffle_wait_dram += request->completion_time - reshuffle_t0;
            // cur_dram_served_r_r = 0;
          }
          else
          {
            cur_dram_served_r_r++;
          }
        }
      }
      else if (request->op_type == 'm' )//&& request->oramid == cur_evict)
      {
        // printf("in m \n");
        
        if (request->nvm_access)
        {
          // printf("in nvm \n");
          // if (cur_nvm_served_m_r == nvm_to_serve_m_r)
          // {
          //   meta_wait_nvm += request->completion_time - meta_t0;
          //   cur_nvm_served_m_r = 0;
          // }
          // else
          // {
          //   cur_nvm_served_m_r++;
          // }
        }
        else
        {
          if (cur_dram_served_m_r == dram_to_serve_m_r)
          {
            // printf("curwrite %d   oramid %d     curaccess %lld \n", cur_dram_served_e_w, request->oramid, cur_evict);
            // meta_wait_dram += request->completion_time - meta_t0;
            // cur_dram_served_m_r = 0;
          }
          else
          {
            // printf("hereeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee\n");
            cur_dram_served_m_r++;
          }
        }
      }
  }
  else if(request->operation_type == WRITE)
  {
    if (request->op_type == 'e' )//&& request->oramid == cur_evict)
      {
        // printf("in evict\n");
        evict_w++;
        if (request->nvm_access)
        {
          if (cur_nvm_served_e_w == nvm_to_serve_e_w)
          {
            // evict_wait_nvm += request->completion_time - evict_t0;
            // cur_nvm_served_e_w = 0;
            // printf("e %d nvm ends %lld  @ %lld started %lld\n", request->oramid, request->completion_time, CYCLE_VAL, evict_t0);

          }
          else
          {
            cur_nvm_served_e_w++;
            if (cur_nvm_served_e_w == nvm_to_serve_e_w)
            {
              // evict_wait_nvm += nvm_tmax - evict_t0;
              // detnvm = longest_nvm;
              detnvm = request->reqid;
              // evict_wait_nvm += request->completion_time - evict_t0;
              // evict_wait_nvm += CYCLE_VAL - evict_t0;
            }
          }
        }
        else
        {
          if (cur_dram_served_e_w == dram_to_serve_e_w)
          {
            w_ended++;
            // printf("curead %d     oramid %d     curaccess %lld \n", cur_dram_served_e_r, request->oramid, cur_evict);
            // evict_wait_dram += request->completion_time - evict_t0;
            // cur_dram_served_e_w = 0;
            // printf("e %d dram ends %lld  @ %lld started %lld\n", request->oramid, request->completion_time, CYCLE_VAL, evict_t0);

          }
          else
          {
            cur_dram_served_e_w++;
            if (cur_dram_served_e_w == dram_to_serve_e_w)
            {
              // evict_wait_dram += dram_tmax - evict_t0;
              // detdram = longest_dram;
              detdram = request->reqid;

              // detdram = request->reqid;
              // evict_wait_dram += request->completion_time - evict_t0;
              // evict_wait_dram += CYCLE_VAL - evict_t0;

            }
          }
        }

      }
      else if (request->op_type == 'r' )//&& request->oramid == cur_reshuffle)
      {
        reshuffle_w++;

        // printf("in reshuffle\n");

        if (request->nvm_access)
        {
          if (cur_nvm_served_r_w == nvm_to_serve_r_w)
          {
            w_ended_nvm++;
            // reshuffle_wait_nvm += request->completion_time - reshuffle_t0;
            // cur_nvm_served_r_w = 0;
            // printf("r %d nvm ends %lld  @ %lld  started %lld\n", request->oramid, request->completion_time, CYCLE_VAL, reshuffle_t0);

          }
          else
          {
            cur_nvm_served_r_w++;
            if (cur_nvm_served_r_w == nvm_to_serve_r_w)
            {
              w_ended_nvm++;
              // reshuffle_wait_nvm += nvm_tmax - reshuffle_t0;
              // detnvm = longest_nvm;
              detnvm = request->reqid;

              // reshuffle_wait_nvm += request->completion_time - reshuffle_t0;
              // reshuffle_wait_nvm += CYCLE_VAL - reshuffle_t0;

            }
          }
        }
        else
        {
          if (cur_dram_served_r_w == dram_to_serve_r_w)
          {
            w_ended_dram++;
            // reshuffle_wait_dram += request->completion_time - reshuffle_t0;
            // cur_dram_served_r_w = 0;
            // printf("r %d dram ends %lld @ %lld  started %lld\n", request->oramid, request->completion_time, CYCLE_VAL, reshuffle_t0);
          }
          else
          {
            cur_dram_served_r_w++;
            if (cur_dram_served_r_w == dram_to_serve_r_w)
            {
              // reshuffle_wait_dram += dram_tmax - reshuffle_t0;
              // detdram = longest_dram;
              detdram = request->reqid;

              // reshuffle_wait_dram += request->completion_time - reshuffle_t0;
              // reshuffle_wait_dram += CYCLE_VAL - reshuffle_t0;
            }
          }
        }
      }
      else if (request->op_type == 'm' )//&& request->oramid == cur_evict)
      {
        if (request->nvm_access)
        {
          // if (cur_nvm_served_m_w == nvm_to_serve_m_w)
          // {
          //   meta_wait_nvm += request->completion_time - meta_t0;
          //   cur_nvm_served_m_w = 0;
          // }
          // else
          // {
          //   cur_nvm_served_m_w++;
          // }
        }
        else
        {
          if (cur_dram_served_m_w == dram_to_serve_m_w)
          {
            // printf("curwrite %d   oramid %d     curaccess %lld \n", cur_dram_served_e_w, request->oramid, cur_evict);
            meta_ended++;
            // meta_wait_dram += request->completion_time - meta_t0;
            // cur_dram_served_m_w = 0;
            // printf("m %d dram ends %lld @ %lld  started %lld\n", request->oramid, request->completion_time, CYCLE_VAL, meta_t0);
          }
          else
          {
            cur_dram_served_m_w++;
            if (cur_dram_served_m_w == dram_to_serve_m_w)
            {
              meta_ended++;
              // meta_wait_dram += dram_tmax - meta_t0;
              // detdram = longest_dram;
              detdram = request->reqid;


              // meta_wait_dram += request->completion_time - meta_t0;
              // meta_wait_dram += CYCLE_VAL - meta_t0;
            }
          }
        }
      }
  }
}

long long int sum_wait_sofar(){
  long long int sum = 0;
  sum +=  max(online_wait_dram, online_wait_nvm);
  sum += max(evict_wait_dram, evict_wait_nvm);
  sum += reshuffle_wait_dram + reshuffle_wait_nvm;
  sum += meta_wait_dram;
  return sum;
}

void calc_wait_value(char op_type, int reqid, long long int comptime, int oramid){
  // long long int t1 = comptime - PIPELINEDEPTH;
  long long int t1 = CYCLE_VAL;
  // bool isnvm;
  // bool end = false;

  // if (reqid == detnvm || reqid == detdram)
  // {
  //   isnvm = (reqid == detnvm);
  //   end = true;
  // }
  
  if (op_type == 'o')
  {
    if (reqid == detnvm)
    {
      // if (t1 - online_t0 > 899)
      // {
        // printf("%c %d waitO %lld\n", op_type, oramid, t1 - online_t0);
      // }
      onvm++;
      online_wait_nvm += t1 - online_t0;
      detnvm = 0;
    }
    else if (reqid == detdram)
    {
      // if (t1 - online_t0 > 899)
      // {
        // printf("%c %d waitO %lld\n", op_type, oramid, t1 - online_t0);
      // }
      
      odram++;
      online_wait_dram += t1 - online_t0;
      detdram = 0;
    }
    
  }
  else if (op_type == 'e')
  {
    if (reqid == detnvm)
    {
      // printf("waitE %lld\n", t1 - evict_t0);
      envm++;
      evict_wait_nvm += t1 - evict_t0;
      detnvm = 0;
    }
    else if (reqid == detdram)
    {
      // printf("waitE %lld\n", t1 - evict_t0);
      edram++;
      evict_wait_dram += t1 - evict_t0;
      detdram = 0;
    }
  }
  else if (op_type == 'r')
  {
    if (reqid == detnvm)
    {
      // printf("waitR %lld\n", t1 - reshuffle_t0);
      rnvm++;
      reshuffle_wait_nvm += t1 - reshuffle_t0;
      detnvm = 0;
    }
    else if (reqid == detdram)
    {
      // printf("waitR %lld\n", t1 - reshuffle_t0);
      rdram++;
      reshuffle_wait_dram += t1 - reshuffle_t0;
      detdram = 0;
    }
  }
  else if (op_type == 'm')
  {
    if (reqid == detdram)
    {
      // printf("waitM %lld\n", t1 - meta_t0);
      mdram++;
      meta_wait_dram += t1 - meta_t0;
      detdram = 0;
    }
  }

  // if (end)
  // {
    // long long int sum_val = sum_wait_sofar();
    // printf("%c %d end %s req%d	@ %lld    sum %lld  %s  comp %lld\n", op_type, oramid, isnvm?"nvm":"dram", reqid, CYCLE_VAL, sum_val, (CYCLE_VAL < sum_val)?"exceed":"", comptime);
  // }
  

}

void determine_served_all(request_t * request){
  optype_t type = request->operation_type;

  // printf("%c %d req%d ", request->op_type, request->oramid, request->reqid);
  if (request->op_type == 'o')
  { 

    // printf("b4 req%d dram %d   nvm %d  lsr %d\n", request->reqid, cur_dram_served_o, cur_nvm_served_o,  last_read_deleted);
    // printf("comp detreq %lld    comp max %lld\n", request->completion_time, comptime_max);
    if (!last_read_deleted)
    {
      last_read_deleted = (cur_dram_served_o == dram_to_serve_o) && (cur_nvm_served_o == nvm_to_serve_o);

      if (last_read_deleted)
      {
        // printf("curdram %d   dramserv %d   curnvm %d    nvmserv %d\n", cur_dram_served_o, dram_to_serve_o, cur_nvm_served_o, nvm_to_serve_o);
        // printf("%c %d detserve req%d	@ %lld  longestReq %d   comptime %lld   longestNVM %d comp %lld   curnvm %d \n", request->op_type, request->oramid, request->reqid, CYCLE_VAL, longest_req, comptime_max, longest_nvm, nvm_tmax, cur_nvm_served_o);

        determineReq = longest_req;
        determineCycle = comptime_max;
        cur_dram_served_o = 0;
        cur_nvm_served_o = 0;
      }
    }
    // printf("af req%d dram %d   nvm %d  lsr %d\n", request->reqid, cur_dram_served_o, cur_nvm_served_o,  last_read_deleted);
  }
  else if (request->op_type == 'e')
  {
    // printf("comp detreq %lld    comp max %lld\n", request->completion_time, comptime_max);
    if (type == READ)
    {
      // printf("%c %d req%d ", request->op_type, request->oramid, request->reqid);
      // printf("b4 req%d dramR %d   nvmR %d  lsr %d\n", request->reqid, cur_dram_served_e_r, cur_nvm_served_e_r,  last_read_deleted);
      if (!last_read_deleted)
      {
        // last_read_deleted = (type == READ) ? (cur_dram_served_e_r == dram_to_serve_e_r) && (cur_nvm_served_e_r == nvm_to_serve_e_r)
        //                                 : (cur_dram_served_e_w == dram_to_serve_e_w) && (cur_nvm_served_e_w == nvm_to_serve_e_w);
        last_read_deleted = (cur_dram_served_e_r == dram_to_serve_e_r) && (cur_nvm_served_e_r == nvm_to_serve_e_r);

        if (last_read_deleted)
        {
        // printf("%c %d detserve req%d	@ %lld  longestReq %d   comptime %lld   longestNVM %d comp %lld   curnvm %d \n", request->op_type, request->oramid, request->reqid, CYCLE_VAL, longest_req, comptime_max, longest_nvm, nvm_tmax, cur_nvm_served_o);
          // if (type == READ)
          // {
            determineReq = longest_req;
            determineCycle = comptime_max;
            cur_dram_served_e_r = 0;
            cur_nvm_served_e_r = 0;
          // }
          // else 
          // {
          //   cur_dram_served_e_w = 0;
          //   cur_nvm_served_e_w = 0;
          // }
        }
      }
      // printf("%c %d req%d ", request->op_type, request->oramid, request->reqid);
      // printf("af req%d dramR %d   nvmR %d  lsr %d\n", request->reqid, cur_dram_served_e_r, cur_nvm_served_e_r,  last_read_deleted);
    }
    else if (type == WRITE)
    {
      // printf("%c %d req%d ", request->op_type, request->oramid, request->reqid);
      // printf("b4 req%d dramW %d   nvmW %d  lsw %d\n", request->reqid, cur_dram_served_e_w, cur_nvm_served_e_w,  last_read_served);
      if (!last_read_served)
      { 
        last_read_served = (cur_dram_served_e_w == dram_to_serve_e_w) && (cur_nvm_served_e_w == nvm_to_serve_e_w);
        if (last_read_served)
        {
          // printf("%c %d servedW req%d dramW %d   nvmW %d  lsw %d\n", request->op_type, request->oramid, request->reqid, cur_dram_served_e_w, cur_nvm_served_e_w,  last_read_served);
          cur_dram_served_e_w = 0;
          cur_nvm_served_e_w = 0;
        }
      }
      // printf("%c %d req%d ", request->op_type, request->oramid, request->reqid);
      // printf("af req%d dramW %d   nvmW %d  lsw %d\n", request->reqid, cur_dram_served_e_w, cur_nvm_served_e_w,  last_read_served);
    }
    

  }
  else if (request->op_type == 'r')
  {
    if (type == READ)
    {
      if (!last_read_deleted)
      {
        if (request->nvm_access)
        {
          last_read_deleted = (cur_nvm_served_r_r == nvm_to_serve_r_r);
          // last_read_deleted = (type == READ) ?  (cur_nvm_served_r_r == nvm_to_serve_r_r)
          //                               :  (cur_nvm_served_r_w == nvm_to_serve_r_w);
        }
        else
        {
          last_read_deleted = (cur_dram_served_r_r == dram_to_serve_r_r) ;
          // last_read_deleted = (type == READ) ? (cur_dram_served_r_r == dram_to_serve_r_r) 
          //                               : (cur_dram_served_r_w == dram_to_serve_r_w);
        }
        


        if (last_read_deleted)
        {
          // printf("detreq r: %d\n", request->reqid);
          // if (type == READ)
          // {
            determineReq = longest_req;
            determineCycle = comptime_max;
            cur_dram_served_r_r = 0;
            cur_nvm_served_r_r = 0;
          // }
          // else 
          // {
          //   cur_dram_served_r_w = 0;
          //   cur_nvm_served_r_w = 0;
          // }
        }                  
      }
    }
    else if (type == WRITE)
    {
      if (!last_read_served)
      { 
         if (request->nvm_access)
        {
          last_read_served = (cur_nvm_served_r_w == nvm_to_serve_r_w);
        }
        else
        {
          last_read_served = (cur_dram_served_r_w == dram_to_serve_r_w) ;
        }
        if (last_read_served)
        {
          cur_dram_served_r_w = 0;
          cur_nvm_served_r_w = 0;
        }
      }
    }

  }
  else if (request->op_type == 'm')
  {
    // printf("comp req%d %lld    comp max %lld\n", request->reqid, request->completion_time, comptime_max);

    if (type == READ)
    {
      // printf("%c %d req%d ", request->op_type, request->oramid, request->reqid);
      // printf("b4 req%d dramR %d   nvmR %d  lsr %d\n", request->reqid, cur_dram_served_m_r, cur_nvm_served_m_r,  last_read_deleted);
      if (!last_read_deleted)
      {
        last_read_deleted = (cur_dram_served_m_r == dram_to_serve_m_r) && (cur_nvm_served_m_r == nvm_to_serve_m_r);
        // last_read_deleted = (type == READ) ? (cur_dram_served_m_r == dram_to_serve_m_r) && (cur_nvm_served_m_r == nvm_to_serve_m_r)
        //                                 : (cur_dram_served_m_w == dram_to_serve_m_w) && (cur_nvm_served_m_w == nvm_to_serve_m_w);
        if (last_read_deleted)
        {
          // if (type == READ)
          // {
            // printf("curdram %d   dramserv %d   curnvm %d    nvmserv %d\n", cur_dram_served_m_r, dram_to_serve_m_r, cur_nvm_served_m_r, nvm_to_serve_m_r);
            // printf("%c %d detserve req%d	@ %lld  longestReq %d   comptime %lld   longestNVM %d comp %lld   curnvm %d \n", request->op_type, request->oramid, request->reqid, CYCLE_VAL, longest_req, comptime_max, longest_nvm, nvm_tmax, cur_nvm_served_o);
            determineReq = longest_req;
            determineCycle = comptime_max;
            cur_dram_served_m_r = 0;
            cur_nvm_served_m_r = 0;
          // }
          // else 
          // {
          //   cur_dram_served_m_w = 0;
          //   cur_nvm_served_m_w = 0;
          // }
        }                  
      }
      // printf("%c %d req%d ", request->op_type, request->oramid, request->reqid);
      // printf("af req%d dramR %d   nvmR %d  lsr %d\n", request->reqid, cur_dram_served_m_r, cur_nvm_served_m_r,  last_read_deleted);
    }
    else if (type == WRITE)
    {
      // printf("%c %d req%d ", request->op_type, request->oramid, request->reqid);
      // printf("b4 req%d dramW %d   nvmW %d  lsw %d\n", request->reqid, cur_dram_served_m_w, cur_nvm_served_m_w,  last_read_served);
      if (!last_read_served)
      { 
        last_read_served = (cur_dram_served_m_w == dram_to_serve_m_w) && (cur_nvm_served_m_w == nvm_to_serve_m_w);
        if (last_read_served)
        {
          cur_dram_served_m_w = 0;
          cur_nvm_served_m_w = 0;
        }
      }
      // printf("%c %d req%d ", request->op_type, request->oramid, request->reqid);
      // printf("af req%d dramW %d   nvmW %d  lsw %d\n", request->reqid, cur_dram_served_m_w, cur_nvm_served_m_w,  last_read_served);
    }
    

  }
  
}


request_t * form_request(int operation_type, int oramid, char op_type, int reqid, bool nvm_access, long long int completion_time){
  request_t * req = (request_t *) malloc (sizeof (request_t));
  req->completion_time = completion_time;
  req->reqid = reqid;
  req->oramid = oramid;
  req->nvm_access = nvm_access;
  req->op_type = op_type;
  req->operation_type = (operation_type == 'R') ? READ : WRITE;
  return req;
}

// void count_w_served(char op_type, ){

// }


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
    if (rd_ptr->request_served == 1)  // && rd_ptr->completion_time <= CYCLE_VAL)
    {
      // if (tracectr >= 4200)
			// {
      // printf("%c %d deleteR req%d	@ %lld\n", rd_ptr->op_type, rd_ptr->oramid, rd_ptr->reqid, CYCLE_VAL);
			// }
      // if (rd_ptr->countdown > 0 )
      // {
      //   rd_ptr->countdown--;
      //   continue;
      // }
      
      update_served_count(rd_ptr);
      determine_served_all(rd_ptr);

      // if (rd_ptr->last_read)
      // {
			// 	last_read_served = true;
      // }
      
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
    if (wrt_ptr->request_served == 1) // && wrt_ptr->completion_time <= CYCLE_VAL)
    {
      // if (tracectr >= 4200)
      // {
      // printf("%c %d deleteW req%d	@ %lld\n", wrt_ptr->op_type, wrt_ptr->oramid, wrt_ptr->reqid, CYCLE_VAL);
      // }

      // if (wrt_ptr->countdown > 0 )
      // {
      //   wrt_ptr->countdown--;
      //   continue;
      // }

      //  if (wrt_ptr->completion_time > CYCLE_VAL)
      // {
      //   // wrt_ptr->countdown--;
      //   continue;
      // }
      

      // update_served_count(wrt_ptr);
      // determine_served_all(wrt_ptr);

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
issue_request_command (request_t * request, char rwt) 
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

  // if (request->beginning)
  // {
  //   if (request->op_type == 'o')
  //   {
  //     // printf("o %d begin   @ %lld\n", request->oramid, CYCLE_VAL);
  //     online_t0 = CYCLE_VAL;
  //     cur_online = request->oramid;
  //   }
  //   else if (request->op_type == 'e')
  //   {
  //     // printf("e %d begin  @ %lld\n", request->oramid, CYCLE_VAL);
  //     // printf("e\n");
  //     evict_t0 = CYCLE_VAL;
  //     cur_evict = request->oramid;
  //   }
  //   else if (request->op_type == 'r')
  //   {
  //     // printf("r\n");
  //     // printf("r %d begin  @ %lld\n", request->oramid, CYCLE_VAL);
  //     reshuffle_t0 = CYCLE_VAL;
  //     cur_reshuffle = request->oramid;
  //   }
  //   else if (request->op_type == 'm')
  //   {
  //     // printf("m %d begin @ %lld\n", request->oramid, CYCLE_VAL);
  //     // printf("r\n");
  //     meta_t0 = CYCLE_VAL;
  //     cur_meta = request->oramid;
  //   }
  // }

  adjust_ddr(request->physical_address);

  // bool isNVM =  (NVM_ENABLE && is_nvm_addr_byte(request->physical_address));
  // if (NVM_ENABLE && is_nvm_addr_byte(request->physical_address))
  // {
    // printf("AFTER ::: T_RCD  %d, T_CAS %d  @ %lld nvm? %d\n", T_RCD, T_CAS, CYCLE_VAL, isNVM);
  // }


  if (NVM_ENABLE && is_nvm_addr_byte(request->physical_address))
  {
    // update_ddr_timing_param(channel);
    // T_RCD = 44 * 5;
  }

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
        // if (is_nvm_addr_byte(request->physical_address) && NVM_ENABLE){
        //   printf("next read: % lld\n", dram_state[channel][rank][bank].next_read);
        //   // printf("cycle + T_RCD: % lld\n", cycle + T_RCD);
        //   printf("T_RCD: %d @ %lld\n", T_RCD, CYCLE_VAL);
        //   printf("T_CAS: %d @ %lld\n", T_CAS, CYCLE_VAL);
        //   printf("max(): % lld\n", max (cycle + T_RCD, dram_state[channel][rank][bank].next_read));
        // }
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

				// printf("%c %d issueR req%d	 @ %lld     comp %lld\n", request->op_type, request->oramid, request->reqid, CYCLE_VAL, request->completion_time);
        // if (request->beginning)
				// {
					
				// 	if (request->op_type == 'o')
				// 	{
				// 		online_t0 = CYCLE_VAL;
				// 		cur_online = request->oramid;
				// 	}
				// 	else if (request->op_type== 'e')
				// 	{
				// 		evict_t0 = CYCLE_VAL;
				// 		cur_evict = request->oramid;
				// 	}
				// 	else if (request->op_type== 'r')
				// 	{
				// 		reshuffle_t0 = CYCLE_VAL;
				// 		cur_reshuffle = request->oramid;
				// 	}
				// 	else if (request->op_type== 'm')
				// 	{
				// 		meta_t0 = CYCLE_VAL;
				// 		cur_meta = request->oramid;
				// 	}
				// }

        // if (request->nvm_access && !nvmt0_set)
        // {
        //   online_t0 = CYCLE_VAL;
        //   nvmt0_set = true;
        // }
        

      
      
      
      
      request->latency = request->completion_time - request->arrival_time;

      if (is_nvm_addr_byte(request->physical_address) && NVM_ENABLE)
      {
        // int coef = (rwt == 'R')? 1 : 3;
        // request->completion_time += NVM_LATENCY;
        // request->completion_time += 60*NVM_LATENCY;
        // printf("nvm  latency: %lld\n", request->latency);
        // printf("coef %d %s\n", coef, (rwt == 'R')? "R":"W");
      }


      request->dispatch_time = CYCLE_VAL;
      request->request_served = 1;
      // Mehrnoosh:

      // if (request->last_read)
			// {
      //   ROB[request->thread_id].waited_on[request->instruction_id] = true;
			// }
      // else
      // {
      //   ROB[request->thread_id].waited_on[request->instruction_id] = false;
      // }

      // ROB[request->thread_id].ending[request->instruction_id] = request->ending;
      // ROB[request->thread_id].beginning[request->instruction_id] = request->beginning;
      // ROB[request->thread_id].nvm_access[request->instruction_id] = request->nvm_access;
      // ROB[request->thread_id].op_type[request->instruction_id] = request->op_type;
      // ROB[request->thread_id].oramid[request->instruction_id] = request->oramid;


      // if (request->last_req)
			// {
			// 	// last_read_served = true;
      //   ROB[request->thread_id].waited_on[request->instruction_id] = true;
			// }
      // else
      // {
      //   ROB[request->thread_id].waited_on[request->instruction_id] = false;
      // }
      // Mehrnoosh.

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
      // if (!last_read_served)
      // {
      //   return 0;
      // }
      
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
      // printf("%c %d issueW req%d	@ %lld\n", request->op_type, request->oramid, request->reqid, CYCLE_VAL);

      request->completion_time = CYCLE_VAL + T_DATA_TRANS + T_WR;
     
      request->latency = request->completion_time - request->arrival_time;

      // if (is_nvm_addr_byte(request->physical_address) && NVM_ENABLE)
      // {
      //   request->completion_time += (request->latency) * NVM_LATENCY * 2 ;
      // }

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

      // Mehrnoosh: rob comp time added for write as well
      // ROB[request->thread_id].comptime[request->instruction_id] = request->completion_time + PIPELINEDEPTH;
      // Mehrnoosh.
     
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
  // T_RCD = 44;
  return 1;

}


// Function called to see if the rank can be transitioned into a fast low
// power state - ACT_PDN or PRE_PDN_FAST. 
  int
is_powerdown_fast_allowed (int channel, int rank) 
{
  if (NVM_ENABLE)
  {
    // update_ddr_timing_param(channel);
  }
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
  if (NVM_ENABLE)
  {
    // update_ddr_timing_param(channel);
  }
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
  if (NVM_ENABLE)
  {
    // update_ddr_timing_param(channel);
  }
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
  if (NVM_ENABLE)
  {
    // update_ddr_timing_param(channel);
  }
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
  if (NVM_ENABLE)
  {
    // update_ddr_timing_param(channel);
  } 
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
  if (NVM_ENABLE)
  {
    // update_ddr_timing_param(channel);
  }

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
  if (NVM_ENABLE)
  {
    // update_ddr_timing_param(channel);
  }

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
  if (NVM_ENABLE)
  {
    // update_ddr_timing_param(channel);
  }

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
  if (NVM_ENABLE)
  {
    // update_ddr_timing_param(channel);
  }
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
  if (NVM_ENABLE)
  {
    // update_ddr_timing_param(channel);
  }

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
  if (NVM_ENABLE)
  {
    // update_ddr_timing_param(channel);
  }
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
  if (NVM_ENABLE)
  {
    // update_ddr_timing_param(channel);
  }
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
  if (NVM_ENABLE)
  {
    // update_ddr_timing_param(channel);
  }
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
  if (NVM_ENABLE)
  {
    // update_ddr_timing_param(channel);
  }
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
  if (NVM_ENABLE)
  {
    // update_ddr_timing_param(channel);
  }
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
  if (NVM_ENABLE)
  {
    // update_ddr_timing_param(channel);
  }
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
  if (NVM_ENABLE)
  {
    // update_ddr_timing_param(channel);
  }
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
  if (NVM_ENABLE)
  {
    // update_ddr_timing_param(channel);
  }
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
  if (NVM_ENABLE)
  {
    // update_ddr_timing_param(channel);
  }
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
    if (NVM_ENABLE)
    {
      // update_ddr_timing_param(channel);
    }
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
  if (NVM_ENABLE)
  {
    // update_ddr_timing_param(channel);
  }
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


