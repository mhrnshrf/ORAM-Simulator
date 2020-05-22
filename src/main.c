#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>

#include "processor.h"
#include "configfile.h"
#include "memory_controller.h"
#include "scheduler.h"
#include "params.h"

#define MAXTRACELINESIZE 64
long long int BIGNUM = 1000000;


int expt_done=0;  

long long int CYCLE_VAL=0;

long long int get_current_cycle()
{
  return CYCLE_VAL;
}

struct robstructure *ROB;

FILE **tif;  /* The handles to the trace input files. */
FILE *config_file;
FILE *vi_file;

int *prefixtable;
// Moved the following to memory_controller.h so that they are visible
// from the scheduler.
//long long int *committed;
//long long int *fetched;
long long int *time_done;
long long int total_time_done;
float core_power=0;


// Mehrnoosh:
#include <sys/time.h>
#include <time.h>
#include "cache.h"

struct timeval sday, eday;
long int period = 0;
int periodctr = 0;
int roundprev = 0;
int tracectr = 0;	// # lines read from the trace file 
int hitctr = 0;		// # hits on llc
int missctr = 0;	// # misses on llc
int evictctr = 0;	// # evictions caused after misses on llc
int evictifctr = 0;
int invokectr_prev = 0;
int oram_path_length = 0;	// # slots along each path of oram
int rho_path_length = 0;	// # slots along each path of rho
int oram_effective_pl = 0;	// # slots along each path of oram minus numbers of slots that are cached from top of the tree
int rho_effective_pl = 0;	// # slots along each path of rho minus numbers of slots that are cached from top of the tree
int reqctr = 0;				// # reqs read from oramq that belongs to one oram access	
int maxreq = 0;				// max number for reqctr that is determined based of effective path lengh ot tree (oram or rho)
// int nomem_cycle = 0;		// # cycles passed in a timing interval	~~~> to keep track of cycles no oram access shall be issued  

long long int fetch_clk = 0;
long long int mem_clk = 0;

bool oram_just_invoked = false;		// a flag raised when the current req from oramq is the first request of an freecursive oram access (an invokation of invoke_oram func)
bool still_same_access = false;		// a flag that raised and maintained until a single oram access is dequeued from oramq ~> for timing enabled
bool mem_tick = false;				// a flag that is raised at a cycle that timing interval has reached ~> for timing enabled
bool mem_cycle = false;				// a flag to indicate whether at this cycle a mem op should be fetched ~> for timing enabled
bool dummy_tick = false;			// a flag to indicate whether at this cycle a dummy access should be made ~> for timing enabled
bool oram_tick = false;				// a flag to indicate whether at this cycle an oram access should be made ~> for timing enabled
bool rho_tick = false;				// a flag to indicate whether at this cycle an rho access should be made ~> for timing enabled
bool dummy_oram = false;			// a flag to indicate whether at this cycle a dummy oram access should be made ~> for timing enabled
bool dummy_rho = false;				// a flag to indicate whether at this cycle a dummy rho access should be made ~> for timing enabled


// struct to keep info of one mem request that is issued from cahce rather than from trace file file
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

// int global_array[BLOCK];
#define SUBARRAY_TEST 15
int subtree_array[SUBARRAY_TEST] = {0};


// Mehrnoosh.

