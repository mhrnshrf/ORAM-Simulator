#ifndef __MEMORY_CONTROLLER_H__
#define __MEMORY_CONTROLLER_H__

#define MAX_NUM_CHANNELS 16
#define MAX_NUM_RANKS 16
#define MAX_NUM_BANKS 32

// Mehrnoosh:



#include <math.h>
// other simulation parameter
#define TRACE_SIZE 4000000          // # addr read from trace file
#define QUEUE_SIZE 2000             // oramq capacity
#define PAGE_SIZE 4096              // page size in byte ~~~> 4KB
#define L1_LATENCY 3                // L1 latency in terms of # cycles 
#define L2_LATENCY 10               // L2 latency in terms of # cycles 
#define MAINMEM_LATENCY 0           // L2 latency in terms of # cycles 
#define WARMUP_THRESHOLD 3000000    // L2 warm up threshold, after which stats are gathered and memory accesses are actully made
#define TIMEOUT_THRESHOLD 10000      // time out threshold in seconds
#define TOP_BOUNDRY 10              // top region tree boundry
#define MID_BOUNDRY 20              // middle region tree boundry

// enable/disable options config
#define TIMEOUT_ENBALE  1     // 0/1 flag to disable/enable finishing the program in case it get stuck
#define SUBTREE_ENABLE  0     // 0/1 flag to diable/enable having subtree adddressing scheme
#define CACHE_ENABLE    1     // 0/1 flag to diable/enable having cache
#define VOLCANO_ENABLE  0     // 0/1 flag to disable/enable having volcano idea both stt and stl
#define STT_ENABLE      0     // 0/1 flag to disable/enable stash top tree  ~> it won't matter if volcano is enabled
#define STL_ENABLE      0     // 0/1 flag to disable/enable slim tree level ~> it won't matter if volcano is enabled
#define WRITE_BYPASS    0     // 0/1 flag to disable/enable cacheing the path id along the data in the LLC which will benefit write reqs to bypass posmap lookup 
#define RHO_ENABLE      0     // 0/1 flag to disable/enable having rho
#define TIMING_ENABLE   0     // 0/1 flag to disable/enable having timing channel security
#define PREFETCH_ENABLE 0     // 0/1 flag to disable/enable having prefetching option in case of having timing channel security
#define EARLY_ENABLE    0     // 0/1 flag to disable/enable early eviction option in case of having timing channel security
#define SNAPSHOT_ENABLE 0     // 0/1 flag to disable/enable performing snapshot by making path oram accesses

#define RING_ENABLE     1     // 0/1 flag to disable/enable ring oram (instead of path oram)
#define RAND_ENABLE     0     // 0/1 flag to disable/enable rand address instead of trace addr
#define WRITE_LINGER    0     // 0/1 flag to disable/enable write linger feature for ring oram
#define RSTL_ENABLE     0     // 0/1 flag to disable/enable write stl feature for ring oram



// oram config
#define H 4     // degree of recursion including data access
#define X 16    // # label per posmap block
#define LEVEL 24 // # levels
#define Z 12     // # slots per bucket
#define U 0.50 // utilization
#define RL 6     // # the reserved level
#define STASH_SIZE_ORG 200     // original size of stash
#define BK_EVICTION 0   // 0/1 flag to disable/enable background eviction
#define TOP_CACHE 10   // # top levels that are cached ---------- freecursive: 10, volcano: don't care
#define L1 9   // upto L1 level buckts have specific Z1 number of slots   (inclusive)
#define L2 17   // upto L2 level buckts have specific Z2 number of slots   (inclusive)
#define L3 22   // upto L3 level buckts have specific Z3 number of slots   (inclusive)
#define CAP_LEVEL 20 // level where cap counter are maintaned

