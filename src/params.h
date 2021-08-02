#ifndef __PARAMS_H__
#define __PARAMS_H__

#include <stdbool.h>

/********************/
/* Processor params */
/********************/
// number of cores in mulicore 
int NUMCORES;

// processor clock frequency multiplier : multiplying the
// DRAM_CLK_FREQUENCY by the following parameter gives the processor
// clock frequency 
 int PROCESSOR_CLK_MULTIPLIER;

//size of ROB
 int ROBSIZE ;// 128;		

// maximum commit width
 int MAX_RETIRE ;// 2;

// maximum instruction fetch width
 int MAX_FETCH ;// 4;	

// depth of pipeline
 int PIPELINEDEPTH ;// 5;


/*****************************/
/* DRAM System Configuration */
/*****************************/
// total number of channels in the system
 int NUM_CHANNELS ;// 1;

// number of ranks per channel
 int NUM_RANKS ;// 2;

// number of banks per rank
 int NUM_BANKS ;// 8;

// number of rows per bank
 int NUM_ROWS ;// 32768;

// number of columns per rank
 int NUM_COLUMNS ;// 128;

// cache-line size (bytes)
 int CACHE_LINE_SIZE ;// 64;

// total number of address bits (i.e. indicates size of memory)
 int ADDRESS_BITS ;// 32;

/****************************/
/* DRAM Chip Specifications */
/****************************/

// dram frequency (not datarate) in MHz
 int DRAM_CLK_FREQUENCY ;// 800;

// All the following timing parameters should be 
// entered in the config file in terms of memory 
// clock cycles.

// RAS to CAS delay
 int T_RCD ;// 44;

// PRE to RAS
 int T_RP ;// 44;

// ColumnRD to Data burst
 int T_CAS ;// 44;

// RAS to PRE delay
 int T_RAS ;// 112;

// Row Cycle time
 int T_RC ;// 156;

// ColumnWR to Data burst
 int T_CWD ;// 20;

// write recovery time (COL_WR to PRE)
 int T_WR ;// 48;

// write to read turnaround
 int T_WTR ;// 24;

// rank to rank switching time
 int T_RTRS ;// 8;

// Data transfer
 int T_DATA_TRANS ;// 16;

// Read to PRE
 int T_RTP ;// 24;

// CAS to CAS
 int T_CCD ;// 16;

// Power UP time fast
 int T_XP ;// 20;

// Power UP time slow
 int T_XP_DLL ;// 40;

// Power down entry
 int T_CKE ;// 16;

// Minimum power down duration
 int T_PD_MIN ;// 16;

// rank to rank delay (ACTs to same rank)
 int T_RRD ;// 20;

// four bank activation window
 int T_FAW ;// 128;

// refresh interval
 int T_REFI;

 // refresh cycle time
 int T_RFC;

/****************************/
/* VOLTAGE & CURRENT VALUES */
/****************************/

float VDD;

float IDD0;

float IDD1;

float IDD2P0;

float IDD2P1;

float IDD2N;

float IDD3P;

float IDD3N;

float IDD4R;

float IDD4W;

float IDD5;

/******************************/
/* MEMORY CONTROLLER Settings */
/******************************/

// maximum capacity of write queue (per channel)
 int WQ_CAPACITY ;// 64;

//  int ADDRESS_MAPPING mode
// 1 is consecutive cache-lines to same row
// 2 is consecutive cache-lines striped across different banks 
 int ADDRESS_MAPPING ;// 1;

 // WQ associative lookup 
 int WQ_LOOKUP_LATENCY;



 //.....................................................
 //                     ORAM Config
 //.....................................................

 
 // options 