int main(int argc, char * argv[])
{
  
  printf("---------------------------------------------\n");
  printf("-- USIMM: the Utah SImulated Memory Module --\n");
  printf("--              Version: 1.3               --\n");
  printf("---------------------------------------------\n");

//   Mehrnoosh:
	
	printf("LEVEL: %d\n",  LEVEL);
	printf("PATH: %d\n",  PATH);
	printf("NODE: %d\n", NODE);
	printf("SLOT: %d\n", SLOT);
	printf("BLOCK: %d\n", BLOCK);
	printf("Z: %d\n", Z);
	printf("U: %f\n", U);
	printf("OV_TRESHOLD: %d\n", OV_TRESHOLD);
	printf("STASH_SIZE: %d\n", STASH_SIZE);
	printf("BK_EVICTION: %d\n", BK_EVICTION);
	printf("EMPTY_TOP: %d\n", EMPTY_TOP);
	printf("TOP_CACHE: %d\n", TOP_CACHE);
	printf("TRACE_SIZE: %d\n", TRACE_SIZE);
	printf("LZ: ");
	// calculating oram path length
	for (int i = 0; i < LEVEL; i++)
	{
		printf("%d ", LZ[i]);
		oram_path_length += LZ[i];
	}
	// calculating effective oram path length
	for (int i = TOP_CACHE; i < LEVEL; i++)
	{
		oram_effective_pl += LZ[i];
	}
	printf("\n= %d ~> oram path length\n", oram_path_length);
	printf("  %d ~> oram effective path length\n", oram_effective_pl);
	printf("L1: %d   Z1:%d\n", L1, Z1);
	printf("L2: %d   Z2:%d\n", L2, Z2);
	printf("L3: %d   Z3:%d\n", L3, Z3);

	printf("\nCACHE_ENABLE: %d\n", CACHE_ENABLE);
	printf("WRITE_BYPASS: %d\n", WRITE_BYPASS);

	printf("\nSUBTREE_ENABLE: %d\n", SUBTREE_ENABLE);
	printf("SUBTREE_SIZE: %d\n", SUBTREE_SIZE);
	printf("SUBTREE_SLOT: %d\n", SUBTREE_SLOT);
	printf("SUBTREE_BUCKET: %d\n", SUBTREE_BUCKET);
	printf("SUBTREE_LEVEL: %d\n", SUBTREE_LEVEL);

	printf("\nRHO_ENABLE: %d\n", RHO_ENABLE);
	printf("RHO_LEVEL: %d\n", RHO_LEVEL);
	printf("RHO_Z: %d\n", RHO_Z);
	// calculating rho path length
	for (int i = 0; i < RHO_LEVEL; i++)
	{
		printf("%d ", RHO_LZ[i]);
		rho_path_length += RHO_LZ[i];
	}
	// calculating effective rho path length
	for (int i = RHO_TOP_CACHE; i < RHO_LEVEL; i++)
	{
		rho_effective_pl += RHO_LZ[i];
	}
	printf("\n= %d ~> rho path length\n", rho_path_length);
	printf("  %d ~> rho effective path length\n", rho_effective_pl);
	printf("RHO_PATH: %d\n", RHO_PATH);
	printf("RHO_NODE: %d\n", RHO_NODE);
	printf("RHO_SLOT: %d\n", RHO_SLOT);
	printf("RHO_BLOCK: %d\n", RHO_BLOCK);
	printf("RHO_SET: %d\n", RHO_SET);
	printf("RHO_WAY: %d\n", RHO_WAY);
 	printf("RHO_OV_TRESHOLD: %d\n", RHO_OV_TRESHOLD);
	printf("RHO_STASH_SIZE: %d\n", RHO_STASH_SIZE);
	printf("RHO_BK_EVICTION: %d\n", RHO_BK_EVICTION);
	printf("RHO_EMPTY_TOP: %d\n", RHO_EMPTY_TOP);
	printf("RHO_TOP_CACHE: %d\n", RHO_TOP_CACHE);

	
	// init_trace();

	
	// test_subtree();
	

	cache_init();
	
	oram_alloc();

	rho_alloc();

	oram_init();
	
	

	

	// test_queue();

	// test_oram(argv);



	fflush(stdout);
	
	 clock_t start, end;
     double cpu_time_used = 0;

	 for (int i = 0; i < NUMCORES; i++)
	 {
		 evicted[i].valid = false;
	 }
	 

	 start = clock();

	//   Mehrnoosh.
  
  int numc=0;
  int num_ret=0;
  int num_fetch=0;
  int num_done=0;
  int numch=0;
  int writeqfull=0;
  int fnstart;
  int currMTapp;
  long long int maxtd;
  int maxcr;
  int pow_of_2_cores;
  char newstr[MAXTRACELINESIZE];
  int *nonmemops;
  char *opertype;
  long long int *addr;
  long long int *instrpc;
  int chips_per_rank=-1;
  // Mehrnoosh:
  int *oramid;
  int *tree;
  int *nonmemops_timing;
  // Mehrnoosh.

  /* Initialization code. */
  printf("Initializing.\n");

  if (argc < 3) {
    printf("Need at least one input configuration file and one trace file as argument.  Quitting.\n");
    return -3;
  }

  config_file = fopen(argv[1], "r");
  if (!config_file) {
    printf("Missing system configuration file.  Quitting. \n");
    return -4;
  }

  NUMCORES = argc-2;


  ROB = (struct robstructure *)malloc(sizeof(struct robstructure)*NUMCORES);
  tif = (FILE **)malloc(sizeof(FILE *)*NUMCORES);
  committed = (long long int *)malloc(sizeof(long long int)*NUMCORES);
  fetched = (long long int *)malloc(sizeof(long long int)*NUMCORES);
  time_done = (long long int *)malloc(sizeof(long long int)*NUMCORES);
  nonmemops = (int *)malloc(sizeof(int)*NUMCORES);
  opertype = (char *)malloc(sizeof(char)*NUMCORES);
  addr = (long long int *)malloc(sizeof(long long int)*NUMCORES);
  instrpc = (long long int *)malloc(sizeof(long long int)*NUMCORES);
  prefixtable = (int *)malloc(sizeof(int)*NUMCORES);
  currMTapp = -1;
  // Mehrnoosh:
  oramid = (int *)malloc(sizeof(int)*NUMCORES);
  tree = (int *)malloc(sizeof(int)*NUMCORES);
  nonmemops_timing = (int *)malloc(sizeof(int)*NUMCORES);
  // Mehrnoosh.
  for (numc=0; numc < NUMCORES; numc++) {
     tif[numc] = fopen(argv[numc+2], "r");
     if (!tif[numc]) {
       printf("Missing input trace file %d.  Quitting. \n",numc);
       return -5;
     }

     /* The addresses in each trace are given a prefix that equals
        their core ID.  If the input trace starts with "MT", it is
	assumed to be part of a multi-threaded app.  The addresses
	from this trace file are given a prefix that equals that of
	the last seen input trace file that starts with "MT0".  For
	example, the following is an acceptable set of inputs for
	multi-threaded apps CG (4 threads) and LU (2 threads):
	usimm 1channel.cfg MT0CG MT1CG MT2CG MT3CG MT0LU MT1LU */
     prefixtable[numc] = numc;

     /* Find the start of the filename.  It's after the last "/". */
     for (fnstart = strlen(argv[numc+2]) ; fnstart >= 0; fnstart--) {
       if (argv[numc+2][fnstart] == '/') {
         break;
       }
     }
     fnstart++;  /* fnstart is either the letter after the last / or the 0th letter. */

     if ((strlen(argv[numc+2])-fnstart) > 2) {
       if ((argv[numc+2][fnstart+0] == 'M') && (argv[numc+2][fnstart+1] == 'T')) {
         if (argv[numc+2][fnstart+2] == '0') {
	   currMTapp = numc;
	 }
	 else {
	   if (currMTapp < 0) {
	     printf("Poor set of input parameters.  Input file %s starts with \"MT\", but there is no preceding input file starting with \"MT0\".  Quitting.\n", argv[numc+2]);
	     return -6;
	   }
	   else 
	     prefixtable[numc] = currMTapp;
	 }
       }
     }
     printf("Core %d: Input trace file %s : Addresses will have prefix %d\n", numc, argv[numc+2], prefixtable[numc]);

     committed[numc]=0;
     fetched[numc]=0;
     time_done[numc]=0;
     ROB[numc].head=0;
     ROB[numc].tail=0;
     ROB[numc].inflight=0;
     ROB[numc].tracedone=0;
  }

  read_config_file(config_file);


	/* Find the appropriate .vi file to read*/
	if (NUM_CHANNELS == 1 && NUMCORES == 1) {
  		vi_file = fopen("input/1Gb_x4.vi", "r"); 
		chips_per_rank= 16;
  		printf("Reading vi file: 1Gb_x4.vi\t\n%d Chips per Rank\n",chips_per_rank); 
	} else if (NUM_CHANNELS == 1 && NUMCORES == 2) {
  		vi_file = fopen("input/2Gb_x4.vi", "r");
		chips_per_rank= 16;
  		printf("Reading vi file: 2Gb_x4.vi\t\n%d Chips per Rank\n",chips_per_rank);
	} else if (NUM_CHANNELS == 1 && (NUMCORES > 2) && (NUMCORES <= 4)) {
  		vi_file = fopen("input/4Gb_x4.vi", "r");
		chips_per_rank= 16;
  		printf("Reading vi file: 4Gb_x4.vi\t\n%d Chips per Rank\n",chips_per_rank);
	} else if (NUM_CHANNELS == 4 && NUMCORES == 1) {
  		vi_file = fopen("input/1Gb_x16.vi", "r");
		chips_per_rank= 4;
  		printf("Reading vi file: 1Gb_x16.vi\t\n%d Chips per Rank\n",chips_per_rank);
	} else if (NUM_CHANNELS == 4 && NUMCORES == 2) {
  		vi_file = fopen("input/1Gb_x8.vi", "r");
		chips_per_rank= 8;
  		printf("Reading vi file: 1Gb_x8.vi\t\n%d Chips per Rank\n",chips_per_rank);
	} else if (NUM_CHANNELS == 4 && (NUMCORES > 2) && (NUMCORES <= 4)) {
  		vi_file = fopen("input/2Gb_x8.vi", "r");
		chips_per_rank= 8;
  		printf("Reading vi file: 2Gb_x8.vi\t\n%d Chips per Rank\n",chips_per_rank);
	} else if (NUM_CHANNELS == 4 && (NUMCORES > 4) && (NUMCORES <= 8)) {
  		vi_file = fopen("input/4Gb_x8.vi", "r");
		chips_per_rank= 8;
  		printf("Reading vi file: 4Gb_x8.vi\t\n%d Chips per Rank\n",chips_per_rank);
	} else if (NUM_CHANNELS == 4 && (NUMCORES > 8) && (NUMCORES <= 16)) {
  		vi_file = fopen("input/4Gb_x4.vi", "r");
		chips_per_rank= 16;
  		printf("Reading vi file: 4Gb_x4.vi\t\n%d Chips per Rank\n",chips_per_rank);
	} else {
		printf ("PANIC:: Channel - Core configuration not supported\n");
		assert (-1);
	}

  	if (!vi_file) {
 	  printf("Missing DRAM chip parameter file.  Quitting. \n");
  	  return -5;
  	}



  assert((log_base2(NUM_CHANNELS) + log_base2(NUM_RANKS) + log_base2(NUM_BANKS) + log_base2(NUM_ROWS) + log_base2(NUM_COLUMNS) + log_base2(CACHE_LINE_SIZE)) == ADDRESS_BITS );
  /* Increase the address space and rows per bank depending on the number of input traces. */
  ADDRESS_BITS = ADDRESS_BITS + log_base2(NUMCORES);
  if (NUMCORES == 1) {
    pow_of_2_cores = 1;
  }
  else {
  pow_of_2_cores = 1 << ((int)log_base2(NUMCORES-1) + 1);
  }
  NUM_ROWS = NUM_ROWS * pow_of_2_cores;

  read_config_file(vi_file);
  print_params();

  for(int i=0; i<NUMCORES; i++)
  {
	  ROB[i].comptime = (long long int*)malloc(sizeof(long long int)*ROBSIZE);
	  ROB[i].mem_address = (long long int*)malloc(sizeof(long long int)*ROBSIZE);
	  ROB[i].instrpc = (long long int*)malloc(sizeof(long long int)*ROBSIZE);
	  ROB[i].optype = (int*)malloc(sizeof(int)*ROBSIZE);
  }
  init_memory_controller_vars();
  init_scheduler_vars();
  /* Done initializing. */

  /* Must start by reading one line of each trace file. */
  for(numc=0; numc<NUMCORES; numc++)
  {
	      if (fgets(newstr,MAXTRACELINESIZE,tif[numc])) {
	        if (sscanf(newstr,"%d %c",&nonmemops[numc],&opertype[numc]) > 0) {
		  if (opertype[numc] == 'R') {
		    if (sscanf(newstr,"%d %c %Lx %Lx",&nonmemops[numc],&opertype[numc],&addr[numc],&instrpc[numc]) < 1) {
		      printf("Panic.  Poor trace format.\n");
		      return -4;
		    }
		  }
		  else {
		    if (opertype[numc] == 'W') {
		      if (sscanf(newstr,"%d %c %Lx",&nonmemops[numc],&opertype[numc],&addr[numc]) < 1) {
		        printf("Panic.  Poor trace format.\n");
		        return -3;
		      }
		    }
		    else {
		      printf("Panic.  Poor trace format.\n");
		      return -2;
		    }
		  }
		}
		else {
		  printf("Panic.  Poor trace format.\n");
		  return -1;
		}
	      }
	      else {
	        if (ROB[numc].inflight == 0) {
	          num_done++;
	          if (!time_done[numc]) time_done[numc] = 1;
	        }
	        ROB[numc].tracedone=1;
	      }
  }


  printf("Starting simulation.\n");
	
  while (!expt_done) {

	// Mehrnoosh:

	no_miss_occured = true;

	if (tracectr >= TRACE_SIZE)
	{
		break;
	}
	
	gettimeofday(&sday, NULL);

	
	// if ((tracectr % 2000 == 0) /*&& tracectr > 9000 && tracectr != roundprev*/ )
	// {
	// 	printf("\n...........................Partial Stat..............................\n");
	// 	printf("@ trace: %d\n", tracectr);
	// 	printf("invoke ctr: 	%d\n", invokectr);
	// 	printf("bk evict rate: %f%%\n", 100*(double)bkctr/invokectr);
	// 	printf("cache hit rate: %f%%\n", 100*(double)hitctr/(hitctr+missctr));
	// 	printf("cache evict rate wrt # miss: %f%%\n", 100*(double)evictctr/(missctr));
	// 	printf("rho hit rate: %f%%\n", 100*(double)rho_hit/(invokectr));
	// 	printf("rho bk evict rate: %f%%\n", 100*(double)rho_bkctr/rho_hit);
	// 	// period = 0;
	// 	// periodctr++;
	// 	// roundprev = tracectr;
	// }
	
	// Mehrnoosh.

    /* For each core, retire instructions if they have finished. */
    for (numc = 0; numc < NUMCORES; numc++) {
      num_ret = 0;
      while ((num_ret < MAX_RETIRE) && ROB[numc].inflight) {
        /* Keep retiring until retire width is consumed or ROB is empty. */
        if (ROB[numc].comptime[ROB[numc].head] < CYCLE_VAL) {  
	  /* Keep retiring instructions if they are done. */
	  ROB[numc].head = (ROB[numc].head + 1) % ROBSIZE;
	  ROB[numc].inflight--;
	  committed[numc]++;
	  num_ret++;
        }
	else  /* Instruction not complete.  Stop retirement for this core. */
	  break;
      }  /* End of while loop that is retiring instruction for one core. */
    }  /* End of for loop that is retiring instructions for all cores. */


    if(CYCLE_VAL%PROCESSOR_CLK_MULTIPLIER == 0)
    { 
      /* Execute function to find ready instructions. */
      update_memory();

      /* Execute user-provided function to select ready instructions for issue. */
      /* Based on this selection, update DRAM data structures and set 
	 instruction completion times. */
	 

      for(int c=0; c < NUM_CHANNELS; c++)
      {
		schedule(c);
		gather_stats(c);	
		  
      }

    }

    /* For each core, bring in new instructions from the trace file to
       fill up the ROB. */
    num_done = 0;
    writeqfull =0;
    for(int c=0; c<NUM_CHANNELS; c++){
	    if(write_queue_length[c] == WQ_CAPACITY)
	    {
		    writeqfull = 1;
		    break;
	    }
    }

    for (numc = 0; numc < NUMCORES; numc++) {
      if (!ROB[numc].tracedone) { /* Try to fetch if EOF has not been encountered. */
        num_fetch = 0;
        while ((num_fetch < MAX_FETCH) && (ROB[numc].inflight != ROBSIZE) && (!writeqfull)) {
          /* Keep fetching until fetch width or ROB capacity or WriteQ are fully consumed. */
	  /* Read the corresponding trace file and populate the tail of the ROB data structure. */
	  /* If Memop, then populate read/write queue.  Set up completion time. */


	  // Mehrnoosh:
	  	// printf("\ncycle: %lld", CYCLE_VAL);
	  	// printf("\nfetch clk: %lld", fetch_clk);
	  	if (TIMING_ENABLE)
		{
			mem_tick = (fetch_clk % TIMING_INTERVAL == 0);					 // whether this cycle it's time to initiate a mem access ~~~> timing interval has reached
			mem_cycle = (mem_tick || still_same_access) ? true : false; 	 // whether this cycle should be processing a mem op

			mem_clk = mem_tick ? mem_clk + 1 : mem_clk;					 // a counter keeping track of # passed mem ticks so far

			oram_tick = (mem_tick && (mem_clk % 3 == 0) )? true : false;	 // whether this cycle it's time to initiate an oram access	
			rho_tick = (mem_tick && !oram_tick)? true : false;				 // whether this cycle it's time to initiate a rho access	
			
			nonmemops_timing[numc] = (mem_cycle) ? 0 : 1;					 // # non mem ops shall be issued next, in case of mem cycle it would be none
			dummy_tick = (mem_tick && (nonmemops[numc] > 0))? true : false;	 // whether this cycle it's time to make a dummy access 

			dummy_oram = dummy_tick && oram_tick;							// whether the dummy access should be oram dummy
			dummy_rho = dummy_tick && rho_tick;								// whether the dummy access should be rho dummy

			if (oramQ->size != 0)
			{
				// if the current available access is not what it's supposed to be, make a dummy access instead
				dummy_oram = (oram_tick && oramQ->head->tree == RHO) || dummy_oram;
				dummy_rho = (rho_tick && oramQ->head->tree == ORAM) || dummy_rho;
			}
			
			
			char *str = oram_tick && !dummy_oram ? "|" : rho_tick && !dummy_rho ? "/" : oram_tick && dummy_oram ? "$" : rho_tick && dummy_rho ? "+" : "_";
			printf("%s ", str);
			if (fetch_clk % 60 == 0)
			{
				printf("\n");
				printf("\n");
			}
			
			// printf("	tick: %d	mem: %d		dummy: %d	nonmemps: %d	req: %d", mem_tick, mem_cycle, dummy_tick, nonmemops[numc], reqctr);
			
			
		}
		else
		{
			nonmemops_timing[numc] = nonmemops[numc];
		}
		
	  // Mehrnoosh.
























	//Mehrnoosh:
	  if (nonmemops_timing[numc]) {  /* Have some non-memory-ops to consume. */
	//   if (nonmemops[numc]) {  /* Have some non-memory-ops to consume. */
	//Mehrnoosh.
	    ROB[numc].optype[ROB[numc].tail] = 'N';
	    ROB[numc].comptime[ROB[numc].tail] = CYCLE_VAL+PIPELINEDEPTH;
	    nonmemops[numc]--;
	    ROB[numc].tail = (ROB[numc].tail +1) % ROBSIZE;
	    ROB[numc].inflight++;
	    fetched[numc]++;
	    num_fetch++;
		// Mehrnoosh:
		// printf("cycle %lld   main: nonmem 	 trace: %d		req: %d 	nonmemops:%d\n", CYCLE_VAL, tracectr, reqctr, nonmemops[numc]);
		// Mehrnoosh.
	  }
	  else { /* Done consuming non-memory-ops.  Must now consume the memory rd or wr. */
			// Mehrnoosh:
			// printf("cycle %lld   main: mem 	 trace: %d		req: %d\n", CYCLE_VAL, tracectr, reqctr);
			// Mehrnoosh.
	      if (opertype[numc] == 'R') {
		  addr[numc] = addr[numc] + (long long int)((long long int)prefixtable[numc] << (ADDRESS_BITS - log_base2(NUMCORES)));    // Add MSB bits so each trace accesses a different address space.
	          ROB[numc].mem_address[ROB[numc].tail] = addr[numc];
	          ROB[numc].optype[ROB[numc].tail] = opertype[numc];
	          ROB[numc].comptime[ROB[numc].tail] = CYCLE_VAL + BIGNUM;
	          ROB[numc].instrpc[ROB[numc].tail] = instrpc[numc];
		
		  // Check to see if the read is for buffered data in write queue - 
		  // return constant latency if match in WQ
		  // add in read queue otherwise
		  int lat = read_matches_write_or_read_queue(addr[numc]);
		  if(lat) {
			ROB[numc].comptime[ROB[numc].tail] = CYCLE_VAL+lat+PIPELINEDEPTH;
		  }
		  else {
			// Mehrnoosh:

			// start = clock();
			insert_read(addr[numc], CYCLE_VAL, numc, ROB[numc].tail, instrpc[numc], oramid[numc], tree[numc]);

			// invoke_oram(addr[numc], CYCLE_VAL, numc, ROB[numc].tail, instrpc[numc], 'R');

			// end = clock();
			// cpu_time_used += ((double) (end - start)) / CLOCKS_PER_SEC;

			
			// Mehrnoosh.
		 	 }
	      }
	      else {  /* This must be a 'W'.  We are confirming that while reading the trace. */
	        if (opertype[numc] == 'W') {
		      addr[numc] = addr[numc] + (long long int)((long long int)prefixtable[numc] << (ADDRESS_BITS - log_base2(NUMCORES)));    // Add MSB bits so each trace accesses a different address space.
		      ROB[numc].mem_address[ROB[numc].tail] = addr[numc];
		      ROB[numc].optype[ROB[numc].tail] = opertype[numc];
		      ROB[numc].comptime[ROB[numc].tail] = CYCLE_VAL+PIPELINEDEPTH;
		      /* Also, add this to the write queue. */

		      if(!write_exists_in_write_queue(addr[numc]))
			// Mehrnoosh:
			{
				// start = clock();

				
				insert_write(addr[numc], CYCLE_VAL, numc, ROB[numc].tail, oramid[numc], tree[numc]);

				// invoke_oram(addr[numc], CYCLE_VAL, numc, ROB[numc].tail, 0, 'W');

				// end = clock();
				// cpu_time_used += ((double) (end - start)) / CLOCKS_PER_SEC;
			}
			// Mehrnoosh.


		      for(int c=0; c<NUM_CHANNELS; c++){
			if(write_queue_length[c] == WQ_CAPACITY)
			{
			  writeqfull = 1;
			  break;
			}
		      }
		}
		else {
		  printf("Panic.  Poor trace format. \n");
		  return -1;
		}
	      }
	      ROB[numc].tail = (ROB[numc].tail +1) % ROBSIZE;
	      ROB[numc].inflight++;
	      fetched[numc]++;
	      num_fetch++;

	      /* Done consuming one line of the trace file.  Read in the next. */
		// Mehrnoosh:

		if (oramQ->size == 0)
		{
			if (TIMING_ENABLE && dummy_tick)
			{
				dummy_access(ORAM); 	// oram to be changed to appropriate tree type based on m*n schedule
			}
			
			// cache enabled:
			else if(CACHE_ENABLE)
			{
				// printf("cache enable if: @ trace %d\n", tracectr);
				while (no_miss_occured && !expt_done)
				{
					if (fgets(newstr,MAXTRACELINESIZE,tif[numc])) {
						// printf("while readline trace ctr: %d  \n", tracectr);
						if (evicted[numc].valid)
						{
							nonmemops[numc] = evicted[numc].nonmemops;
							opertype[numc] = evicted[numc].opertype;
							addr[numc] = evicted[numc].addr;
							evicted[numc].valid = false;
							eviction_writeback[numc] = true;
							evictifctr++;
							// printf("main: evicted if addr: %lld\n", addr[numc]);
							break;
						}
						

						if (sscanf(newstr,"%d %c",&nonmemops[numc],&opertype[numc]) > 0) {
								tracectr++;
							if (opertype[numc] == 'R') {
								if (sscanf(newstr,"%d %c %Lx %Lx",&nonmemops[numc],&opertype[numc],&addr[numc],&instrpc[numc]) < 1) {
								printf("Panic.  Poor trace format.\n");
								return -4;
								}
							}
							else {
								if (opertype[numc] == 'W') {
									if (sscanf(newstr,"%d %c %Lx",&nonmemops[numc],&opertype[numc],&addr[numc]) < 1) {
										printf("Panic.  Poor trace format.\n");
										return -3;
									}
								}
								else {
								printf("Panic.  Poor trace format.\n");
								return -2;
								}
							}
							if (cache_access(addr[numc], opertype[numc]) == HIT)
							{
								hitctr++;
							}
							else // miss occured
							{
								missctr++;
								int victim = cache_fill(addr[numc], opertype[numc]);
								if ( victim != -1)
								{
									evictctr++;
									evicted[numc].valid = true;
									evicted[numc].nonmemops = 0;	//nonmemops[numc]+1;  // ??? probably it should be 0 instead
									evicted[numc].opertype = 'W';
									evicted[numc].addr = victim;
								}

								no_miss_occured = false;

							}
						}
						else {
							printf("Panic.  Poor trace format.\n");
							return -1;
						}
					
					}
					else {
						if (ROB[numc].inflight == 0) {
						num_done++;
						if (!time_done[numc]) time_done[numc] = CYCLE_VAL;
						}
						ROB[numc].tracedone=1;
						break;  /* Break out of the while loop fetching instructions. */
					}

				}

			}
			// cache disabled:
			else if (!CACHE_ENABLE)	
			{
			/* Done consuming one line of the trace file.  Read in the next. */
			if (fgets(newstr,MAXTRACELINESIZE,tif[numc])) {
				if (sscanf(newstr,"%d %c",&nonmemops[numc],&opertype[numc]) > 0) {
					tracectr++;
			if (opertype[numc] == 'R') {
				if (sscanf(newstr,"%d %c %Lx %Lx",&nonmemops[numc],&opertype[numc],&addr[numc],&instrpc[numc]) < 1) {
				printf("Panic.  Poor trace format.\n");
				return -4;
				}
			}
			else {
				if (opertype[numc] == 'W') {
				if (sscanf(newstr,"%d %c %Lx",&nonmemops[numc],&opertype[numc],&addr[numc]) < 1) {
					printf("Panic.  Poor trace format.\n");
					return -3;
				}
				}
				else {
				printf("Panic.  Poor trace format.\n");
				return -2;
				}
			}
				}
				else {
				printf("Panic.  Poor trace format.\n");
				return -1;
				}
				}
				else {
					if (ROB[numc].inflight == 0) {
					num_done++;
					if (!time_done[numc]) time_done[numc] = CYCLE_VAL;
					}
					ROB[numc].tracedone=1;
					break;  /* Break out of the while loop fetching instructions. */
				}
			}

			if (eviction_writeback[numc])
			{
				write_cache_hit = true;
				eviction_writeback[numc] = false;
				if (RHO_ENABLE)
				{
					int masked_addr = (int)(evicted[numc].addr & (BLOCK-1));
					if (rho_lookup(masked_addr) == -1)
					{
						rho_insert(addr[numc]);		// add evicted blk from llc to rho and consequently evicted blk from rho to oram
					}
				}
			}

			invoke_oram(addr[numc], CYCLE_VAL, numc, 0, instrpc[numc], opertype[numc]); // ??? argumnets: cycle_val, numc, 0 are not actually used...
			oram_just_invoked = true;
		} 
		if (oramQ->size != 0)
		{
			// printf("else oramq size: %d   @ trace %d\n", oramQ->size, tracectr);

			// if (TIMING_ENABLE && dummy_tick)
			// {
			// 	TreeType t = oram_tick ? ORAM : RHO;
			// 	dummy_access(t); 	// oram to be changed to appropriate tree type based on m*n schedule
			// 	dummy_tick = false;
			// }

			// if (TIMING_ENABLE && mem_tick)
			// {
			// 	if (oram_tick && oramQ->head->tree == RHO)
			// 	{
			// 		dummy_access(ORAM);
			// 		dummy_tick = false;
			// 	}
			// 	if (rho_tick && oramQ->head->tree == ORAM)
			// 	{
			// 		dummy_access(RHO);
			// 		dummy_tick = false;
			// 	}
				
			// }
			

			still_same_access = true;
			
			int nonmemsaved = nonmemops[numc];
			Element *pN = Dequeue(oramQ);
			addr[numc] = pN->addr;
			// pN->cycle 
			// pN->thread 
			// pN->instr; 
			instrpc[numc] = pN->pc; 
			opertype[numc] = pN->type;
			oramid[numc] = pN->oramid;
			tree[numc] = pN->tree;
			nonmemops[numc] = 0; // ??? not sure about this one ~~~> guess resolved
			

			if (oram_just_invoked)
			{
				// printf("\nmain: oram just invoked @ cycle %lld\n", CYCLE_VAL);
				nonmemops[numc] = nonmemsaved; 	//  determined from the trace
				oram_just_invoked = false;
			}

			if (reqctr == maxreq)
			{
				reqctr = 0;
				maxreq = (pN->tree == RHO)? rho_effective_pl : oram_effective_pl;
				still_same_access = false;
			}
			
			reqctr++;
			free(pN);

		}
		


		// Mehrnoosh.












	      
	  }  /* Done consuming the next rd or wr. */
	// Mehrnoosh:
	fetch_clk++;
	// Mehrnoosh.

	} /* One iteration of the fetch while loop done. */
      } /* Closing brace for if(trace not done). */
      else { /* Input trace is done.  Check to see if all inflight instrs have finished. */
        if (ROB[numc].inflight == 0) {
	  num_done++;
	  if (!time_done[numc]) time_done[numc] = CYCLE_VAL;
	}
      }
    } /* End of for loop that goes through all cores. */


    if (num_done == NUMCORES) {
      /* Traces have been consumed and in-flight windows are empty.  Must confirm that write queues have been drained. */
      for (numch=0;numch<NUM_CHANNELS;numch++) {
        if (write_queue_length[numch]) break;
      }
      if (numch == NUM_CHANNELS) expt_done=1;  /* All traces have been consumed and the write queues are drained. */
    }

    /* Printing details for testing.  Remove later. */
    //printf("Cycle: %lld\n", CYCLE_VAL);
    //for (numc=0; numc < NUMCORES; numc++) {
     // printf("C%d: Inf %d : Hd %d : Tl %d : Comp %lld : type %c : addr %x : TD %d\n", numc, ROB[numc].inflight, ROB[numc].head, ROB[numc].tail, ROB[numc].comptime[ROB[numc].head], ROB[numc].optype[ROB[numc].head], ROB[numc].mem_address[ROB[numc].head], ROB[numc].tracedone);
    //}

    CYCLE_VAL++;  /* Advance the simulation cycle. */

	// Mehrnoosh:
	gettimeofday(&eday, NULL);
    period =  ((eday.tv_sec * 1000000 + eday.tv_usec) - (sday.tv_sec * 1000000 + sday.tv_usec))/ 1000000;
	fflush(stdout);
	// Mehrnoosh.
  }


  /* Code to make sure that the write queue drain time is included in
     the execution time of the thread that finishes last. */
  maxtd = time_done[0];
  maxcr = 0;
  for (numc=1; numc < NUMCORES; numc++) {
    if (time_done[numc] > maxtd) {
      maxtd = time_done[numc];
      maxcr = numc;
    }
  }
  time_done[maxcr] = CYCLE_VAL;

  core_power = 0;
  for (numc=0; numc < NUMCORES; numc++) {
    /* A core has peak power of 10 W in a 4-channel config.  Peak power is consumed while the thread is running, else the core is perfectly power gated. */
    core_power = core_power + (10*((float)time_done[numc]/(float)CYCLE_VAL));
  }
  if (NUM_CHANNELS == 1) {
    /* The core is more energy-efficient in our single-channel configuration. */
    core_power = core_power/2.0 ;
  }



  printf("Done with loop. Printing stats.\n");
  printf("Cycles %lld\n", CYCLE_VAL);
  total_time_done = 0;
  for (numc=0; numc < NUMCORES; numc++) {
    printf("Done: Core %d: Fetched %lld : Committed %lld : At time : %lld\n", numc, fetched[numc], committed[numc], time_done[numc]);
    total_time_done += time_done[numc];
  }
  printf("Sum of execution times for all programs: %lld\n", total_time_done);
  printf("Num reads merged: %lld\n",num_read_merge);
  printf("Num writes merged: %lld\n",num_write_merge);
  /* Print all other memory system stats. */
  scheduler_stats();
  print_stats();  

  /*Print Cycle Stats*/
  for(int c=0; c<NUM_CHANNELS; c++)
	  for(int r=0; r<NUM_RANKS ;r++)
		  calculate_power(c,r,0,chips_per_rank);

	printf ("\n#-------------------------------------- Power Stats ----------------------------------------------\n");
	printf ("Note:  1. termRoth/termWoth is the power dissipated in the ODT resistors when Read/Writes terminate \n");
	printf ("          in other ranks on the same channel\n");
	printf ("#-------------------------------------------------------------------------------------------------\n\n");


  /*Print Power Stats*/
	float total_system_power =0;
  for(int c=0; c<NUM_CHANNELS; c++)
	  for(int r=0; r<NUM_RANKS ;r++)
		  total_system_power += calculate_power(c,r,1,chips_per_rank);

		printf ("\n#-------------------------------------------------------------------------------------------------\n");
	if (NUM_CHANNELS == 4) {  /* Assuming that this is 4channel.cfg  */
	  printf ("Total memory system power = %f W\n",total_system_power/1000);
	  printf("Miscellaneous system power = 40 W  # Processor uncore power, disk, I/O, cooling, etc.\n");
	  printf("Processor core power = %f W  # Assuming that each core consumes 10 W when running\n",core_power);
	  printf("Total system power = %f W # Sum of the previous three lines\n", 40 + core_power + total_system_power/1000);
	  printf("Energy Delay product (EDP) = %2.9f J.s\n", (40 + core_power + total_system_power/1000)*(float)((double)CYCLE_VAL/(double)3200000000) * (float)((double)CYCLE_VAL/(double)3200000000));
	}
	else {  /* Assuming that this is 1channel.cfg  */
	  printf ("Total memory system power = %f W\n",total_system_power/1000);
	  printf("Miscellaneous system power = 10 W  # Processor uncore power, disk, I/O, cooling, etc.\n");  /* The total 40 W misc power will be split across 4 channels, only 1 of which is being considered in the 1-channel experiment. */
	  printf("Processor core power = %f W  # Assuming that each core consumes 5 W\n",core_power);  /* Assuming that the cores are more lightweight. */
	  printf("Total system power = %f W # Sum of the previous three lines\n", 10 + core_power + total_system_power/1000);
	  printf("Energy Delay product (EDP) = %2.9f J.s\n", (10 + core_power + total_system_power/1000)*(float)((double)CYCLE_VAL/(double)3200000000) * (float)((double)CYCLE_VAL/(double)3200000000));
	}


// Mehrnoosh:

end = clock();
cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;


printf("\n............... ORAM Stats ...............\n");
printf("total time: %f s\n", cpu_time_used);
printf("trace ctr: %d\n", tracectr);
printf("invoke ctr: 	%d\n", invokectr);
printf("oram ctr: 	%d\n", oramctr);
printf("oram dummy ctr: 	%d\n", dummyctr);
printf("bk evict rate: %f%%\n", 100*(double)bkctr/invokectr);
printf("cache hit rate: %f%%\n", 100*(double)hitctr/(hitctr+missctr));
printf("cache evict rate wrt # miss: %f%%\n", 100*(double)evictctr/(missctr));
printf("rho hit rate: %f%%\n", 100*(double)rho_hit/(invokectr));
printf("rho bk evict rate: %f%%\n", 100*(double)rho_bkctr/rho_hit);
printf("rho dummy ctr: 	%d\n", rho_dummyctr);

// print_cap_percent();
// count_tree();

// printf("stash dist:\n");
// for (int i = 0; i < STASH_SIZE+1; i++)
// {
// 	printf("%d\n",stash_dist[i]);
// }


// Mehrnoosh.

  return 0;
}







