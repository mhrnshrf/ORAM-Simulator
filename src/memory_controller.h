#ifndef __MEMORY_CONTROLLER_H__
#define __MEMORY_CONTROLLER_H__

#define MAX_NUM_CHANNELS 16
#define MAX_NUM_RANKS 16
#define MAX_NUM_BANKS 32

// Mehrnoosh:

#include "params.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// other simulation invariant
// #define TRACE_SIZE 300000000          // # addr read from trace file
// #define QUEUE_SIZE 3000               // oramq capacity
// #define PAGE_SIZE 4096                // page size in byte ~~~> 4KB
// #define L1_LATENCY 3                   // L1 latency in terms of # cycles 
// #define L2_LATENCY 10                  // L2 latency in terms of # cycles 
// #define MAINMEM_LATENCY 0             // mem latency in terms of # cycles 
// #define WARMUP_CACHE 0             // L2 warm up threshold, after which stats are gathered and memory accesses are actully made
// #define TIMEOUT_THRESHOLD 20000        // time out threshold in seconds
// #define TOP_BOUNDARY 10               // top region tree BOUNDARY
// #define MID_BOUNDARY 20                // middle region tree BOUNDARY

// enable/disable options config
// #define TIMEOUT_ENABLE  1     // 0/1 flag to disable/enable finishing the program in case it get stuck
// #define SUBTREE_ENABLE  0     // 0/1 flag to disable/enable having subtree addressing scheme
// #define CACHE_ENABLE    0     // 0/1 flag to disable/enable having cache
#define VOLCANO_ENABLE  0     // 0/1 flag to disable/enable having volcano idea both stt and stl
#define STT_ENABLE      0     // 0/1 flag to disable/enable stash top tree  ~> it won't matter if volcano is enabled
#define STL_ENABLE      0     // 0/1 flag to disable/enable slim tree level ~> it won't matter if volcano is enabled
// #define WRITE_BYPASS    0     // 0/1 flag to disable/enable cacheing the path id along the data in the LLC which will benefit write reqs to bypass posmap lookup 
// #define RHO_ENABLE      0     // 0/1 flag to disable/enable having rho
// #define TIMING_ENABLE   0     // 0/1 flag to disable/enable having timing channel security
// #define PREFETCH_ENABLE 0     // 0/1 flag to disable/enable having prefetching option in case of having timing channel security
// #define EARLY_ENABLE    0     // 0/1 flag to disable/enable early eviction option in case of having timing channel security
// #define SNAPSHOT_ENABLE 0     // 0/1 flag to disable/enable performing snapshot by making path oram accesses
// #define NONSEC_ENABLE   0     // 0/1 flag to disable/enable oram simulation if off usimm runs normally
// #define BK_EVICTION     0     // 0/1 flag to disable/enable background eviction
// #define SNAP_CACHE      0     // 0/1 flag to disable/enable  snapshot with having L2 cache
#define RING_ENABLE     1     // 0/1 flag to disable/enable ring oram (instead of path oram)
// #define RAND_ENABLE     0     // 0/1 flag to disable/enable rand address instead of trace addr
// #define WSKIP_ENABLE    0     // 0/1 flag to disable/enable write linger feature for ring oram
#define RSTL_ENABLE     0     // 0/1 flag to disable/enable stl feature for ring oram
// #define SKIP_ENABLE     0     // 0/1 flag to disable/enable skip middle level feature for ring oram
// #define LINGER_BASE     0     // 0/1 flag to disable/enable write linger baseline for ring oram
// #define DUMMY_ENABLE    0     // 0/1 flag to disable/enable dummy enable baseline for ring oram
// #define DYNAMIC_EP      0     // 0/1 flag that indicates whether ep occur based on number of reshuffles rather than static schedule or 
// #define META_ENABLE     0     // 0/1 flag that indicates whether stale info is stored in metadata tree
// #define SIM_ENABLE      0     // 0/1 flag that indicates whether usimm simulation is enabled if disabled only oram alg runs
// #define WAIT_ENABLE     0     // 0/1 flag that indicates whether wait for last read req to complete
// #define LLC_DIRTY       0     // 0/1 flag that indicates whether everything is dirty eviction from cache

// oram invariant
#define H 4     // degree of recursion including data access
#define X 128    // # label per posmap block
#define LEVEL 27 // # levels
#define USUAL_Z 4     // # slots per bucket in usual cases like path oram baseline and ir-oram
#define U 0.50 // utilization
#define RL 6     // # the reserved level
#define STASH_SIZE_ORG 300     // original size of stash
// #define TOP_CACHE 10   // # top levels that are cached ---------- freecursive: 10, volcano: don't care
#define L1 9   // upto L1 level buckts have specific Z1 number of slots   (inclusive)
#define L2 15   // upto L2 level buckts have specific Z2 number of slots   (inclusive)  // l24 ->17  ds15 ~> 14
#define L3 17   // upto L3 level buckts have specific Z3 number of slots   (inclusive) //  l24 ->22  ds15 ~> 22
#define CAP_LEVEL 20 // level where cap counter are maintaned