// subtree config
// #define ROW_BUFF_SIZE 1024 // size of row buffer in terms of bytes ~~~> used for subtree address translation
#define ROW_BUFF_SIZE 8192 // size of row buffer in terms of bytes ~~~> used for subtree address translation
#define NUM_CHANNELS_SUBTREE 4  // # memory channel used for subtree calculation
#define CACHE_LINE_SIZE 64      // cache line size in bytes used for subtree calculation

// rho config
#define RHO_STASH_SIZE 200  // size of rho stash
#define RHO_LEVEL 19    // # levels in rho
#define RHO_Z 2  // # slots per bucket in rho
#define RHO_OV_THRESHOLD   RHO_STASH_SIZE - RHO_Z*(RHO_LEVEL+1)   // overflow threshold for background eviction; C - Z(L+1)
#define RHO_BK_EVICTION 0   // 0/1 flag to disable/enable background eviction in rho
#define RHO_L1 9  // upto L1 level buckts have specific Z1 number of slots   (inclusive)
#define RHO_L2 12   // upto L2 level buckts have specific Z2 number of slots   (inclusive)
#define RHO_L3 14   // upto L3 level buckts have specific Z3 number of slots   (inclusive)
#define RHO_Z1 2   // # slots per bucket upto L1
#define RHO_Z2 2   // # slots per bucket upto L2
#define RHO_Z3 2   // # slots per bucket upto L3
#define RHO_EMPTY_TOP 0   // # top empty levels of rho ~~~> equivalent to L1 = EMPTY_TOP-1, Z1 = 0 for ------  valcano: 10  freecursive: 0
#define RHO_TOP_CACHE 6   // # top levels that are cached in rho
#define RHO_WAY 10   // # ways in each set accociative entry of rho tag array


// timing channel security config
// #define TIMING_INTERVAL 100   // # cycles after each one oram access is initiated either real or dummy one
#define T1_INTERVAL 1000   // # cycles after each one oram access is initiated either real or dummy one
#define T2_INTERVAL 100   // # cycles after each one oram access is initiated either real or dummy one


// prefetching config
#define PREFETCH_INPLACE 1
#define PREFETCH_BUF_SIZE 32
#define PREFETCH_STRIDE 1
#define PREFETCH_TYPE COMBO

// early eviction config


// ring oram config
#define RING_A 5
#define RING_S 7
#define RING_Z 5
#define INT_BITS LEVEL - 1 
#define RING_REV 512
#define EP_TURN 2
#define SL1 L1
#define SL2 L2
#define SL3 L3




enum{
  // main tree
  EMPTY_TOP = (VOLCANO_ENABLE || STT_ENABLE) ? 10 : 0,
  Z1 = (VOLCANO_ENABLE || STT_ENABLE) ? 0 : (RING_ENABLE && RSTL_ENABLE)? Z-2: Z,   // # slots per bucket upto L1
  Z2 = (VOLCANO_ENABLE || STL_ENABLE) ? 2 :(RING_ENABLE && RSTL_ENABLE)? Z: Z,   // # slots per bucket upto L2
  Z3 = (VOLCANO_ENABLE || STL_ENABLE) ? 3 : (RING_ENABLE &&RSTL_ENABLE)? Z-5:Z,   // # slots per bucket upto L3
  Z4 = (RING_ENABLE) ? Z3 : Z,
  PATH = (long long int)pow(2,LEVEL-1),  // # paths in oram tree
  NODE = (long long int)pow(2,LEVEL)-1,  // # nodes in oram tree
  SLOT = Z1*((long long int)pow(2,L1+1)-1) + Z2*((long long int)pow(2,L2+1)-(long long int)pow(2,L1+1)) + Z3*((long long int)pow(2,L3+1)-(long long int)pow(2,L2+1)) + Z*((long long int)pow(2,LEVEL)-(long long int)pow(2,L3+1)),  // # free slots in oram tree
  // BLOCK = (RING_ENABLE) ? (long long int)((RING_Z*SLOT*U)/Z):((long long int)floor(U*(Z1*((long long int)pow(2,L1+1)-1) + Z2*((long long int)pow(2,L2+1)-(long long int)pow(2,L1+1)) + Z3*((long long int)pow(2,L3+1)-(long long int)pow(2,L2+1)) + Z*((long long int)pow(2,LEVEL)-(long long int)pow(2,L3+1))))),  // # valid blocks in oram tree
  BLOCK = 33260542, 
  // BLOCK = 16777215, 
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
  S2 = (RING_ENABLE && RSTL_ENABLE) ? Z2-RING_Z : RING_S,   // # dummy slots per bucket upto SL2
  S3 = (RING_ENABLE && RSTL_ENABLE) ? Z3-RING_Z : RING_S,   // # dummy slots per bucket upto SL3
  S4 = S3,

};



