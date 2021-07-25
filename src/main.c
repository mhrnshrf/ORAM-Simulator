#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>


#include "processor.h"
#include "configfile.h"
#include "memory_controller.h"
#include "scheduler.h"
#include "params.h"
#include "stt.h"

#define MAXTRACELINESIZE 164
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

// FILE **shadtif;  /* The handles to the trace input files. */

// Mehrnoosh:
#include <sys/time.h>
#include <time.h>
#include "cache.h"
#include "prefetcher.h"
#include "plb.h"
#include<signal.h> 

struct timeval sday, eday;
long int period = 0;
int periodctr = 0;
int nonmemctr = 0;
int roundprev = 0;
// double exe_time = 0;
// int tracectr = 0;	// # lines read from the trace file 
// int hitctr = 0;		// # hits on llc
// int missctr = 0;	// # misses on llc
int evictifctr = 0;
int invokectr_prev = 0;
int oram_path_length = 0;	// # slots along each path of oram
int rho_path_length = 0;	// # slots along each path of rho
// int oram_effective_pl = 0;	// # slots along each path of oram minus numbers of slots that are cached from top of the tree
int rho_effective_pl = 0;	// # slots along each path of rho minus numbers of slots that are cached from top of the tree
int reqctr = 0;				// # reqs read from oramq that belongs to one oram access	
int maxreq = 0;				// max number for reqctr that is determined based of effective path lengh ot tree (oram or rho)
// int nomem_cycle = 0;		// # cycles passed in a timing interval	~~~> to keep track of cycles no oram access shall be issued  

long long int trace_clk = 0;
// long long int mem_clk = 0;

bool oram_just_invoked = false;		// a flag raised when the current req from oramq is the first request of an freecursive oram access (an invokation of invoke_oram func)
bool still_same_access = false;		// a flag that raised and maintained until a single oram access is dequeued from oramq ~> for timing enabled
bool mem_tick = false;				// a flag that is raised at a cycle that timing interval has reached ~> for timing enabled
bool mem_cycle = false;				// a flag to indicate whether at this cycle a mem op should be fetched ~> for timing enabled
bool dummy_tick = false;			// a flag to indicate whether at this cycle a dummy access should be made ~> for timing enabled
bool oram_tick = false;				// a flag to indicate whether at this cycle an oram access should be made ~> for timing enabled
bool rho_tick = false;				// a flag to indicate whether at this cycle an rho access should be made ~> for timing enabled
bool dummy_oram = false;			// a flag to indicate whether at this cycle a dummy oram access should be made ~> for timing enabled
bool dummy_rho = false;				// a flag to indicate whether at this cycle a dummy rho access should be made ~> for timing enabled
bool skip_invokation = false;		// a flag to indicate whether oram invokation should be skipped, it is raised at dummy tick ~> for timing enabled
bool dummy_already_made = false;	// a flag to indicate whether a dummy access has already been made ~> for timing enabled
bool bk_already_made = false;	// a flag to indicate whether a bk evict access has already been made ~> for bk evict enabled



int curr_access = -3; 	// the id of current access (oram or rho, real or dummy)

int fill_access = 0;	// # prefetch history table access
int fill_miss = 0;		// # miss on prefetch history table access

int hit_nonmemops = 0;  // # nonmemops encountered during hit on llc
int consec_dumctr = 0;

int endpoint;

long long int rctr = 0;
long long int wctr = 0;
long long int instctr= 0;

double rmpki;
double wmpki;



int path_access_latency = 0;
double path_access_latency_avg = 0;
// struct to keep info of one mem request that is issued from cahce rather than from trace file file
typedef struct MemRequest{
  bool valid;
  int nonmemops; 
  char opertype;
  long long int addr;
  long long int instrpc;
} MemRequest;


MemRequest waited_for_evicted[16]; 	// array of request that are waitng for evicted block writeback to finish for cores, each core can have one waited request at a time 16: max num of cores
bool no_miss_occured;	// a flag that is set based on cache access and used to keep on reading trace file until it's cache hit
bool eviction_writeback[16] = {0}; // a flag that says next request is gonna be eviction writeback

// int global_array[BLOCK];
#define SUBARRAY_TEST 15
int subtree_array[SUBARRAY_TEST] = {0};

void print_req_symbols(){
	char *str = oram_tick && !dummy_oram ? "|" : rho_tick && !dummy_rho ? "/" : oram_tick && dummy_oram ? "$" : rho_tick && dummy_rho ? "+" : "_";
	printf("%s ", str);
	if (trace_clk % TIMING_INTERVAL == 0)
	{
		printf("\n");
		printf("\n");
	}
}

void print_config(char * filename){
	char line[200];

	FILE *fp = fopen(filename, "r");

    if (fp == NULL)
	{	printf("ERROR: Config file is null\n");
		exit(EXIT_FAILURE);
	}

	printf("\n....................................................\n");
	printf("                Config File Parameters\n");
	printf("....................................................\n");

    while (fgets(line, 200, fp) != NULL ) {
        printf("%s", line);
    }
	fclose(fp);
}