// subtree invariant
// #define ROW_BUFF_SIZE 1024 // size of row buffer in terms of bytes ~~~> used for subtree address translation
#define ROW_BUFF_SIZE 8192 // size of row buffer in terms of bytes ~~~> used for subtree address translation
#define NUM_CHANNELS_SUBTREE 4  // # memory channel used for subtree calculation
#define CACHE_LINE_SIZE 128      // cache line size in bytes used for subtree calculation

// rho invariant
#define RHO_STASH_SIZE 200  // size of rho stash
#define RHO_LEVEL 19    // # levels in rho
#define RHO_Z 2  // # slots per bucket in rho
#define RHO_OV_THRESHOLD   RHO_STASH_SIZE - RHO_Z*(RHO_LEVEL+1)   // overflow threshold for background eviction; C - Z(L+1)
// #define RHO_BK_EVICTION 0   // 0/1 flag to disable/enable background eviction in rho
#define RHO_L1 9  // upto L1 level buckts have specific Z1 number of slots   (inclusive)
#define RHO_L2 12   // upto L2 level buckts have specific Z2 number of slots   (inclusive)
#define RHO_L3 14   // upto L3 level buckts have specific Z3 number of slots   (inclusive)
#define RHO_Z1 2   // # slots per bucket upto L1
#define RHO_Z2 2   // # slots per bucket upto L2
#define RHO_Z3 2   // # slots per bucket upto L3
// #define RHO_EMPTY_TOP 0   // # top empty levels of rho ~~~> equivalent to L1 = EMPTY_TOP-1, Z1 = 0 for ------  valcano: 10  freecursive: 0
// #define RHO_TOP_CACHE 6   // # top levels that are cached in rho
#define RHO_WAY 10   // # ways in each set accociative entry of rho tag array


// timing channel security 
// #define TIMING_INTERVAL 100   // # cycles after each one oram access is initiated either real or dummy one
// #define T1_INTERVAL 1000   // # cycles after each one oram access is initiated either real or dummy one
// #define T2_INTERVAL 100   // # cycles after each one oram access is initiated either real or dummy one


// prefetching config
#define PREFETCH_BUF_SIZE 32
#define PREFETCH_TYPE COMBO
#define PREFETCH_INPLACE 1
#define PREFETCH_STRIDE 1

// early eviction config


// ring oram invariant
// #define RING_A 5
#define RING_S 7
#define RING_Z 5
#define INT_BITS LEVEL - 1 
#define RING_REV 512
// #define EP_TURN 2
#define SL1 L1
#define SL2 L2
#define SL3 L3
// #define SKIP_LIMIT 70
// #define SKIP_L1 10
// #define SKIP_L2 14
// #define DUMMY_TH 100
// #define DEP_TH 5             // threshold on # reshuffle for dynamic ep 
// #define WL_CAP 100000        // cap on wl feature
#define STALE_CAP 56          // cap on stale for each 5 levels each of which is 64 bit
#define STALE_BUF_SIZE 300   //size of stale buffer
#define GL_COUNT 3          // # gathering levels
#define GLMAX 15          // last level gl

// #define DD_SATURATE 30000000
#define DD_SATURATE 0

#define NCORES 1

#define SUP_HORIZ_MAX 8

#define MAX_SHUF 20

#define RING_ZSTL 0   // to apply IR-Alloc on Ring ORAM

#define LOG_ENABLE 0 // to print log of all accesses requests and types
#define LOG_TH 39000000 // loging threshold


#define SIT_ENABLE 0 // SGX Integrity Tree enable
#define SIT_ARITY 8 // SGX Integrity Tree arity
#define SIT_LEVEL 9 // SGX Integrity Tree levels

#define BIGBUCK 0

#define PCM_TH 150

#define DUP_ENABLE 0
#define DUP_MAX 2

#define DUPACT_ENABLE 0

#define POS_SKIP 0

#define ACCDIST 101

#define POSBLK_BYTE 4


#define RESETDIST 101

#define UINT2_MAX 3
#define UINT4_MAX 15
#define UINT6_MAX 63 
#define UINT9_MAX 511
#define UINT12_MAX 4095
#define UINT11_MAX 2047
// #define UINT8_MAX 255
#define UINT5_MAX 31

#define UINT15_MAX 32767
#define UINT7_MAX 127
#define UINT10_MAX 1023

#define POSACT_ENABLE 1


