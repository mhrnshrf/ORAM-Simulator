#ifndef __MEMORY_CONTROLLER_H__
#define __MEMORY_CONTROLLER_H__

#define MAX_NUM_CHANNELS 16
#define MAX_NUM_RANKS 16
#define MAX_NUM_BANKS 32

// Mehrnoosh:



#include <math.h>
// other simulation parameter
#define TRACE_SIZE 1000000  // # addr read from trace file
#define QUEUE_SIZE 1000     // oramq capacity
#define PAGE_SIZE 4096      // page size in byte ~~~> 4KB
#define L1_LATENCY 3        // L1 latency in terms of # cycles 
#define L2_LATENCY 10       // L2 latency in terms of # cycles 
#define MAINMEM_LATENCY 200       // L2 latency in terms of # cycles 

// enable/disable options config
#define VOLCANO_ENABLE 0     // 0/1 flag to disable/enable having volcano idea
#define CACHE_ENABLE 1       // 0/1 flag to diable/enable having cache
#define WRITE_BYPASS 0       // 0/1 flag to disable/enable cacheing the path id along the data in the LLC which will benefit write reqs to bypass posmap lookup 
#define SUBTREE_ENABLE 0     // 0/1 flag to diable/enable having subtree adddressing scheme
#define RHO_ENABLE 0         // 0/1 flag to disable/enable having rho
#define TIMING_ENABLE 1      // 0/1 flag to disable/enable having timing channel security
#define PREFETCH_ENABLE 0    // 0/1 flag to disable/enable having prefetching option in case of having timing channel security

// oram config
#define H 4     // degree of recursion including data access
#define X 16    // # label per posmap block
#define LEVEL 24 // # levels
#define Z 4     // # slots per bucket
#define U 0.50 // utilization
#define RL 6     // # the reserved level
#define STASH_SIZE 200     // size of stash
#define PLB_SIZE 1024     // size of plb (# entry)
#define OV_TRESHOLD   STASH_SIZE - Z*(LEVEL+1)   // overflow threshold for background eviction; C - Z(L+1)
#define BK_EVICTION 1   // 0/1 flag to disable/enable background eviction
#define EMPTY_TOP 0   // # top empty levels ~~~> equivalent to L1 = EMPTY_TOP-1, Z1 = 0 for ------  valcano: 10  freecursive: 0
#define TOP_CACHE 10   // # top levels that are cached ---------- freecursive: 10, volcano: don't care
#define L1 9   // upto L1 level buckts have specific Z1 number of slots   (inclusive)
#define L2 15   // upto L2 level buckts have specific Z2 number of slots   (inclusive)
#define L3 18   // upto L3 level buckts have specific Z3 number of slots   (inclusive)
#define Z1 4   // # slots per bucket upto L1
#define Z2 4   // # slots per bucket upto L2
#define Z3 4   // # slots per bucket upto L3
#define CAP_LEVEL 20 // level where cap counter are maintaned

// subtree config
// #define ROW_BUFF_SIZE 1024 // size of row buffer in terms of bytes ~~~> used for subtree address translation
#define ROW_BUFF_SIZE 8192 // size of row buffer in terms of bytes ~~~> used for subtree address translation
#define NUM_CHANNELS_SUBTREE 1  // # memory channel used for subtree calculation
#define CACHE_LINE_SIZE 64      // cache line size in bytes used for subtree calculation



// rho config
#define RHO_STASH_SIZE 200  // size of rho stash
#define RHO_LEVEL 19    // # levels in rho
#define RHO_Z 2  // # slots per bucket in rho
#define RHO_OV_TRESHOLD   RHO_STASH_SIZE - RHO_Z*(RHO_LEVEL+1)   // overflow threshold for background eviction; C - Z(L+1)
#define RHO_BK_EVICTION 1   // 0/1 flag to disable/enable background eviction in rho
#define RHO_L1 7  // upto L1 level buckts have specific Z1 number of slots   (inclusive)
#define RHO_L2 8   // upto L2 level buckts have specific Z2 number of slots   (inclusive)
#define RHO_L3 9   // upto L3 level buckts have specific Z3 number of slots   (inclusive)
#define RHO_Z1 2   // # slots per bucket upto L1
#define RHO_Z2 2   // # slots per bucket upto L2
#define RHO_Z3 2   // # slots per bucket upto L3
#define RHO_EMPTY_TOP 0   // # top empty levels of rho ~~~> equivalent to L1 = EMPTY_TOP-1, Z1 = 0 for ------  valcano: 10  freecursive: 0
#define RHO_TOP_CACHE 0   // # top levels that are cached in rho
#define RHO_WAY 10   // # ways in each set accociative entry of rho tag array


// timing channel security config
#define TIMING_INTERVAL 300   // # cycles after each one oram access is initiated either real or dummy one

// prefetching config
#define PREFETCH_BUF_SIZE 256
#define PREFETCH_STRIDE 1
#define PREFETCH_TYPE COMBO






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
extern long long int plb_access[H-1];
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

static const int LZ[LEVEL] = {[0 ... L1] = Z1, [L1+1 ... L2] = Z2, [L2+1 ... L3] = Z3, [L3+1 ... LEVEL-1] = Z};  // array of different Z for different levels in oram
static const int RHO_LZ[RHO_LEVEL] = {[0 ... RHO_L1] = RHO_Z1, [RHO_L1+1 ... RHO_L2] = RHO_Z2, [RHO_L2+1 ... RHO_L3] = RHO_Z3, [RHO_L3+1 ... RHO_LEVEL-1] = RHO_Z};  // array of different Z for different levels in rho

enum{
  // main tree
  PATH = (long long int)pow(2,LEVEL-1),  // # paths in oram tree
  NODE = (long long int)pow(2,LEVEL)-1,  // # nodes in oram tree
  SLOT = Z1*((long long int)pow(2,L1+1)-1) + Z2*((long long int)pow(2,L2+1)-(long long int)pow(2,L1+1)) + Z3*((long long int)pow(2,L3+1)-(long long int)pow(2,L2+1)) + Z*((long long int)pow(2,LEVEL)-(long long int)pow(2,L3+1)),  // # free slots in oram tree
  BLOCK = (long long int)floor(U*(Z1*((long long int)pow(2,L1+1)-1) + Z2*((long long int)pow(2,L2+1)-(long long int)pow(2,L1+1)) + Z3*((long long int)pow(2,L3+1)-(long long int)pow(2,L2+1)) + Z*((long long int)pow(2,LEVEL)-(long long int)pow(2,L3+1)))),  // # valid blocks in oram tree
  CAP_NODE = (int)pow(2,CAP_LEVEL), // # nodes at first non-empty level of tree (L1+1) in oram tree
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
};



void oram_alloc();
void oram_init();
void test_init();
void print_path(int label);
void background_eviction();
void count_tree();
void init_trace();
void print_count_level();
void test_read_write();
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
bool plb_contain(int tag);
void prefetch_access(int addr);
void invoke_prefetch();
void insert_buffer(int addr);
bool buffer_contain(int addr);
int buffer_index(int addr);
unsigned int block_addr(long long int physical_addr);
unsigned int page_addr(long long int physical_addr);
char offset_val(long long int addr);
void footprint_update(long long int addr);
void test_footprint();

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