bool TIMEOUT_ENABLE ;     // 0/1 flag to disable/enable finishing the program in case it get stuck
bool SUBTREE_ENABLE ;     // 0/1 flag to disable/enable having subtree adddressing scheme
bool CACHE_ENABLE   ;     // 0/1 flag to disable/enable having cache
bool WRITE_BYPASS   ;     // 0/1 flag to disable/enable cacheing the path id along the data in the LLC which will benefit write reqs to bypass posmap lookup 
bool RHO_ENABLE     ;     // 0/1 flag to disable/enable having rho
bool TIMING_ENABLE  ;     // 0/1 flag to disable/enable having timing channel security
bool PREFETCH_ENABLE;     // 0/1 flag to disable/enable having prefetching option in case of having timing channel security
bool EARLY_ENABLE   ;     // 0/1 flag to disable/enable early eviction option in case of having timing channel security
bool SNAPSHOT_ENABLE;     // 0/1 flag to disable/enable performing snapshot by making path oram accesses
bool NONSEC_ENABLE  ;     // 0/1 flag to disable/enable oram simulation if off usimm runs normally
bool BK_EVICTION    ;     // 0/1 flag to disable/enable background eviction
bool RHO_BK_EVICTION;     // 0/1 flag to disable/enable background eviction c
bool SNAP_CACHE     ;     // 0/1 flag to disable/enable  snapshot with having L2 cache
bool RAND_ENABLE    ;     // 0/1 flag to disable/enable rand address instead of trace addr
bool WSKIP_ENABLE   ;     // 0/1 flag to disable/enable write linger feature for ring oram
bool SKIP_ENABLE    ;     // 0/1 flag to disable/enable skip middle level feature for ring oram
bool LINGER_BASE    ;     // 0/1 flag to disable/enable write linger baseline for ring oram
bool DUMMY_ENABLE   ;     // 0/1 flag to disable/enable dummy enable baseline for ring oram
bool DYNAMIC_EP     ;     // 0/1 flag that indicates whether ep occur based on number of reshuffles rather than static schedule or 
bool META_ENABLE    ;     // 0/1 flag that indicates whether stale info is stored in metadata tree
bool SIM_ENABLE     ;     // 0/1 flag that indicates whether usimm simulation is enabled if disabled only oram alg runs
bool WAIT_ENABLE    ;     // 0/1 flag that indicates whether wait for last read req to complete
bool LLC_DIRTY      ;     // 0/1 flag that indicates whether everything is dirty eviction from cache
bool DEAD_ENABLE    ;     // 0/1 flag that indicates whether dead block reuse option in enabled
bool NVM_ENABLE     ;     // 0/1 flag that indicates whether nvm memory fixed latency is applied enabled
bool SURONLY_ENABLE ;     // 0/1 flag that indicates whether only surplus slots are used for remote allocation
bool CB_ENABLE      ;     // 0/1 flag that indicates whether compact bucket feature of string oram paper is enabled


long long int TRACE_SIZE ;       // # addr read from trace file
int QUEUE_SIZE ;                  // oramq capacity
int PAGE_SIZE ;                          // page size in byte ~~~> 4KB
int L1_LATENCY ;                        // L1 latency in terms of # cycles 
int L2_LATENCY ;                     // L2 latency in terms of # cycles 
int MAINMEM_LATENCY ;                   // mem latency in terms of # cycles 
long long int WARMUP_CACHE ;          // L2 warm up threshold, after which stats are gathered and memory accesses are actully made
int TIMEOUT_THRESHOLD ;                           // time out threshold in seconds
int TOP_BOUNDARY ;                      // top region tree boundry
int MID_BOUNDARY ;                      // middle region tree boundry
int TOP_CACHE ;                        // # top levels that are cached ---------- freecursive: 10, volcano: don't care
int RHO_EMPTY_TOP ;                     // # top empty levels of rho ~~~> equivalent to L1 = EMPTY_TOP-1, Z1 = 0 for ------  valcano: 10  freecursive: 0
int RHO_TOP_CACHE ;                     // # top levels that are cached in rho
int T1_INTERVAL ;                    // # cycles after each one oram access is initiated either real or dummy one, t1
int T2_INTERVAL ;                    // # cycles after each one oram access is initiated either real or dummy one, t2
int RING_A ;                            // ring oram evict path 
int EP_TURN ;
int SKIP_LIMIT ;
int SKIP_L1 ;
int SKIP_L2 ;
int DUMMY_TH ;
int DEP_TH ;                                 // threshold on # reshuffle for dynamic ep 
int WL_CAP ;                            // cap on wl feature
int DEADQ_SIZE ;                            // size of dead queue
int DEAD_GATHER_OFFSET ;                            // how many levels to skip after top cache for gathering dead blocks
float REMOTE_ALLOC_RATIO ;                            // what percentage of dead blk let the leaves occupy
int NVM_START ;                            // starting what level is stored in nvm (inclusive)
int WARMUP_TREE  ;         // warmup threshold for oram tree
int CB_GREEN_MAX  ;         // number of green blocks in compact bucket feature of string oram paper
int NVM_LATENCY  ;          // nvm latency in terms of DRAM cycle
int REMOTE_START_OFF;  // offset in terms of number of trace, how earlier than warmup start the remote alloc
int NVM_CHANNEL ;  // number of channels dedicated to nvm




// params




#endif // __PARAMS_H__