enum{
  // main tree
  Z = (RING_ENABLE) ? RING_Z + RING_S + BIGBUCK : USUAL_Z,
  EMPTY_TOP = (VOLCANO_ENABLE || STT_ENABLE) ? 10 : 0,
  Z1 = (VOLCANO_ENABLE || STT_ENABLE) ? 0 : (RING_ENABLE && RSTL_ENABLE)? Z-BIGBUCK: Z,   // # slots per bucket upto L1    Z-5
  Z2 = (VOLCANO_ENABLE || STL_ENABLE) ? 2 : (RING_ENABLE && RSTL_ENABLE)? Z-BIGBUCK: Z,   // # slots per bucket upto L2
  Z3 = (VOLCANO_ENABLE || STL_ENABLE) ? 2 : (RING_ENABLE && RSTL_ENABLE)? Z-BIGBUCK: Z,   // # slots per bucket upto L3   Z-10
  Z4 = (RING_ENABLE && !RSTL_ENABLE) ? Z3 : (RING_ENABLE && RSTL_ENABLE)? Z: Z,
  PATH = (long long int)pow(2,LEVEL-1),  // # paths in oram tree
  NODE = (long long int)pow(2,LEVEL)-1,  // # nodes in oram tree
  SLOT = Z1*((long long int)pow(2,L1+1)-1) + Z2*((long long int)pow(2,L2+1)-(long long int)pow(2,L1+1)) + Z3*((long long int)pow(2,L3+1)-(long long int)pow(2,L2+1)) + ((RING_ENABLE)?Z4:Z)*((long long int)pow(2,LEVEL)-(long long int)pow(2,L3+1)),  // # free slots in oram tree
  // BLOCK = (RING_ENABLE) ? (long long int)((RING_Z*SLOT*U)/Z):((long long int)floor(U*(Z1*((long long int)pow(2,L1+1)-1) + Z2*((long long int)pow(2,L2+1)-(long long int)pow(2,L1+1)) + Z3*((long long int)pow(2,L3+1)-(long long int)pow(2,L2+1)) + Z*((long long int)pow(2,LEVEL)-(long long int)pow(2,L3+1))))),  // # valid blocks in oram tree
  // BLOCK = (RING_ENABLE) ? 33260542*((long long int)pow(2,LEVEL-24)) : (long long int)NODE*((long long int)floor(USUAL_Z*U)),
  // BLOCK = (RING_ENABLE) ? 41943037*((long long int)pow(2,LEVEL-24)) : (long long int)NODE*((long long int)floor(USUAL_Z*U)),
  // BLOCK = (RING_ENABLE) ? 2621437*((long long int)pow(2,LEVEL-20)) : (long long int)NODE*((long long int)floor(USUAL_Z*U)),
  // BLOCK = (RING_ENABLE) ? 1310718 : (long long int)NODE*((long long int)floor(USUAL_Z*U)),
  // BLOCK = (unsigned long long int)pow(SIT_ARITY, SIT_LEVEL-1),
  // BLOCK = 33260542*((long long int)pow(2,LEVEL-24)),
  // BLOCK = 16777215, 
  // BLOCK = 10485759, 
  // BLOCK = 335544316, 
  // BLOCK = 2621437, 
  BLOCK_ORG = (POSACT_ENABLE) ? (long long int)(USUAL_Z*U*NODE) :(long long int)(RING_Z*U*NODE),
  BLOCK = (POSACT_ENABLE) ? (long long int)(BLOCK_ORG * (1 - pow(1.0 / X, H)) / (1.0 - 1.0 / X)) : BLOCK_ORG, 
  CAP_NODE = (int)pow(2,CAP_LEVEL), // # nodes at first non-empty level of tree (L1+1) in oram tree
  STASH_SIZE = (VOLCANO_ENABLE /*|| STT_ENABLE*/) ? (int) (STASH_SIZE_ORG + (pow(2,EMPTY_TOP)-1)*Z) : STASH_SIZE_ORG,
  OV_THRESHOLD = STASH_SIZE - Z*(LEVEL+1),   // overflow threshold for background eviction; C - Z(L+1)
  // subtree scheme
  SUBTREE_SIZE = (int) ROW_BUFF_SIZE * NUM_CHANNELS_SUBTREE,  // size of each 2k-arry tree that forms a node in bytes
  SUBTREE_SLOT = (int) (SUBTREE_SIZE/CACHE_LINE_SIZE),    // # slots that subtree holds
  SUBTREE_BUCKET = (unsigned int) (SUBTREE_SLOT/Z) - 1 ,                // # buckets per subtree given each bucket holds Z slots
  SUBTREE_LEVEL = (unsigned int) ceil(log(SUBTREE_BUCKET)/log(2)),      // # levels of each subtree ~~~> i.e. k
  // rho tree
  RHO_PATH  = (long long int)pow(2,RHO_LEVEL-1),   // # paths in rho
  RHO_NODE = (long long int)pow(2,RHO_LEVEL)-1,    // # nodes in rho
  RHO_SLOT = RHO_Z1*((long long int)pow(2,RHO_L1+1)-1) + RHO_Z2*((long long int)pow(2,RHO_L2+1)-(long long int)pow(2,RHO_L1+1)) + RHO_Z3*((long long int)pow(2,RHO_L3+1)-(long long int)pow(2,RHO_L2+1)) + RHO_Z*((long long int)pow(2,RHO_LEVEL)-(long long int)pow(2,RHO_L3+1)),  // # free slots in rho
  RHO_BLOCK = (int)((long long int)floor(U*(RHO_Z1*((long long int)pow(2,RHO_L1+1)-1) + RHO_Z2*((long long int)pow(2,RHO_L2+1)-(long long int)pow(2,RHO_L1+1)) + RHO_Z3*((long long int)pow(2,RHO_L3+1)-(long long int)pow(2,RHO_L2+1)) + RHO_Z*((long long int)pow(2,RHO_LEVEL)-(long long int)pow(2,RHO_L3+1))))),  // # valid blocks in rho
  // RHO_SET = (int) ceil(RHO_BLOCK/10),
  RHO_SET = 16000,
  // ring oram
  S1 = (RING_ENABLE && RSTL_ENABLE) ? Z1-RING_Z : RING_S,   // # dummy slots per bucket upto SL1
  S2 = (RING_ENABLE && RSTL_ENABLE && !RING_ZSTL) ? Z2-RING_Z : RING_S,   // # dummy slots per bucket upto SL2
  S3 = (RING_ENABLE && RSTL_ENABLE && !RING_ZSTL) ? Z3-RING_Z : RING_S,   // # dummy slots per bucket upto SL3
  S4 = (RING_ENABLE && RSTL_ENABLE)? Z4-RING_Z: S3,
  DATA_ADDR_SPACE = SLOT,
  META_ADDR_SPACE = NODE,
  DEADQ_TH = Z*500,
  // SIT_LEVEL = (int) LEVEL/(int) (log(SIT_ARITY)/log(2)),
  SIT_LEAF = (long long int) pow(SIT_ARITY,SIT_LEVEL-1),
  SIT_NODE = (long long int)(pow(SIT_ARITY,SIT_LEVEL)-1)/(long long int)(pow(SIT_ARITY,1)-1),
  DUP_BLK = BLOCK/DUP_MAX, // number of unique blocks when dup feature is enabled
  // STALE_TH = STALE_BUF_SIZE - (GL_COUNT*STALE_CAP+1),
  NOUNCE_MAX = UINT11_MAX,
  WITHIN_CTR_MAX = X - 1,
  PATHID_CTR_MAX = UINT8_MAX,
  NOUNCE_WIDTH = (int) log2(NOUNCE_MAX) + 1,
  WITHIN_CTR_WIDTH = (int) log2(WITHIN_CTR_MAX) + 1,
  PATHID_CTR_WIDTH = (int) log2(PATHID_CTR_MAX) + 1,
};