void print_oram_params(){
	printf("\n....................................................\n");
	printf("                     Options\n");
	printf("....................................................\n");
	printf("Cache          %s\n", CACHE_ENABLE?"Enabled":"No" );
	printf("Subtree        %s\n", SUBTREE_ENABLE?"Enabled":"No" );
	printf("Volcano        %s\n", VOLCANO_ENABLE?"Enabled":"No" );
	printf("STT            %s\n", STT_ENABLE?"Enabled":"No" );
	printf("STL            %s\n", STL_ENABLE?"Enabled":"No" );
	printf("Write Bypass   %s\n", WRITE_BYPASS?"Enabled":"No" );
	printf("Rho            %s\n", RHO_ENABLE?"Enabled":"No" );
	printf("Timing         %s\n", TIMING_ENABLE?"Enabled":"No" );
	printf("Prefetch       %s\n", PREFETCH_ENABLE?"Enabled":"No" );
	printf("Early WB       %s\n", EARLY_ENABLE?"Enabled":"No" );
	printf("Snapshot       %s\n", SNAPSHOT_ENABLE?"Enabled":"No" );
	printf("Timeout        %s\n", TIMEOUT_ENABLE?"Enabled":"No" );
	printf("Ring           %s\n", RING_ENABLE?"Enabled":"No" );
	printf("Nonsecure      %s\n", NONSEC_ENABLE?"Enabled":"No" );
	printf("LLC Dirty      %s\n\n", LLC_DIRTY?"Enabled":"No" );
 

	printf("....................................................\n");
	printf("             Simulation Parameters\n");
	printf("....................................................\n");
	printf("Trace         %s\n", bench);
  	printf("T Interval    %d\n", TIMING_INTERVAL);
	printf("Trace Size    %dm\n", (int)(TRACE_SIZE/pow(10,6)));
  	printf("Endpoint      %dm\n", (int)(endpoint/pow(10,6)));
	printf("Queue Size    %d\n", QUEUE_SIZE);
	printf("Page Size     %d KB\n", (int)(PAGE_SIZE/pow(1024,1)));
	printf("L1 Latency    %d\n", L1_LATENCY);
	printf("L2 Latency    %d\n", L2_LATENCY);
	printf("Mem Latency   %d\n", MAINMEM_LATENCY);
	printf("Warmup Thld   %dm\n", (int)(WARMUP_CACHE/pow(10,6)));
	printf("Timeout Thld  %d (s)\n", TIMEOUT_THRESHOLD);
	printf("Top Boundary   %d\n", TOP_BOUNDARY);
	printf("Mid Boundary   %d\n\n", MID_BOUNDARY);

	printf("....................................................\n");
	printf("                  ORAM Config\n");
	printf("....................................................\n");
	printf("Level           %d\n",  LEVEL);
	printf("Path            %d\n",  PATH);
	printf("Node            %d\n", NODE);
	printf("Slot            %d\n", SLOT);
	printf("Block           %d\n", BLOCK);
	printf("Z               %d\n", Z);
	printf("U               %f\n", U);
	printf("OV Threshold    %d\n", OV_THRESHOLD);
	printf("Stash Size      %d\n", STASH_SIZE);
	printf("PLB Size        %d KB / %d-way\n", (int)(PLB_SIZE/pow(1024,1)), PLB_WAY);
	printf("BK Eviction     %d\n", BK_EVICTION);
	printf("Empty Top       %d\n", EMPTY_TOP);
	printf("Top Cache       %d\n\n", TOP_CACHE);
	printf("L1  %d       Z1  %d\n", L1, Z1);
	printf("L2  %d      Z2  %d\n", L2, Z2);
	printf("L3  %d      Z3  %d\n\n", L3, Z3);
	printf("LZ ");
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
	printf("  %d ~> oram effective path length\n\n", oram_effective_pl);

	printf("\n....................................................\n");
	printf("                  Cache Config\n");
	printf("....................................................\n");
	printf("Cache Enable   %s\n", CACHE_ENABLE?"On":"Off");
	printf("LLC Size       %d MB / %d-way\n", (int)(CACHE_SIZE/pow(1024,2)), NUM_WAY);
	printf("Write Bypass   %d\n", WRITE_BYPASS);

	printf("\n....................................................\n");
	printf("                 Subtree Config\n");
	printf("....................................................\n");
	printf("Subtree Enable 	   %s\n", SUBTREE_ENABLE?"On":"Off");
	printf("Subtree Channels   %d\n", NUM_CHANNELS_SUBTREE);
	printf("Subtree Size       %d\n", SUBTREE_SIZE);
	printf("Subtree Slot	   %d\n", SUBTREE_SLOT);
	printf("Subtree Bucket     %d\n", SUBTREE_BUCKET);
	printf("Subtree Level      %d\n", SUBTREE_LEVEL);

	printf("\n....................................................\n");
	printf("                   RHO Config\n");
	printf("....................................................\n");
	printf("Rho Enable          %s\n", RHO_ENABLE?"On":"Off");
	printf("Rho Level           %d\n", RHO_LEVEL);
	printf("Rho Path            %d\n", RHO_PATH);
	printf("Rho Node            %d\n", RHO_NODE);
	printf("Rho Slot            %d\n", RHO_SLOT);
	printf("Rho Block           %d\n", RHO_BLOCK);
	printf("Rho Set             %d\n", RHO_SET);
	printf("Rho Way             %d\n", RHO_WAY);
	printf("Rho Z               %d\n", RHO_Z);
 	printf("Rho OV Threshold    %d\n", RHO_OV_THRESHOLD);
	printf("Rho Stash Size      %d\n", RHO_STASH_SIZE);
	printf("Rho BK Eviction     %d\n", RHO_BK_EVICTION);
	printf("Rho Empty Top       %d\n", RHO_EMPTY_TOP);
	printf("Rho Top Cache       %d\n\n", RHO_TOP_CACHE);
	printf("Rho   L1  %d     Z1  %d\n", RHO_L1, RHO_Z1);
	printf("Rho   L2  %d    Z2  %d\n", RHO_L2, RHO_Z2);
	printf("Rho   L3  %d    Z3  %d\n\n", RHO_L3, RHO_Z3);
	printf("Rho LZ ");
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
	printf("  %d ~> rho effective path length\n\n", rho_effective_pl);

	printf("\n....................................................\n");
	printf("                 Timing Config\n");
	printf("....................................................\n");
	printf("Timing Enable       %s\n", TIMING_ENABLE?"On":"Off");
	// printf("Timing Interval     %d cycles\n\n", TIMING_INTERVAL);
	printf("T1 Interval         %d cycles\n", T1_INTERVAL);
	printf("T2 Interval         %d cycles\n\n", T2_INTERVAL);

	printf("\n....................................................\n");
	printf("                 Prefetch Config\n");
	printf("....................................................\n");
	printf("Prefetch Enable     %s\n", PREFETCH_ENABLE?"On":"Off");
	printf("Buffer Entry #      %d\n\n", PREFETCH_BUF_SIZE);


	printf("\n....................................................\n");
	printf("                 STT Config\n");
	printf("....................................................\n");
	printf("STT Enable     %s\n", STT_ENABLE?"On":"Off");
	printf("STT Size       %d entry / %d-way\n\n", STT_SET*STT_WAY, STT_WAY);


	printf("\n....................................................\n");
	printf("                 Ring Config\n");
	printf("....................................................\n");
	printf("Ring Enable     %s\n", RING_ENABLE?"On":"Off");
	printf("Ring STL        %s\n", RSTL_ENABLE?"On":"Off");
	printf("Write Linger    %s\n", WSKIP_ENABLE?"On":"Off");
	printf("Dummy Enable    %s\n", DUMMY_ENABLE?"On":"Off");
	printf("Dynamic EP      %s\n", DYNAMIC_EP?"On":"Off");
	printf("DEP TH          %d\n", DEP_TH);
	printf("Ring A	        %d\n", RING_A);
	printf("Ring S	        %d\n", RING_S);
	printf("Ring Z	        %d\n", RING_Z);
	printf("Ring   SL1  %d     S1  %d\n", SL1, S1);
	printf("Ring   SL2  %d    S2  %d\n", SL2, S2);
	printf("Ring   SL3  %d    S3  %d\n\n", SL3, S3);
	printf("Ring LS \n");
	for (int i = 0; i < LEVEL; i++)
	{
		printf("%d ", LS[i]);
	}
	// printf("\n= %d ~> ring oram s\n", ring_oram_path_length);
	// printf("  %d ~> ring oram effective s\n\n", ring_oram_effective_pl);
	

	
	printf("\n....................................................\n\n\n\n\n");

}


// Mehrnoosh.