typedef enum {ORAM, RHO} TreeType;
typedef enum {REGULAR, EVICT} AccessType;
typedef enum {TAIL, HEAD} EnqueueType;
typedef enum {POS1, POS2} PosType;
typedef enum {STRIDE_BASED, HISTORY_BASED, COMBO} PrefetchType;


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
  long long int orig_addr;
  struct Element_t *prev; 
}Element;


typedef struct Queue {
    Element *head;
    Element *tail;
    int size;
    int limit;
} Queue;


// extern long long int CYCLE_VAL; 
extern Queue *oramQ;
extern Queue *plbQ; 
extern int tracectr; 
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

extern long long int topctr;
extern long long int midctr;
extern long long int botctr;


extern long long int CYCLE_VAL;
extern long long int cache_clk;
extern int stash_leftover;
extern int stash_removed;
extern int fillhit;
extern int fillmiss;

double cpu_time_used;
extern int missctr;
extern int hitctr;
extern long long int mem_clk;


extern int PosMap[BLOCK];  

extern int oram_effective_pl;

static const int LZ[LEVEL] = {[0 ... L1] = Z1, [L1+1 ... L2] = Z2, [L2+1 ... L3] = Z3, [L3+1 ... LEVEL-1] = Z4};  // array of different Z for different levels in oram
static const int RHO_LZ[RHO_LEVEL] = {[0 ... RHO_L1] = RHO_Z1, [RHO_L1+1 ... RHO_L2] = RHO_Z2, [RHO_L2+1 ... RHO_L3] = RHO_Z3, [RHO_L3+1 ... RHO_LEVEL-1] = RHO_Z};  // array of different Z for different levels in rho
static const int LS[LEVEL] = {[0 ... SL1] = S1, [SL1+1 ... SL2] = S2, [SL2+1 ... SL3] = S3, [SL3+1 ... LEVEL-1] = S4};  // array of different S for different levels in  ring oram





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
Element *Dequeue(Queue *pQueue);
bool isEmpty(Queue* pQueue);
void test_queue();
void insert_oramQ(long long int addr, long long int cycle, int thread, int instr, long long int pc, char type);
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
unsigned int block_addr(unsigned int caddr);
unsigned int page_addr(unsigned int physical_addr);
char offset_val(unsigned int addr);
void footprint_update(unsigned int addr);
void test_footprint();
void early_writeback();
void reset_dirty_search();
unsigned int byte_addr(long long int physical_addr);
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
int issue_request_command(request_t * req);

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
request_t* insert_read(long long int physical_address, long long int arrival_cycle, int thread_id, int instruction_id, long long int instruction_pc, int oramid, TreeType tree);

// enqueue a write into the corresponding write queue (returns ptr to new_node)
request_t* insert_write(long long int physical_address, long long int arrival_time, int thread_id, int instruction_id, int oramid, TreeType tree);

// update stats counters
void gather_stats(int channel);

// print statistics
void print_stats();

// calculate power for each channel
float calculate_power(int channel, int rank, int print_stats_type, int chips_per_rank);



#endif // __MEM_CONTROLLER_HH__