typedef enum {ORAM, RHO} TreeType;
typedef enum {REGULAR, EVICT} AccessType;
typedef enum {TAIL, HEAD} EnqueueType;
typedef enum {POS1, POS2} PosType;
typedef enum {STRIDE_BASED, HISTORY_BASED, COMBO} PrefetchType;
typedef enum {REFRESHED, REMEMBERED, ALLOCATED, DEAD} DeadState;
typedef enum {SNAP, TMP, ALL, NONE} PrintType;


typedef struct Slot Slot;


#include <stdbool.h>

typedef struct Element_t{
  long long int addr;
  long long int cycle; 
  int thread; 
  int instr; 
  long long int pc;
  char type;
  int oramid;
  TreeType tree;
  bool last_read;
  long long int orig_addr;
  struct Element_t *prev; 
  DeadState dd;
  int index;
  int offset;
  bool nvm_access;
  bool beginning;
  bool ending;
  char op_type;
  int reqid;
  int level;
  int dtser;
}Element;


typedef struct Queue {
    Element *head;
    Element *tail;
    int size;
    int limit;
} Queue;


// extern long long int CYCLE_VAL; 
extern long long int online_time;
extern long long int evict_time;
extern long long int reshuff_time;

extern Queue *deadQ_arr[LEVEL];
extern Queue *oramQ;
extern Queue *deadQ;
extern Queue *plbQ; 
extern unsigned long long int tracectr; 
extern int tracectr_test; 
extern int invokectr; 
extern int prefetchctr; 
extern int bkctr; 
extern int rho_bkctr; 
extern int stash_dist[STASH_SIZE+1];
extern int rho_stash_dist[RHO_STASH_SIZE+1];
extern int oramctr; 
extern int rhoctr; 
extern int rho_hit; 
extern bool write_cache_hit;   // flag to be effective if write bypass is enabled
extern int dummyctr;
extern int rho_dummyctr;
extern int curr_trace;
extern int next_trace;
extern int pos1ctr;
extern int pos2ctr;
extern int pos1acc_ctr;
extern int pos2acc_ctr;
extern int pos1hit;
extern int pos2hit;
extern int pos1conf;
extern int pos2conf;
extern long long int plb_hit[H-1];
extern long long int plbaccess[H-1];
extern int case1;
extern int case2;
extern int case3;
extern int plbpos1;
extern int stashpos1;
extern int bufferpos1;
extern int curr_page;
extern long long int curr_pc;
extern char curr_offset;
extern unsigned long long int curr_footprint;
extern int earlyctr;
extern int evictctr;
extern int dirty_pointctr;
extern int pos1_access;
extern int pos2_access;
extern int ptr_fail;
extern int search_fail;
extern int precase;
extern int sttctr;
extern int stashctr;
extern int ring_evictctr;
extern bool ring_dummy;
extern bool dirty_evict;
extern bool SIM_ENABLE_VAR;
extern bool CACHE_ENABLE_VAR;

extern long long int topctr;
extern long long int midctr;
extern long long int botctr;


extern long long int CYCLE_VAL;
extern long long int cache_clk;
extern int stash_leftover;
extern int stash_removed;
extern int fillhit;
extern int fillmiss;

double exe_time;
extern int missctr;
extern int rmiss;
extern int wmiss;
extern int hitctr;
extern long long int mem_clk;


long long int mem_req_start;
long long int mem_req_latencies;


extern int PosMap[BLOCK];  

extern int oram_effective_pl;
extern int TIMING_INTERVAL;