int main(int argc, char * argv[])
{

	// random_trace();

	last_read_served = true;
  
  printf("---------------------------------------------\n");
  printf("-- USIMM: the Utah SImulated Memory Module --\n");
  printf("--              Version: 1.3               --\n");
  printf("---------------------------------------------\n");

//   Mehrnoosh:

	for (int i = 0; i < argc; i++)
	{
	 pargv[i] = argv[i];
	}

	strcpy(exp_name, argv[3]);
	// strncpy(exp_name, argv[3], 20);
	// int c = 0;
	// do 
	// {
	// 	exp_name[c] = argv[3][c];
	// 	c++;
	// }  while (argv[3][c] != '\0');
	
	// printf("exp name %s\n", exp_name); 
	// test delete from q 
	
	// Queue *q = ConstructQueue(4);
	// for (int i = 1; i <= q->limit; i++)
	// {
	// 	Element *e = (Element*) malloc(sizeof (Element)); 
	// 	e->index = i;
	// 	e->offset = i*10;
	// 	Enqueue(q, e);
	// 	printf("%d , %d added\n", e->index, e->offset);
	// }
	// printf("queue size: %d\n", q->size);
	// printf("tail %d , %d \n", q->tail->index, q->tail->offset);
	// printf("head %d , %d \n", q->head->index, q->head->offset);

	// remove_dead(q, 4, 40);

	// printf("@> remove queue size: %d\n", q->size);



	// Element *e = (Element*) malloc(sizeof (Element)); 
	// e->index = 7;
	// e->offset = 7*10;
	// Enqueue(q, e);
	// printf("%d , %d added\n", e->index, e->offset);

	// printf("tail %d , %d \n", q->tail->index, q->tail->offset);
	// printf("head %d , %d \n", q->head->index, q->head->offset);


	// int qs = q->size;
	
	 
	// for (int i = 0; i < qs; i++)
	// {	Element *e = Dequeue(q);

	// 	printf("%d , %d\n", e->index, e->offset);
	// }

	// exit(0);
	



	
	 clock_t start, end;
     

	 for (int i = 0; i < NUMCORES; i++)
	 {
		 waited_for_evicted[i].valid = false;
	 }
	 

	 

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
//   int *nonmemops;
//   char *opertype;
//   long long int *addr;
//   long long int *instrpc;
  int chips_per_rank=-1;
  // Mehrnoosh:
//   int *shad_nonmemops;
//   char *shad_opertype;
//   long long int *shad_addr;
//   long long int *shad_instrpc;
//   char shadstr[MAXTRACELINESIZE];
//   int *oramid;
//   int *tree;
//   bool *last_read;
//   int *nonmemops_timing;
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

//   NUMCORES = argc-2;
  NUMCORES = argc-3;


  ROB = (struct robstructure *)malloc(sizeof(struct robstructure)*NUMCORES);
//   struct robstructure ROB[NCORES];
//   tif = (FILE **)malloc(sizeof(FILE *)*NUMCORES);
  FILE * tif[NCORES];

//   committed = (long long int *)malloc(sizeof(long long int)*NUMCORES);
  long long int committed[NCORES];
//   fetched = (long long int *)malloc(sizeof(long long int)*NUMCORES);
  long long int fetched[NCORES];
//   time_done = (long long int *)malloc(sizeof(long long int)*NUMCORES);
  long long int time_done[NCORES];
//   nonmemops = (int *)malloc(sizeof(int)*NUMCORES);
  int nonmemops[NCORES];
//   opertype = (char *)malloc(sizeof(char)*NUMCORES);
  char opertype[NCORES];
//   addr = (long long int *)malloc(sizeof(long long int)*NUMCORES);
  long long int addr[NCORES];
//   instrpc = (long long int *)malloc(sizeof(long long int)*NUMCORES);
  long long int instrpc[NCORES];
//   prefixtable = (int *)malloc(sizeof(int)*NUMCORES);
  int prefixtable[NCORES];
  currMTapp = -1;
  // Mehrnoosh:
//   shadtif = (FILE **)malloc(sizeof(FILE *)*NUMCORES);

//   oramid = (int *)malloc(sizeof(int)*NUMCORES);
  int oramid[NCORES];
//   tree = (int *)malloc(sizeof(int)*NUMCORES);
  int tree[NCORES];
//   last_read = (bool *)malloc(sizeof(bool)*NUMCORES);
  bool last_read[NCORES];
//   nonmemops_timing = (int *)malloc(sizeof(int)*NUMCORES);
  int nonmemops_timing[NCORES];
//   shad_nonmemops = (int *)malloc(sizeof(int)*NUMCORES);
//   shad_opertype = (char *)malloc(sizeof(char)*NUMCORES);
//   shad_addr = (long long int *)malloc(sizeof(long long int)*NUMCORES);
//   shad_instrpc = (long long int *)malloc(sizeof(long long int)*NUMCORES);



  for (numc=0; numc < NUMCORES; numc++) {
	//  shadtif[numc] = fopen(argv[numc+2], "r");
    //  if (!shadtif[numc]) {
    //    printf("Missing shadow input trace file %d.  Quitting. \n",numc);
    //    return -5;
    //  }


     tif[numc] = fopen(argv[numc+2], "r");
     if (!tif[numc]) {
       printf("Missing input trace file %d.  Quitting. \n",numc);
       return -5;
     }
	

	 
	 



	// Mehrnoosh.

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
  print_config(argv[1]);



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
			  // Mehrnoosh:
			//   fgets(shadstr,MAXTRACELINESIZE,shadtif[numc]);
			//   fgets(shadstr,MAXTRACELINESIZE,shadtif[numc]);
			//   printf("newstr: %s\n", newstr);
			//   printf("shadstr: %s\n", shadstr);
			  // Mehrnoosh:
	        if (sscanf(newstr,"%d %c",&nonmemops[numc],&opertype[numc]) > 0) {
		  if (opertype[numc] == 'R') {
		    if (sscanf(newstr,"%d %c %Lx %Lx",&nonmemops[numc],&opertype[numc],&addr[numc],&instrpc[numc]) < 1) {
		      printf("Panic.  Poor trace format. newstr: %s\n", newstr);
		      return -4;
		    }
		  }
		  else {
		    if (opertype[numc] == 'W') {
		      if (sscanf(newstr,"%d %c %Lx",&nonmemops[numc],&opertype[numc],&addr[numc]) < 1) {
		        printf("Panic.  Poor trace format. newstr: %s\n", newstr);
		        return -3;
		      }
		    }
		    else {
		      printf("Panic.  Poor trace format. newstr: %s\n", newstr);
		      return -2;
		    }
		  }
		}
		else {
		  printf("Panic.  Poor trace format. newstr: %s\n", newstr);
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


  printf("Starting simulation.\n\n");


//   Mehrnoosh:

	char bench_path[100]; 
	strcpy(bench_path, argv[2]);
	char delim[] = "/";
	char *ptr = strtok(bench_path, delim);

	while (ptr != NULL)
	{
		strcpy(bench, ptr);
		ptr = strtok(NULL, delim);
	}

	if (SIM_ENABLE && CACHE_ENABLE)
	{

		if (strcmp(bench, "deepsjeng") == 0)
		{
			endpoint = WARMUP_CACHE + WARMUP_TREE + 248000;
			TIMING_INTERVAL = T2_INTERVAL;
		}
		else if (strcmp(bench, "bwaves") == 0)
		{
			endpoint = TRACE_SIZE;
			TIMING_INTERVAL = T2_INTERVAL;
		}
		else if (strcmp(bench, "lbm") == 0)
		{
			endpoint = WARMUP_CACHE + WARMUP_TREE + 492000;
			TIMING_INTERVAL = T2_INTERVAL;
		}
		else if (strcmp(bench, "cam4") == 0)
		{
			endpoint = WARMUP_CACHE + WARMUP_TREE +  382000;
			TIMING_INTERVAL = T2_INTERVAL;
		}
		else if (strcmp(bench, "imagick") == 0)
		{
			endpoint = WARMUP_CACHE + WARMUP_TREE +  620000;
			TIMING_INTERVAL = T1_INTERVAL;
		}
		else if (strcmp(bench, "fotonik3d") == 0)
		{
			endpoint = WARMUP_CACHE + WARMUP_TREE + 327000;
			TIMING_INTERVAL = T1_INTERVAL;
		}
		else if (strcmp(bench, "roms") == 0)
		{
			endpoint = WARMUP_CACHE + WARMUP_TREE + 772000;
			TIMING_INTERVAL = T2_INTERVAL;
		}
		else if (strcmp(bench, "mcf") == 0)
		{
			endpoint = WARMUP_CACHE + WARMUP_TREE + 492000;
			TIMING_INTERVAL = T1_INTERVAL;
		}
		//  else if (strcmp(bench, "wrf") == 0) // this added for nonsec exp
		//  {
		// 	 endpoint = 3327000;
		// 	 TIMING_INTERVAL = T2_INTERVAL;
		//  }
		// else if (strcmp(bench, "bwaves") == 0) // this added for nonsec exp
		//  {
		// 	 endpoint = 3227000;
		// 	 TIMING_INTERVAL = T2_INTERVAL;
		//  }
		else
		{
			endpoint = TRACE_SIZE;
			TIMING_INTERVAL = T1_INTERVAL;
		}
	}
	else
	{
		endpoint = TRACE_SIZE;
		TIMING_INTERVAL = T1_INTERVAL;
	}
	



//.....................................................................
// 						ORAM initialization							  .
//.....................................................................

	// explore_lz();

	print_oram_params();

	var_init();

	// init_trace();

	
	// test_subtree();
	
	// test_footprint();

	// long long int inaddr = 0xfffffffff;
	// int addrout = byte_addr(2147483520);
	// printf("block: %d\n", BLOCK);
	// printf("out: %d\n", addrout);
	// printf("out: %x\n", addrout);
	// int xxx = block_addr(addrout);
	// printf("xxx: %x\n", xxx);
	

	// plb_test();

	// export_csv(argv);


	fflush(stdout);
	
	cache_init();

	plb_init();

	stt_init();

	table_init();
	
	oram_alloc();

	rho_alloc();

	if (SNAPSHOT_ENABLE)
	{
		oram_init_path();
		// oram_init();
		take_snapshot(argv);
	}
		
	if (!NONSEC_ENABLE)
	{
		oram_init();
		/* code */
	}
	
	printf("oram init is done.\n");

	fflush(stdout);

	// switch_enqueue_to(HEAD);
	// test_queue();
	

	// if (!SIM_ENABLE)
	// {
	// test_oram(argv);
	// }

	// random_test(argv);

	// test_ring();
	// test_oram(argv);

	if (tracectr < WARMUP_TREE)
	{
		switch_sim_enable_to(false);
		switch_cache_enable_to(false);
		// switch_dead_enable_to(false);
	}
	else
	{
		switch_sim_enable_to(SIM_ENABLE);
		switch_cache_enable_to(CACHE_ENABLE);
		// switch_dead_enable_to(DEAD_ENABLE);
	}
	
	

//.....................................................................


//   Mehrnoosh.

//   int cnt = 0;
	
//   signal(SIGINT, handle_sigint); 	
  while (!expt_done) {


	// Mehrnoosh:
	// printf("@ %d\n", tracectr);
	// printf("\n@ trace %d	writeq length: %lld \n", tracectr, write_queue_length[numc]);
	// print_count_level();
	// cnt++;
	// if (cnt > 1)
	// {
	// 	exit(1);
	// }


	if (SNAP_CACHE)
	{
		if (invokectr % 100000 == 0 )
		{
			printf("%dk\n\n",(int)(invokectr/1000));
			print_count_level();
			printf("\n\n\n\n");
		}
	}
	

	

	start = clock();

	no_miss_occured = true;

	if (tracectr >= TRACE_SIZE-3 || tracectr >= endpoint /* || mem_clk >= (TRACE_SIZE - WARMUP_CACHE) */)
	{
		break;
	}

	if (TIMEOUT_ENABLE && exe_time >= TIMEOUT_THRESHOLD)
	{
		printf("WARNING: reached timeout!\n\n");
		break;
	}
	
	
	gettimeofday(&sday, NULL);

	
	// if ((tracectr % 1000 == 0) /*&& tracectr > 9000 && tracectr != roundprev*/ )
	// {
	// 	// printf("\n...........................Partial Stat..............................\n");
	// 	printf("\n@ trace: %d	invoke ctr: %d   cache hit: %f%%\n", tracectr, invokectr, 100*(double)hitctr/(hitctr+missctr));
	// 	// printf("cache hit rate: %f%%\n", 100*(double)hitctr/(hitctr+missctr));
	// 	// printf("bk evict rate: %f%%\n", 100*(double)bkctr/invokectr);
	// 	// printf("cache evict rate wrt # miss: %f%%\n", 100*(double)evictctr/(missctr));
	// 	// printf("invoke ctr: 	%d\n", invokectr);
	// 	// printf("rho hit rate: %f%%\n", 100*(double)rho_hit/(invokectr));
	// 	// printf("rho bk evict rate: %f%%\n", 100*(double)rho_bkctr/rho_hit);
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
		// printf("in for\n");
      if (!ROB[numc].tracedone) { /* Try to fetch if EOF has not been encountered. */
        num_fetch = 0;
        while (((num_fetch < MAX_FETCH) && (ROB[numc].inflight != ROBSIZE) && (!writeqfull)) /* || ( !SIM_ENABLE && (tracectr < TRACE_SIZE ))*/ ) {
		// printf("in while q full\n");
			// printf("writeq isn't full\n");
          /* Keep fetching until fetch width or ROB capacity or WriteQ are fully consumed. */
	  /* Read the corresponding trace file and populate the tail of the ROB data structure. */
	  /* If Memop, then populate read/write queue.  Set up completion time. */


	  // Mehrnoosh:
	//   printf("@ while fetch trace\n");
		// if (plbQ->size > 1)
		// {
		// 	printf("plb queue: %d @ 	trace: %d\n", plbQ->size, tracectr);
		// }
	  

	  	// printf("\ncycle: %lld", CYCLE_VAL);
	  	// printf("\nfetch clk: %lld", trace_clk);
	  	if (TIMING_ENABLE)
		{
			mem_tick = (trace_clk % TIMING_INTERVAL == 0);					 // whether this cycle it's time to initiate a mem access ~~~> timing interval has reached
			mem_cycle = (mem_tick || still_same_access) ? true : false; 	 // whether this cycle should be processing a mem op

			mem_clk = mem_tick ? mem_clk + 1 : mem_clk;						 // a counter keeping track of # passed mem ticks so far

			if (RHO_ENABLE)
			{
				oram_tick = (mem_tick && (mem_clk % 3 == 0) )? true : false;	 // whether this cycle it's time to initiate an oram access	
				rho_tick = (mem_tick && !oram_tick)? true : false;				 // whether this cycle it's time to initiate a rho access	
			}
			else
			{
				oram_tick = mem_tick;
				rho_tick = false;
			}
			
			
			
			nonmemops_timing[numc] = (mem_cycle) ? 0 : 1;					 // # non mem ops shall be issued next, in case of mem cycle it would be none
			dummy_tick = (mem_tick && (nonmemops[numc] > 0))? true : false;	 // whether this cycle it's time to make a dummy access 
			
			

			dummy_oram = dummy_tick && oram_tick;							// whether the dummy access should be oram dummy
			dummy_rho = dummy_tick && rho_tick;								// whether the dummy access should be rho dummy

			if (RHO_ENABLE && oramQ->size != 0)
			{
				// if the current available access is not what it's supposed to be, make a dummy access instead
				dummy_oram = (oram_tick && oramQ->head->tree != ORAM) || dummy_oram;
				dummy_rho = (rho_tick && oramQ->head->tree != RHO) || dummy_rho;
			}

			//  bk eviction for timing
			if (mem_tick && !rho_tick && BK_EVICTION && bk_evict_needed())
			{
				dummy_tick = true;
				dummy_oram = true;
				// printf("@> mem tick  %d   trace %d\n", stashctr, tracectr);
			}
			
			// if (dummy_tick)
			// {
			// 	consec_dumctr++;
			// }
			// else if (mem_tick)
			// {
			// 	if (consec_dumctr != 0 )
			// 	{
			// 		printf("%d\n", consec_dumctr);
			// 	}
			// 	// printf("%d\n", consec_dumctr);
			// 	consec_dumctr = 0;
			// }
			
			
			
			
		}
		else
		{
			nonmemops_timing[numc] = nonmemops[numc];
			// printf("nonmemops:  %d    cycle %lld\n", nonmemops_timing[numc], CYCLE_VAL);
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
		// printf("nonmemops:  %d    cycle %lld\n", nonmemops_timing[numc], CYCLE_VAL);

		trace_clk++;

		// printf("nonmemops: %d\n", nonmemops[numc]);
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
			if (NONSEC_ENABLE)
			{
				last_read[numc] = true;
				last_read_served = false;
			}
			
			// if (SIM_ENABLE)
			// {
				insert_read(addr[numc], CYCLE_VAL, numc, ROB[numc].tail, instrpc[numc], oramid[numc], tree[numc], last_read[numc]);
			// }
			
			

			// invoke_oram(addr[numc], CYCLE_VAL, numc, ROB[numc].tail, instrpc[numc], 'R');

			// end = clock();
			// exe_time += ((double) (end - start)) / CLOCKS_PER_SEC;

			
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

				// if (SIM_ENABLE)
				// {
					insert_write(addr[numc], CYCLE_VAL, numc, ROB[numc].tail, oramid[numc], tree[numc]);
				// }

				// invoke_oram(addr[numc], CYCLE_VAL, numc, ROB[numc].tail, 0, 'W');

				// end = clock();
				// exe_time += ((double) (end - start)) / CLOCKS_PER_SEC;
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
		  printf("Panic.  Poor trace format. newstr: %s\n", newstr);
		  return -1;
		}
	      }
	      ROB[numc].tail = (ROB[numc].tail +1) % ROBSIZE;
	      ROB[numc].inflight++;
	      fetched[numc]++;
	      num_fetch++;

	      /* Done consuming one line of the trace file.  Read in the next. */
		// Mehrnoosh:

		ring_dummy = (stashctr >= DUMMY_TH) ? true : false;

		if (DUMMY_ENABLE && ring_dummy)
		{
			ring_access(-1);
		}
		

		if (oramQ->size == 0  && (!DUMMY_ENABLE || !ring_dummy) && (last_read_served || !WAIT_ENABLE))  /*&& last_read_served*/
		{
			if (invokectr != 0)
			{
				mem_req_latencies += CYCLE_VAL - mem_req_start;
			}
			
			// printf("@ oramQ->size == 0\n");
			if (((!TIMING_ENABLE || (dummy_oram && EARLY_ENABLE)) && BK_EVICTION && bk_evict_needed()) && !RING_ENABLE)
			{
				// printf("bk evict\n");
				background_eviction();
				skip_invokation = true; 
				bk_already_made = true;
			}
			else if (TIMING_ENABLE && dummy_tick)
			{
				// just skip the trace reading in case of dummy turn
				skip_invokation = true;
				dummy_already_made = true;
				// printf("\nskip on\n");
				if (dummy_oram)
				{
					if (EARLY_ENABLE)
					{
						early_writeback();
					}
					else if (PREFETCH_ENABLE)
					{	
						invoke_prefetch();
					}
					else
					{
						dummy_access(ORAM); 
					} 
				}
				else if (dummy_rho)
				{
					dummy_access(RHO); 	
				}
			}
			
			// cache enabled:
			// else if(CACHE_ENABLE)
			// {
				// printf("cache enable if: @ trace %d\n", tracectr);
				while ((no_miss_occured && !expt_done) || (!SIM_ENABLE_VAR && tracectr < TRACE_SIZE-3) )
				{
					// if (tracectr % 1000000 == 0)
					// {
					// 	printf("@ %d\n", tracectr);
					// }
					// if (tracectr >= 17000000)
					// {
					// 	printf("no miss %d\n", tracectr);
					// }
					// if (tracectr > 21000000 && tracectr % 500000 == 0)
					// {
					// 	printf("while %d\n", tracectr);
					// }
					
					if (SIM_ENABLE && !SIM_ENABLE_VAR && tracectr >= WARMUP_TREE)
					{
						switch_sim_enable_to(SIM_ENABLE);
						switch_cache_enable_to(CACHE_ENABLE);
						// switch_dead_enable_to(DEAD_ENABLE);
						break;
					}
					
					cache_clk++;
					if (fgets(newstr,MAXTRACELINESIZE,tif[numc])) {
						// printf("while readline trace ctr: %d  \n", tracectr);
						// if (fgets(shadstr,MAXTRACELINESIZE,shadtif[numc]))
						// {
						// 	if (sscanf(shadstr,"%d %c %Lx %Lx",&shad_nonmemops[numc],&shad_opertype[numc],&shad_addr[numc],&shad_instrpc[numc]) < 1) {
						// 				printf("SHADOW Panic.  Poor trace format.\n");
						// 				return -3;
						// 	}
						// 	next_trace = shad_addr[numc];
						// }
						// printf("newstr: %s		shad: %s\n", newstr, shadstr);
						// printf("newstr: %s\n", newstr);
						// printf("shadstr: %s\n", shadstr);
						
						if (waited_for_evicted[numc].valid)
						{
							// printf("evicted trace %d\n", tracectr);
							nonmemops[numc] = waited_for_evicted[numc].nonmemops;
							opertype[numc] = waited_for_evicted[numc].opertype;
							addr[numc] = waited_for_evicted[numc].addr;
							instrpc[numc] = waited_for_evicted[numc].instrpc;
							waited_for_evicted[numc].valid = false;
							// eviction_writeback[numc] = true;
							evictifctr++;
							
							// printf("main: evicted if addr: %lld\n", addr[numc]);
							if (tracectr >= WARMUP_CACHE && SIM_ENABLE_VAR)
							{
								break;
							}

							if (!SIM_ENABLE_VAR)
							{
								invoke_oram(addr[numc], CYCLE_VAL, numc, 0, instrpc[numc], opertype[numc]);
							}
							
						}
						

						if (sscanf(newstr,"%d %c",&nonmemops[numc],&opertype[numc]) > 0) {
								// if (tracectr >= 17000000)
								// {
								// 	printf("new trace %d\n", tracectr);
								// }
							
								tracectr++;
								tracectr_test++;
							if (opertype[numc] == 'R') {
								rctr++;
								instctr += nonmemops[numc] + 1;
								if (sscanf(newstr,"%d %c %Lx %Lx",&nonmemops[numc],&opertype[numc],&addr[numc],&instrpc[numc]) < 1) {
								printf("Panic.  Poor trace format. newstr: %s\n", newstr);
								return -4;
								}
							}
							else {
								if (opertype[numc] == 'W') {
									// reset_dirty_search();
									wctr++;
									instctr += nonmemops[numc] + 1;
									if (sscanf(newstr,"%d %c %Lx %Lx",&nonmemops[numc],&opertype[numc],&addr[numc],&instrpc[numc]) < 1) {
										printf("Panic.  Poor trace format. newstr: %s\n", newstr);
										return -3;
									}
								}
								else {
								printf("Panic.  Poor trace format. newstr: %s\n", newstr);
								return -2;
								}
							}
							addr[numc] = byte_addr(addr[numc]);
							// if (nonmemops[numc] == 10)
							// {
							// 	nonmemops[numc] = 200;
							// }
							
							if (CACHE_ENABLE_VAR)
							{
								// hit
								if ((cache_access(addr[numc], opertype[numc]) == HIT) || plb_contain(block_addr(addr[numc])) || stash_contain(block_addr(addr[numc])))
								{
									// pin idea for eraly wb
									if (EARLY_ENABLE && tracectr > WARMUP_CACHE)
									{
										if (opertype[numc] == 'W')
										{
											// int posblk = pos_calc(block_addr(addr[numc]), 1);
											// plb_access(posblk);
										}
									}
									
									
									hitctr++;
									hit_nonmemops += nonmemops[numc] + L2_LATENCY;
								}
								else // miss occurred
								{
									missctr++;
									if (opertype[numc] == 'R')
									{
										rmiss++;
									}
									else if (opertype[numc] == 'W')
									{
										wmiss++;
									}
									
									if (nonmemops[numc] > 100)
									{
										nonmemctr += (nonmemops[numc]/100);
										// nonmemctr++;
									}

									// prefetcher history collection
									if (PREFETCH_ENABLE)
									{
										unsigned int page = page_addr(addr[numc]);
										if (page != curr_page)
										{
											fill_access++;
											Event e = {.pc = curr_pc, .addr = curr_page, .offset = curr_offset};
											if ((table_access(e) == -1))
											{
												fill_miss++;
												if ((__builtin_popcount(curr_footprint) > 1))
												{
													table_fill(e, curr_footprint);
												}
												
											}
											
											curr_page = page;
											curr_pc = instrpc[numc];
											curr_offset = offset_val(addr[numc]);
											curr_footprint = 0;
											footprint_update(addr[numc]);

										}
										else
										{
											footprint_update(addr[numc]);
										}
									}
									

									
									// first serve the evicted block then next time serve this trace
									int victim;
									victim = cache_fill(addr[numc], opertype[numc]);
									// printf("shad: %d\n", shad_nonmemops[numc]);

									// if (shad_nonmemops[numc] == 10 && opertype[numc] == 'W' /*&& victim != -1*/)
									// {
									// 	missl1wb++;
									// }
									
									
									

									if (EARLY_ENABLE && tracectr > WARMUP_CACHE)
									{
										if (opertype[numc] == 'W')
										{
											// int posblk = pos_calc(block_addr(addr[numc]), 1);
											// plb_access(posblk);
										}
									}
				

									if ( victim != -1)
									{
										evictctr++;
										waited_for_evicted[numc].valid = true;
										waited_for_evicted[numc].nonmemops = MAINMEM_LATENCY;	
										waited_for_evicted[numc].opertype = opertype[numc];
										waited_for_evicted[numc].addr = addr[numc];
										waited_for_evicted[numc].instrpc = instrpc[numc];
										eviction_writeback[numc] = true;

										addr[numc] = victim;
										opertype[numc] = 'W';
										dirty_evict = true;
									}

									nonmemops[numc] += hit_nonmemops /* + L2_LATENCY */;
									hit_nonmemops = 0;
									nonmemops_sum += nonmemops[numc];

									

									no_miss_occured = false;

									if (!SIM_ENABLE_VAR)
									{
										
										invoke_oram(addr[numc], CYCLE_VAL, numc, 0, instrpc[numc], opertype[numc]);
								
									}
									

								}
								if (tracectr < WARMUP_CACHE)
								{
									no_miss_occured = true;
									waited_for_evicted[numc].valid = false;
									eviction_writeback[numc] = false;
									hitctr = 0;
									missctr = 0;
									evictctr = 0;
									cache_dirty = 0;
									nonmemops_sum = 0;
									missl1wb = 0;
								}
							}
							else if (!CACHE_ENABLE_VAR)
							{
								no_miss_occured = false;

								// if (tracectr > 1 && tracectr % 10000000 == 0)
								// {
								// 	int ind = tracectr/1000000;
								// 	export_csv_intermed(exp_name, ind, shuff_interval);
								// 	reset_shuff_interval();
								// }
								

								if (!SIM_ENABLE_VAR)
								{
									// bool edge = false;
									// if (tracectr == 22299999 || tracectr == 22299983 || tracectr == 22299982 || tracectr == 22299998)
									// {
									// 	printf("b4 %d\n", tracectr);
									// 	edge = true;
									// }
									// if (tracectr >= 17000000)
									// {
									// 	printf("invoke %d\n", tracectr);
									// }
									invoke_oram(addr[numc], CYCLE_VAL, numc, 0, instrpc[numc], opertype[numc]);
									// if (tracectr >= 17000000)
									// {
									// 	printf("invoked %d\n", tracectr);
									// }
									// if (edge)
									// {
									// 	printf("after %d\n", tracectr);
									// }
								}
							}
							
						}
						else {
							printf("Panic.  Poor trace format. newstr: %s\n", newstr);
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
						// if (SIM_ENABLE)
						// {
						// }
						
					}

				}

			// }
			// cache disabled:
			// else if (!CACHE_ENABLE)	
			// {
			// /* Done consuming one line of the trace file.  Read in the next. */
			// if (fgets(newstr,MAXTRACELINESIZE,tif[numc])) {
			// 	if (sscanf(newstr,"%d %c",&nonmemops[numc],&opertype[numc]) > 0) {
			// 		tracectr++;
			// if (opertype[numc] == 'R') {
			// 	if (sscanf(newstr,"%d %c %Lx %Lx",&nonmemops[numc],&opertype[numc],&addr[numc],&instrpc[numc]) < 1) {
			// 	printf("Panic.  Poor trace format. newstr: %s\n", newstr);
			// 	return -4;
			// 	}
			// }
			// else {
			// 	if (opertype[numc] == 'W') {
			// 	if (sscanf(newstr,"%d %c %Lx",&nonmemops[numc],&opertype[numc],&addr[numc]) < 1) {
			// 		printf("Panic.  Poor trace format. newstr: %s\n", newstr);
			// 		return -3;
			// 	}
			// 	}
			// 	else {
			// 	printf("Panic.  Poor trace format. newstr: %s\n", newstr);
			// 	return -2;
			// 	}
			// }
			// 	}
			// 	else {
			// 	printf("Panic.  Poor trace format. newstr: %s\n", newstr);
			// 	return -1;
			// 	}
			// 	}
			// 	else {
			// 		if (ROB[numc].inflight == 0) {
			// 		num_done++;
			// 		if (!time_done[numc]) time_done[numc] = CYCLE_VAL;
			// 		}
			// 		ROB[numc].tracedone=1;
			// 		break;  /* Break out of the while loop fetching instructions. */
			// 	}
			// 	addr[numc] = byte_addr(addr[numc]);
			// }

			if (eviction_writeback[numc])
			{
				write_cache_hit = true;
				eviction_writeback[numc] = false;
				if (RHO_ENABLE)
				{
					int masked_addr = block_addr(addr[numc]);
					if (rho_lookup(masked_addr) == -1)
					{
						rho_insert(addr[numc]);		// add evicted blk from llc to rho and consequently evicted blk from rho to oram
					}
				}
			}

			if (TIMING_ENABLE && !dummy_tick && RHO_ENABLE)
			{
				int masked_addr = block_addr(addr[numc]);
				if (rho_tick && (rho_lookup(masked_addr) == -1))			// if it misses on rho and it's rho turn should raise dummy rho
				{
					dummy_rho = true;
				}
				else if (oram_tick && (rho_lookup(masked_addr) != -1))		// if it hits on rho but it's oram turn should raise dummy oram
				{
					dummy_oram = true;
				}
				
			}
			
			// skip if dummy tick but if it's dummy rho or dummy oram due to rho miss or hit, invoke the oram anyway in order not to lose the trace just read 
			if (!skip_invokation)
			{
				// printf("invoke oram trace %d addr %lld\n", tracectr, addr[numc]);
				// if (invokectr % 10000 == 0)
				// {
				// 	printf("\n@ while exp  trace %d\n", tracectr);
				// 	print_count_level();
				// }
				// printf("@- %d\n", tracectr);
				if (!NONSEC_ENABLE)
				{
					invoke_oram(addr[numc], CYCLE_VAL, numc, 0, instrpc[numc], opertype[numc]); // ??? argumnets: cycle_val, numc, 0 are not actually used...
					oram_just_invoked = true;
					curr_trace = addr[numc];
					mem_req_start = CYCLE_VAL;
					dirty_evict = false;
				}
				// printf("@> %d\n", tracectr);

			}
			skip_invokation = false;
			// printf("\nskip off\n");
			
			
		} 
		if (oramQ->size != 0 && (last_read_served || !WAIT_ENABLE))  /*&& last_read_served*/
		{
			// printf("if nonzero oramq: %d   @ trace %d\n", oramQ->size, tracectr);

			if (((!TIMING_ENABLE || (dummy_oram && EARLY_ENABLE)) && BK_EVICTION && bk_evict_needed() && !bk_already_made) && !RING_ENABLE)
			{
				if (oramQ->size < QUEUE_SIZE - 2*oram_effective_pl)
				{
					background_eviction();
				}
				
			}
			// if a dummy access is not already made ~> it would be already made if it was dummy tick and queue was empty
			else if (TIMING_ENABLE && !dummy_already_made)
			{
				if (dummy_oram)
				{
					if (EARLY_ENABLE)
					{
						early_writeback();
					}
					else if (PREFETCH_ENABLE)
					{
						invoke_prefetch();
					}
					else
					{
						dummy_access(ORAM); 
					}
				}
				else if (dummy_rho)
				{
					dummy_access(RHO); 	
				}
			}
			else if (TIMING_ENABLE && dummy_already_made)
			{
				dummy_already_made = false;
			}

			bk_already_made = false;
			

			if (!still_same_access)
			{
				reqctr = 0;
				curr_access = oramQ->head->oramid;
				maxreq = (oramQ->head->tree == RHO) ? rho_effective_pl*2 : oram_effective_pl*2;
				trace_clk++;
				path_access_latency = CYCLE_VAL;
				// printf("\nfetch:  req: %d   max: %d	oramq: %d, curr:%d\n", reqctr, maxreq, oramQ->size, curr_access);	
			}
			
			// if (TIMING_ENABLE && nonmemops[numc] > 0)
			// {
			// 	nonmemops[numc]--;
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
			last_read[numc] = pN->last_read;
			if (pN->last_read)
			{
				last_read_served = false;
			}
			
			if (RING_ENABLE && NVM_ENABLE)
			{
				if (pN->nvm_access)
				{
					nonmemops[numc] += 750;
				}
				
			}
			
			
			// nonmemops[numc] = 0; // ??? not sure about this one ~~~> guess resolved
			
			// printf("\ndequeued: oramid: %d	req: %d \n", pN->oramid, reqctr);

			if (oram_just_invoked)
			{
				nonmemops[numc] = nonmemsaved; 	//  determined from the trace
				// printf("non mem saved: %d\n", nonmemops[numc]);
				oram_just_invoked = false;
			}
			

			if (pN->oramid != curr_access)
			{
				printf("\nERROR: deququed req %d does not match curr access %d @ trace %d\n", pN->oramid, curr_access, tracectr);
				exit(1);
			}

			if (reqctr == maxreq-1)
			{
				still_same_access = false;
				path_access_latency = CYCLE_VAL - path_access_latency;
				path_access_latency_avg = ((double)path_access_latency+path_access_latency_avg*(oramctr+dummyctr-1))/(oramctr+dummyctr);
				// printf("\ncurr access ended:  oramq: %d\n", oramQ->size);
					
			}
			
			reqctr++;
			free(pN);

		}

		

		// Mehrnoosh.














	      
	  }  /* Done consuming the next rd or wr. */
	// Mehrnoosh:
	// print_req_symbols();
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
	
	end = clock();
	exe_time += ((double) (end - start)) / CLOCKS_PER_SEC;
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


  export_csv(argv);


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



// printf("\nPrefetch #             %d\n", prefetchctr);
// printf("Pref pos1 #              %d\n", pos1ctr);
// printf("Pref pos2 #              %d\n", pos2ctr);
// printf("Pref pos1 hit            %f%%\n", 100*(double)pos1hit/pos1acc_ctr);
// printf("Pref pos2 hit            %f%%\n", 100*(double)pos2hit/pos2acc_ctr);
// printf("Pref pos1 hit #          %d\n", pos1hit);
// printf("Pref pos2 hit #          %d\n", pos2hit);
// printf("Pref pos1 acc #          %d\n", pos1acc_ctr);
// printf("Pref pos2 acc #          %d\n", pos2acc_ctr);
// printf("Pref fill hit            %f%%\n", 100*(double)(fill_access-fill_miss)/fill_access);
// printf("Pref fill hit #          %d\n", fill_access-fill_miss);
// printf("Pref fill access #       %d\n", fill_access);
// printf("Pref match hit           %f%%\n", 100*(double)(match_hit-(fill_access-fill_miss))/(hist_access-fill_access));
// printf("Pref match hit #         %d\n", match_hit-(fill_access-fill_miss));
// printf("Pref match access #      %d\n", hist_access-fill_access);

// printf("Pos1 conf #              %d\n", pos1conf);
// printf("Pos2 conf #              %d\n", pos2conf);
// printf("Case1 #                  %d\n", case1);
// printf("Case2 #                  %d\n", case2);
// printf("Case3 #                  %d\n", case3);
// printf("PLB pos1 #               %d\n", plbpos1);
// printf("Stash pos1 #             %d\n", stashpos1);
// printf("Buffer pos1 #            %d\n", bufferpos1);
// printf("Nonmemops #              %d\n\n", nonmemctr);

rmpki = (1000*rctr)/instctr;
wmpki = (1000*wctr)/instctr;

// print_oram_stats();



// printf("\n\n\n\n............... ORAM Stats ...............\n\n");
// printf("Execution Time (s)       %f\n", exe_time);
// printf("Total Cycles             %lld \n", CYCLE_VAL);
// printf("Trace Size               %d\n", tracectr);
// printf("Mem Cycles #             %lld\n", mem_clk);
// printf("Invoke Mem #             %d\n", invokectr);
// printf("ORAM Access #            %d\n", oramctr);
// printf("ORAM Dummy #             %d\n", dummyctr);
// printf("Pos1 Access #            %d\n", pos1_access);
// printf("Pos2 Access #            %d\n", pos2_access);
// printf("PLB pos0 hit             %f%%\n", 100*(double)plb_hit[0]/plbaccess[0]);
// printf("PLB pos1 hit             %f%%\n", 100*(double)plb_hit[1]/plbaccess[1]);
// printf("PLB pos2 hit             %f%%\n", 100*(double)plb_hit[2]/plbaccess[2]);
// printf("PLB pos0 hit #           %lld\n", plb_hit[0]);
// printf("PLB pos1 hit #           %lld\n", plb_hit[1]);
// printf("PLB pos2 hit #           %lld\n", plb_hit[2]);
// printf("PLB pos0 acc #           %lld\n", plbaccess[0]);
// printf("PLB pos1 acc #           %lld\n", plbaccess[1]);
// printf("PLB pos2 acc #           %lld\n", plbaccess[2]);
// printf("oramQ Size               %d\n", oramQ->size);
// printf("Bk Evict                 %f%%\n", 100*(double)bkctr/oramctr);
// printf("Bk Evict #               %d\n", bkctr);
// printf("Cache Hit                %f%%\n", 100*(double)hitctr/(hitctr+missctr));
// printf("Cache Evict              %f%%\n", 100*(double)evictctr/(missctr));
// printf("Rho Hit                  %f%%\n", 100*(double)rho_hit/(invokectr));
// printf("Rho Access #             %d\n", rhoctr);
// printf("Rho  Dummy #             %d\n", rho_dummyctr);
// printf("Rho Bk Evict             %f%%\n", 100*(double)rho_bkctr/rho_hit);
// printf("Early WB #               %d\n", earlyctr);
// printf("Early WB Pointer #       %d\n", dirty_pointctr);
// printf("Cache Dirty #            %d\n", cache_dirty);
// printf("ptr fail #               %d\n", ptr_fail);
// printf("search fail #            %d\n", search_fail);
// printf("pin ctr #                %d\n", pinctr);
// printf("unpin ctr #              %d\n", unpinctr);
// printf("prefetch case #          %d\n", precase);
// printf("STT Cand #               %d\n", sttctr);
// printf("Stash leftover #         %d\n", stash_leftover);
// printf("Stash removed #          %d\n", stash_removed);
// printf("fill hit #               %d\n", fillhit);
// printf("fill miss #              %d\n", fillmiss);
// printf("Top hit                  %f%%\n", 100*(double)topctr/(topctr+midctr+botctr));
// printf("Mid hit                  %f%%\n", 100*(double)midctr/(topctr+midctr+botctr));
// printf("Bot hit                  %f%%\n", 100*(double)botctr/(topctr+midctr+botctr));
// printf("Path Latency Avg         %f\n", path_access_latency_avg);
// printf("R ctr                    %lld\n", rctr);
// printf("W ctr                    %lld\n", wctr);
// printf("Inst ctr                 %lld\n", instctr);
      
// print_plb_stat();

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

// void handle_sigint(int sig) 
// { 
//     printf("Caught signal %d\n", sig);
// 	printf("\n\n\n\n............... ORAM Stats ...............\n\n");
// 	printf("Execution Time           %f s\n", exe_time);
// 	printf("Trace Size               %d\n", tracectr);
// 	printf("Mem Cycles #             %lld\n", mem_clk);
// 	printf("Invoke Mem #             %d\n", invokectr);
// 	printf("ORAM Access #            %d\n", oramctr);
// 	printf("Rho Access #             %d\n", rhoctr);
// 	printf("ORAM Dummy #             %d\n", dummyctr);
// 	printf("Rho  Dummy #             %d\n", rho_dummyctr);
// 	printf("Early WB #               %d\n", earlyctr);
// 	printf("Early WB Pointer #       %d\n", dirty_pointctr);
// 	printf("Cache Dirty #            %d\n", cache_dirty);
// 	printf("PLB pos0 hit             %f%%\n", 100*(double)plb_hit[0]/plbaccess[0]);
// 	printf("PLB pos1 hit             %f%%\n", 100*(double)plb_hit[1]/plbaccess[1]);
// 	printf("PLB pos2 hit             %f%%\n", 100*(double)plb_hit[2]/plbaccess[2]);
// 	printf("PLB pos0 hit #           %lld\n", plb_hit[0]);
// 	printf("PLB pos1 hit #           %lld\n", plb_hit[1]);
// 	printf("PLB pos2 hit #           %lld\n", plb_hit[2]);
// 	printf("PLB pos0 acc #           %lld\n", plbaccess[0]);
// 	printf("PLB pos1 acc #           %lld\n", plbaccess[1]);
// 	printf("PLB pos2 acc #           %lld\n", plbaccess[2]);
// 	printf("oramQ Size               %d\n", oramQ->size);
// 	printf("Bk Evict                 %f%%\n", 100*(double)bkctr/invokectr);
// 	printf("Cache Hit                %f%%\n", 100*(double)hitctr/(hitctr+missctr));
// 	printf("Cache Evict              %f%%\n", 100*(double)evictctr/(missctr));
// 	printf("Rho Hit                  %f%%\n", 100*(double)rho_hit/(invokectr));
// 	printf("Rho Bk Evict             %f%%\n", 100*(double)rho_bkctr/rho_hit); 
// }