static const int LZ[LEVEL] = {[0 ... L1] = Z1, [L1+1 ... L2] = Z2, [L2+1 ... L3] = Z3, [L3+1 ... LEVEL-1] = Z4};  // array of different Z for different levels in oram
// static const int LZ[LEVEL] = {Z, Z, Z, Z, Z, Z, Z, Z, Z, Z, Z3, Z3, Z2, Z2, Z2, Z2, Z2, Z2, Z2, Z3, Z3, Z, Z, Z, Z, Z};  // array of different Z for different levels in oram
static const int RHO_LZ[RHO_LEVEL] = {[0 ... RHO_L1] = RHO_Z1, [RHO_L1+1 ... RHO_L2] = RHO_Z2, [RHO_L2+1 ... RHO_L3] = RHO_Z3, [RHO_L3+1 ... RHO_LEVEL-1] = RHO_Z};  // array of different Z for different levels in rho
static const int LS[LEVEL] = {[0 ... SL1] = S1, [SL1+1 ... SL2] = S2, [SL2+1 ... SL3] = S3, [SL3+1 ... LEVEL-1] = S4};  // array of different S for different levels in  ring oram

static const int GL[GL_COUNT] = {5, 10, GLMAX};  // array of different Z for different levels in oram
static const int GL_CAP[GL_COUNT] = {56, 40, 8};  // array of different Z for different levels in oram
// static const int GL[GL_COUNT] = {1};  // array of different Z for different levels in oram

enum {META_MAX_SIZE = (int)pow(2, GLMAX)};

extern bool last_read_served;
extern bool last_read_deleted;
extern bool last_req_served;
extern long long int nonmemops_sum;
extern long long int nonmemops_executed;
extern unsigned long long int nonmemops_trace;
 
extern long long int missl1wb;
extern char bench[20];
extern char exp_name[20];
extern char *pargv[5];

extern int shuff_interval[LEVEL];

extern unsigned long long int NVM_ADDR_BYTE;
extern unsigned long long int NVM_ADDR_VAR;

extern unsigned long long int online_wait_dram;
extern unsigned long long int online_wait_nvm;
extern unsigned long long int evict_wait_dram;
extern unsigned long long int evict_wait_nvm;
extern unsigned long long int reshuffle_wait_dram;
extern unsigned long long int reshuffle_wait_nvm;
extern unsigned long long int meta_wait_dram;
extern unsigned long long int meta_wait_nvm;
extern unsigned long long int online_t0;
extern unsigned long long int cur_online;
extern unsigned long long int evict_t0;
extern unsigned long long int cur_evict;
extern unsigned long long int reshuffle_t0;
extern unsigned long long int cur_reshuffle;
extern unsigned long long int meta_t0;
extern unsigned long long int cur_meta;

extern int cur_dram_served;
extern int cur_nvm_served;

extern int determineReq;
extern int detnvm;
extern int detdram;
extern long long int determineCycle;
extern long long int comptime_max;
extern int longest_req;
extern long long int nvm_tmax;
extern long long int dram_tmax;
extern bool nvmt0_set;
extern int lrs_ctr;

// extern int dram_to_serve_e_w;
// extern int nvm_to_serve_e_w;


extern int cur_dram_served_o;
extern int cur_nvm_served_o; 
extern int cur_dram_served_e_r;
extern int cur_dram_served_e_w;
extern int cur_nvm_served_e_r;
extern int cur_nvm_served_e_w;
extern int cur_dram_served_m_r;
extern int cur_dram_served_m_w;
extern int cur_nvm_served_m_r;
extern int cur_nvm_served_m_w;
extern int cur_dram_served_r_r;
extern int cur_dram_served_r_w;
extern int cur_nvm_served_r_r;
extern int cur_nvm_served_r_w;

extern int dram_to_serve_o;
extern int nvm_to_serve_o;
extern int dram_to_serve_e_r;
extern int dram_to_serve_e_w;
extern int nvm_to_serve_e_r;
extern int nvm_to_serve_e_w;
extern int dram_to_serve_m_r;
extern int dram_to_serve_m_w;
extern int nvm_to_serve_m_r;
extern int nvm_to_serve_m_w;
extern int dram_to_serve_r_r;
extern int dram_to_serve_r_w;
extern int nvm_to_serve_r_r;
extern int nvm_to_serve_r_w;

extern unsigned long long int rctr;
extern unsigned long long int wctr;

// extern int TraceCount[BLOCK]; 
// extern unsigned long long int trace_dist[ACCDIST];



void oram_alloc();
void oram_init();
void test_init();
void print_path(int label);
void background_eviction();
void count_tree();
void init_trace();
void print_count_level();
void take_snapshot(char * argv[]);
void read_path(int label);
void write_path(int label);
void remap_block(int addr);
int add_to_stash(Slot s);
void remove_from_stash(int index);
void test_oram(char * argv[]);
void freecursive_access(int addr, char type);
int get_stash(int addr);
bool stash_contain(int addr);
void invoke_oram(long long int physical_address, long long int arrival_time, int thread_id, int instruction_id, long long int instruction_pc, char type);
int  calc_index(int label, int l);
int calc_level(int index);
void print_tree();
void print_stash();
int assign_a_path(int addr);
void print_cap_percent();
int concat(int a, int b);
int digcount(int num);
int index_to_addr(int index);
void rho_alloc();
void rho_access(int addr, int label);
int rho_lookup(int addr);
void rho_update_tag_array(int addr, int label);
void rho_insert(int physical_address);
Queue *ConstructQueue(int limit);
void DestructQueue(Queue *queue);
bool Enqueue(Queue *pQueue, Element *item);
bool EnqueueHead(Queue *pQueue, Element *item);
Element *Dequeue(Queue *pQueue);
bool isEmpty(Queue* pQueue);
void test_queue();
void insert_oramQ(long long int addr, long long int cycle, int thread, int instr, long long int pc, char type, bool last_read, bool nvm_access, char op_type, bool beginning, bool ending, int level);
void test_subtree();
void dummy_access(TreeType tree);
void switch_enqueue_to(EnqueueType enqueue);
void print_plb_stat();
// bool plb_contain(int tag);
void prefetch_access(int addr);
void invoke_prefetch();
void insert_buffer(int addr);
bool buffer_contain(int addr);
int buffer_index(int addr);
// unsigned int block_addr(long long int physical_addr);
unsigned int block_addr(unsigned long long int caddr);
unsigned int page_addr(unsigned int physical_addr);
char offset_val(unsigned int addr);
void footprint_update(unsigned int addr);
void test_footprint();
void early_writeback();
void reset_dirty_search();
unsigned long long int byte_addr(long long int physical_addr);
void handle_sigint(int sig);
bool bk_evict_needed();
int pos_calc(int addr, int degree);
void print_oram_stats();
void switch_sim_enable_to(bool tf);
void oram_init_path();
void free_stash();
void ring_access(int addr);
void ring_read_path(int label, int addr);
void ring_evict_path(int label);
void ring_early_reshuffle(int label);
void ring_invalidate(int index, int offset);
int reverse_lex(int n);
void print_shuff_stat();
void export_csv(char * argv[]);
int add_stale_buf(Slot s);
void remove_stale_buf(int index);
int get_stale_buf(int addr);
int gl_index(int index, int h);
void stale_access(int index, int h, char type);
void var_init();
void gather_dead(int index, int i);
void remote_invalidate(int index, int offset);
int calc_mem_addr(int index, int offset, char type);
int remote_allocate(int index, int offset);
bool remove_dead(Queue *pQueue, int key1, int key2);
void reset_shuff_interval();
void print_array(unsigned long long int * arr, int size, FILE *fp, char* str);
void export_intermed(char * exp_name, unsigned long long int ind, long double *arr, unsigned long long int suffix);
void record_util_level();
void reset_util_level();
void print_array_double(long double * arr, int size, FILE *fp, char *str);
bool in_nvm(int level);
bool is_nvm_addr(int addr);
void switch_cache_enable_to(bool tf);
void random_test(char * argv[]);
void bucket_meta_access(int index);
void random_trace();
void explore_lz();
void test_ring();
void switch_dead_enable_to(bool tf);
void update_count_stat(int count, int level);
void update_ddr_timing_param(int channel);
void reset_profile_counters();
void calc_wait_value(char op_type, int reqid, long long int comptime, int oramid);
long long int sum_wait_sofar();
void set_reshuffle_w(int level);
void adjust_ddr(long long int addr);
void print_super_node(int * arr, char * exp, char * bench, char * realdum);
int  calc_super(int label,  int l);
int  calc_tri(int label,  int l);
int calc_horiz(int index, int label, int i);
int calc_ring_s(int index, int level);
int write_bucket(int index, int label, int level, char op_type, bool first_super);
int detect_inplace_available(int index, int level);
bool is_super_level(int level);
int calc_super_in_tree(int index);
void read_bucket(int index, int i, char op_type, int residue, bool first_super);
int decide_which_super(int index, int i, int addr);
int count_bucket_dumvalid(int index, int i);
int calc_super_path(int label, int level, int index);
int calc_space(const int * lz);
int sit_level(int index);
int sit_index(int label,  int l);
void sit_access(unsigned long long int addr);
void sit_count();
void sit_init();
int sit_index_mid(int label,  int l);
int accgap_index_mid(int label,  int l);
void accgap_count();
uint32_t merkle_addr(int addr, int i);
uint8_t merkle_offset(int addr, int i);
// Mehrnoosh.























// Moved here from main.c 
long long int *committed; // total committed instructions in each core
long long int *fetched;   // total fetched instructions in each core


//////////////////////////////////////////////////
//	Memory Controller Data Structures	//
//////////////////////////////////////////////////

// DRAM Address Structure
typedef struct draddr
{
  long long int actual_address; // physical_address being accessed
  int channel;	// channel id
  int rank;	// rank id
  int bank;	// bank id
  long long int row;	// row/page id
  int column;	// column id
} dram_address_t;

// DRAM Commands 
typedef enum {ACT_CMD, COL_READ_CMD, PRE_CMD, COL_WRITE_CMD, PWR_DN_SLOW_CMD, PWR_DN_FAST_CMD, PWR_UP_CMD, REF_CMD, NOP} command_t; 

// Request Types
typedef enum {READ, WRITE} optype_t;

// Single request structure self-explanatory
typedef struct req
{
  unsigned long long int physical_address;
  dram_address_t dram_addr;
  long long int arrival_time;     
  long long int dispatch_time; // when COL_RD or COL_WR is issued for this request
  long long int completion_time; //final completion time
  long long int latency; // dispatch_time-arrival_time
  int thread_id; // core that issued this request
  command_t next_command; // what command needs to be issued to make forward progress with this request
  int command_issuable; // can this request be issued in the current cycle
  optype_t operation_type; // Read/Write
  int request_served; // if request has it's final command issued or not
  int instruction_id; // 0 to ROBSIZE-1
  long long int instruction_pc; // phy address of instruction that generated this request (valid only for reads)
  void * user_ptr; // user_specified data
  struct req * next;
  // Mehrnoosh:
  int oramid;       // oram access id to whcih the request belongs to
  TreeType tree;    // which tree the request belongs to oram or rho
  bool last_read;   // 0/1 flag to indicate whether the request is the last read request in oram read phase
  bool nvm_access;   // 0/1 flag to indicate whether the request is a nvm block access
  bool beginning;
  bool ending;
  char op_type;
  bool last_req;
  int reqid;
  int countdown;

  // Mehrnoosh.
} request_t;

// Bankstates
typedef enum 
{
  IDLE, PRECHARGING, REFRESHING, ROW_ACTIVE, PRECHARGE_POWER_DOWN_FAST, PRECHARGE_POWER_DOWN_SLOW, ACTIVE_POWER_DOWN
} bankstate_t;

// Structure to hold the state of a bank
typedef struct bnk
{
  bankstate_t state;
  long long int active_row;
  long long int next_pre;
  long long int next_act;
  long long int next_read;
  long long int next_write;
  long long int next_powerdown;
  long long int next_powerup;
  long long int next_refresh;
}bank_t;

// contains the states of all banks in the system 
bank_t dram_state[MAX_NUM_CHANNELS][MAX_NUM_RANKS][MAX_NUM_BANKS];

// command issued this cycle to this channel
int command_issued_current_cycle[MAX_NUM_CHANNELS];

// cas command issued this cycle to this channel
int cas_issued_current_cycle[MAX_NUM_CHANNELS][MAX_NUM_RANKS][MAX_NUM_BANKS]; // 1/2 for COL_READ/COL_WRITE

// Per channel read queue
request_t * read_queue_head[MAX_NUM_CHANNELS];

// Per channel write queue
request_t * write_queue_head[MAX_NUM_CHANNELS];

// issuables_for_different commands
int cmd_precharge_issuable[MAX_NUM_CHANNELS][MAX_NUM_RANKS][MAX_NUM_BANKS];
int cmd_all_bank_precharge_issuable[MAX_NUM_CHANNELS][MAX_NUM_RANKS];
int cmd_powerdown_fast_issuable[MAX_NUM_CHANNELS][MAX_NUM_RANKS];
int cmd_powerdown_slow_issuable[MAX_NUM_CHANNELS][MAX_NUM_RANKS];
int cmd_powerup_issuable[MAX_NUM_CHANNELS][MAX_NUM_RANKS];
int cmd_refresh_issuable[MAX_NUM_CHANNELS][MAX_NUM_RANKS];


// refresh variables
long long int next_refresh_completion_deadline[MAX_NUM_CHANNELS][MAX_NUM_RANKS];
long long int last_refresh_completion_deadline[MAX_NUM_CHANNELS][MAX_NUM_RANKS];
int forced_refresh_mode_on[MAX_NUM_CHANNELS][MAX_NUM_RANKS];
int refresh_issue_deadline[MAX_NUM_CHANNELS][MAX_NUM_RANKS];
int issued_forced_refresh_commands[MAX_NUM_CHANNELS][MAX_NUM_RANKS];
int num_issued_refreshes[MAX_NUM_CHANNELS][MAX_NUM_RANKS];

long long int read_queue_length[MAX_NUM_CHANNELS];
long long int write_queue_length[MAX_NUM_CHANNELS];

// Stats
long long int num_read_merge ;
long long int num_write_merge ;
long long int stats_reads_merged_per_channel[MAX_NUM_CHANNELS];
long long int stats_writes_merged_per_channel[MAX_NUM_CHANNELS];
long long int stats_reads_seen[MAX_NUM_CHANNELS];
long long int stats_writes_seen[MAX_NUM_CHANNELS];
long long int stats_reads_completed[MAX_NUM_CHANNELS];
long long int stats_writes_completed[MAX_NUM_CHANNELS];

double stats_average_read_latency[MAX_NUM_CHANNELS];
double stats_average_read_queue_latency[MAX_NUM_CHANNELS];
double stats_average_write_latency[MAX_NUM_CHANNELS];
double stats_average_write_queue_latency[MAX_NUM_CHANNELS];

long long int stats_page_hits[MAX_NUM_CHANNELS];
double stats_read_row_hit_rate[MAX_NUM_CHANNELS];

// Time spent in various states
long long int stats_time_spent_in_active_standby[MAX_NUM_CHANNELS][MAX_NUM_RANKS];
long long int stats_time_spent_in_active_power_down[MAX_NUM_CHANNELS][MAX_NUM_RANKS];
long long int stats_time_spent_in_precharge_power_down_fast[MAX_NUM_CHANNELS][MAX_NUM_RANKS];
long long int stats_time_spent_in_precharge_power_down_slow[MAX_NUM_CHANNELS][MAX_NUM_RANKS];
long long int stats_time_spent_in_power_up[MAX_NUM_CHANNELS][MAX_NUM_RANKS];
long long int last_activate[MAX_NUM_CHANNELS][MAX_NUM_RANKS];
long long int last_refresh[MAX_NUM_CHANNELS][MAX_NUM_RANKS];
double average_gap_between_activates[MAX_NUM_CHANNELS][MAX_NUM_RANKS];
double average_gap_between_refreshes[MAX_NUM_CHANNELS][MAX_NUM_RANKS];
long long int stats_time_spent_terminating_reads_from_other_ranks[MAX_NUM_CHANNELS][MAX_NUM_RANKS];
long long int stats_time_spent_terminating_writes_to_other_ranks[MAX_NUM_CHANNELS][MAX_NUM_RANKS];

// Command Counters
long long int stats_num_activate_read[MAX_NUM_CHANNELS][MAX_NUM_RANKS][MAX_NUM_BANKS];
long long int stats_num_activate_write[MAX_NUM_CHANNELS][MAX_NUM_RANKS][MAX_NUM_BANKS];
long long int stats_num_activate_spec[MAX_NUM_CHANNELS][MAX_NUM_RANKS][MAX_NUM_BANKS];
long long int stats_num_activate[MAX_NUM_CHANNELS][MAX_NUM_RANKS];
long long int stats_num_precharge[MAX_NUM_CHANNELS][MAX_NUM_RANKS][MAX_NUM_BANKS];
long long int stats_num_read[MAX_NUM_CHANNELS][MAX_NUM_RANKS][MAX_NUM_BANKS];
long long int stats_num_write[MAX_NUM_CHANNELS][MAX_NUM_RANKS][MAX_NUM_BANKS];
long long int stats_num_powerdown_slow[MAX_NUM_CHANNELS][MAX_NUM_RANKS];
long long int stats_num_powerdown_fast[MAX_NUM_CHANNELS][MAX_NUM_RANKS];
long long int stats_num_powerup[MAX_NUM_CHANNELS][MAX_NUM_RANKS];



// functions

// to get log with base 2
unsigned int log_base2(unsigned int new_value);

// initialize memory_controller variables
void init_memory_controller_vars();

// called every cycle to update the read/write queues
void update_memory();

// activate to bank allowed or not
int is_activate_allowed(int channel, int rank, int bank);

// precharge to bank allowed or not
int is_precharge_allowed(int channel, int rank, int bank);

// all bank precharge allowed or not
int is_all_bank_precharge_allowed(int channel, int rank);

// autoprecharge allowed or not
int is_autoprecharge_allowed(int channel,int rank,int bank);

// power_down fast allowed or not
int is_powerdown_fast_allowed(int channel,int rank);

// power_down slow allowed or not
int is_powerdown_slow_allowed(int channel,int rank);

// powerup allowed or not
int is_powerup_allowed(int channel,int rank);

// refresh allowed or not
int is_refresh_allowed(int channel,int rank);


// issues command to make progress on a request
int issue_request_command(request_t * req, char rwt);

// power_down command
int issue_powerdown_command(int channel, int rank, command_t cmd);

// powerup command
int issue_powerup_command(int channel, int rank);

// precharge a bank
int issue_activate_command(int channel, int rank, int bank, long long int row);

// precharge a bank
int issue_precharge_command(int channel, int rank, int bank);

// precharge all banks in a rank
int issue_all_bank_precharge_command(int channel, int rank);

// refresh all banks
int issue_refresh_command(int channel, int rank);

// autoprecharge all banks
int issue_autoprecharge(int channel, int rank, int bank);

// find if there is a matching write request
int read_matches_write_or_read_queue(long long int physical_address);

// find if there is a matching request in the write queue
int write_exists_in_write_queue(long long int physical_address);

// enqueue a read into the corresponding read queue (returns ptr to new node)
request_t* insert_read(long long int physical_address, long long int arrival_cycle, int thread_id, int instruction_id, long long int instruction_pc, int oramid, TreeType tree, bool last_read, bool nvm_access, char op_type, bool beginning, bool ending, bool last_req, int reqid);

// enqueue a write into the corresponding write queue (returns ptr to new_node)
request_t* insert_write(long long int physical_address, long long int arrival_time, int thread_id, int instruction_id, int oramid, TreeType tree, bool nvm_access, char op_type, bool beginning, bool ending, bool last_req, bool last_read, int reqid);

// update stats counters
void gather_stats(int channel);

// print statistics
void print_stats();

// calculate power for each channel
float calculate_power(int channel, int rank, int print_stats_type, int chips_per_rank);


// Mehrnoosh:
request_t * form_request(int operation_type, int oramid, char op_type, int reqid, bool nvm_access, long long int completion_time);
void determine_served_all(request_t * request);
void update_served_count(request_t * request);
// Mehrnoosh.



#endif // __MEM_CONTROLLER_HH__
