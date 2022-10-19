/*
 * Copyright 2002-2019 Intel Corporation.
 * 
 * This software is provided to you as Sample Source Code as defined in the accompanying
 * End User License Agreement for the Intel(R) Software Development Products ("Agreement")
 * section 1.L.
 * 
 * This software and the related documents are provided as is, with no express or implied
 * warranties, other than those that are expressly stated in the License.
 */

/*
 *  This file contains an ISA-portable PIN tool for tracing memory accesses.
 */



//  Mehrnoosh:

#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
// FILE * trace;

// #define CACHE_SIZE 262144   // in bytes ~~~> 256 KB
#define CACHE_SIZE_L1 65536   // in bytes ~~~> 64 KB
#define NUM_WAY_L1 4           // bytes ~~~> # way per set
#define CACHE_SIZE_L2 262144   // in bytes ~~~> 256 KB
#define NUM_WAY_L2 8           // bytes ~~~> # way per set
#define BLOCK_SIZE 64       // bytes ~~~> cacheline size
#define ADDR_WIDTH 32       // bits
#define L1_LATENCY 3        // L1 latency in terms of # cycles 
#define L2_LATENCY 10       // L2 latency in terms of # cycles 
#define SKIP_THRESH 1790900000000       // skip threshold for dumping trace
// #define SKIP_THRESH 0       // skip threshold for dumping trace
#define CLK_PRINT 0       // 0/1 flag to print cache clk

enum reqType {CREAD = 'R', CWRITE = 'W'};
enum status {MISS = false, HIT = true};
enum eviction{NO_EVICT = -1};

long long int cache_clk = 0;
long long int viclru = 0;


enum{
  NUM_SET_L1 = (unsigned int)(CACHE_SIZE_L1/BLOCK_SIZE)/NUM_WAY_L1,  
  INDEX_WIDTH_L1 = (unsigned int)log2(NUM_SET_L1), 
  NUM_SET_L2 = (unsigned int)(CACHE_SIZE_L2/BLOCK_SIZE)/NUM_WAY_L2,  
  INDEX_WIDTH_L2 = (unsigned int)log2(NUM_SET_L2), 
  OFFSET_WIDTH = (unsigned int)log2(BLOCK_SIZE),
  TAG_WIDTH_L1 = (unsigned int)ADDR_WIDTH - INDEX_WIDTH_L1 - OFFSET_WIDTH,
  TAG_WIDTH_L2 = (unsigned int)ADDR_WIDTH - INDEX_WIDTH_L2 - OFFSET_WIDTH,
};


typedef struct Cacheline{
  bool valid;
  bool dirty;
  unsigned int tag;
  unsigned int addr;
  long long int LRU;
} Cacheline;

typedef struct Param{
   unsigned int NUM_SET; 
   unsigned int INDEX_WIDTH; 
   unsigned int NUM_WAY; 
} Param;

Param paramL1 = {.NUM_SET = NUM_SET_L1, .INDEX_WIDTH = INDEX_WIDTH_L1, .NUM_WAY = NUM_WAY_L1}; 
Param paramL2 = {.NUM_SET = NUM_SET_L2, .INDEX_WIDTH = INDEX_WIDTH_L2, .NUM_WAY = NUM_WAY_L2}; 

Cacheline **L1 = new Cacheline*[NUM_SET_L1];     // the cache
// for(int i = 0; i < NUM_SET_L1; ++i)
//     L1[i] = new int[NUM_WAY_L1];

Cacheline L2[NUM_SET_L2][NUM_WAY_L2];     // the cache
// long long int LRU[NUM_SET][NUM_WAY];                 // a array to keep track of lru for eviction

// invalidate all cahce blocks upon init
void cache_init(Cacheline **CC, Param paramCC){
    for (unsigned int i = 0; i < paramCC.NUM_SET; i++)
    {
        for (unsigned int j = 0; j < paramCC.NUM_WAY; j++)
        {
            CC[i][j].valid = false;
            CC[i][j].dirty = false;
            CC[i][j].tag = -1;
            
            CC[i][j].LRU = 0;
        }
    }
}

void update_LRU(unsigned int index, unsigned int way, Cacheline **CC){
    CC[index][way].LRU = cache_clk;
    // if (LRU[index][way] >= NUM_WAY - 1)
    // {
    //     LRU[index][way] = 0;
    // }
    // else
    // {
    //     LRU[index][way]++;
    // } 
}

void reset_LRU(unsigned int index, unsigned int way, Cacheline **CC){
    CC[index][way].LRU = cache_clk;
    // LRU[index][way] = 1;
}


int find_spot(unsigned int index, Cacheline **CC, Param paramCC){
    for (unsigned int j = 0; j < paramCC.NUM_WAY; j++)
    {
        if (!CC[index][j].valid)
        {
            return j;
        }
    }
    return -1;  
}


// find the cacheline with the least recently used
int find_victim(unsigned int index, Cacheline **CC, Param paramCC) {
    int victim = -1;
    // char min = NUM_WAY;
    long long int min = cache_clk;
    for (unsigned int j = 0; j < paramCC.NUM_WAY; j++)
    {
        if (CC[index][j].LRU < min)
        {
            victim = j;
            min = CC[index][j].LRU;

        }
    }

    return victim;
}



unsigned int get_index(unsigned int addr, Param paramCC){
    unsigned int index = addr >> OFFSET_WIDTH;
    // index = index << (TAG_WIDTH);
    // index = index >> (TAG_WIDTH);
    index = index & ((unsigned int)pow(2, paramCC.INDEX_WIDTH)-1);
    return index;

}

unsigned int get_tag(unsigned int addr, Param paramCC){
    unsigned int tag = addr >> (paramCC.INDEX_WIDTH + OFFSET_WIDTH);
    return tag;
}


// return true on hit and false on miss
bool cache_access(unsigned int addr, char type, FILE * trace, Cacheline **CC, Param paramCC){
    // fprintf(trace,"CA < get index\n");
    unsigned int index = get_index(addr, paramCC);
    // if(index < 0 || index >= NUM_SET){
    //     fprintf(trace,"ERROR\n");
    //     fprintf(trace,"index %d\n", index);
    //     exit(1);
    // }
    unsigned int tag = get_tag(addr, paramCC);
    // fprintf(trace,"CA > get tag\n");

    // fprintf(trace,"CA < L1 index %d \n", index);
    for (unsigned int j = 0; j < paramCC.NUM_WAY; j++)
    {
        // hit
        // fprintf(trace,"CA < L1 index %d   j %d\n", index, j);
        if (CC[index][j].tag == tag && CC[index][j].valid)
        {   
            // fprintf(trace,"CA > L1 index %d   j %d\n", index, j);
            if (type == CWRITE)
            {
                CC[index][j].dirty = true;
            }
            // fprintf(trace,"CA > dirty L1 index %d   j %d\n", index, j);
            update_LRU(index, j, CC);  
            // fprintf(trace,"CA > update L1 index %d   j %d\n", index, j);
            return true;    
        }        
    }
    // fprintf(trace,"CA > for loop\n");
    // miss
    return false;
}


// try to fill the cache with new data, it may lead to eviction ~~~> is called when miss happens
int cache_fill(unsigned int addr,  char type, FILE * trace, Cacheline **CC, Param paramCC){
    unsigned int index = get_index(addr, paramCC);
    unsigned int tag = get_tag(addr, paramCC);

    int victim = -1;
   
    // miss only
    int way = find_spot(index, CC, paramCC);

    // miss & evict
    if (way == -1)
    {
        way = find_victim(index, CC, paramCC);
        // if (way == -1)
        // {
        //     fprintf(trace,"ERROR\n");
        //     fprintf(trace,"way %d\n", way);
        //     exit(1);
        // }
        
        viclru = CC[index][way].LRU;
        if (CC[index][way].dirty)
        {
            victim = CC[index][way].addr;
        }
    }


    // cacheline fill
    CC[index][way].valid = true;
    CC[index][way].tag = tag;
    CC[index][way].addr = addr;
    CC[index][way].dirty = false;
    
    if (type == CWRITE)
    {
        CC[index][way].dirty = true;
    }
    
    reset_LRU(index, way, CC);  

    return victim;  
}

//  Mehrnoosh.




#include <stdio.h>
#include "pin.H"


FILE * trace;
int nonmemops = 0;
long long int access = 0;
long long int hit = 0;

long long int rctr = 0;
long long int wctr = 0;
long long int instctr = 0;

// Print a memory read record
VOID RecordMemRead(VOID * ip, VOID * addr)
{
    cache_clk++;
    access++;
    instctr++;
    nonmemops += L1_LATENCY;

    // get pointer value and store it in an usigned int
    char str[100];
    sprintf(str, "%p\n", addr);
    unsigned int addrval;
    sscanf(str,"%x", &addrval);
    int victim = -1;
    unsigned int v = 0;
    bool skip = false;

    // char buf[100];
    // sprintf(buf, "%p\n", ip);
    // unsigned int ipval;
    // sscanf(buf,"%x", &ipval);

    // fprintf(trace,"R < cache access\n");

	if (!cache_access(addrval, 'R', trace, L1, paramL1)) // miss
	{
        // fprintf(trace,"R > cache access\n");
        rctr++;
		victim = cache_fill(addrval, 'R', trace, L1, paramL1);

        // fprintf(trace,"R > cache fill\n");

		// if needed to evict a block
        // if (ipval != 0x7fbe48b439c0)
        {
            if (victim != -1)
            {
                v = (unsigned int)victim;
                // fprintf(trace,"%d W 0x%x %p     %f      %f  %lld\n", nonmemops, v,  ip, (double)hit/access, (double)(1000*wctr/(double)instctr), instctr);
                fprintf(trace,"%d W 0x%x %p\n", nonmemops, v,  ip);
                }
                nonmemops = L2_LATENCY;
        }
        // fprintf(trace,"R > victim print\n");
        // else
        // {
        //     skip = true;
        // }
        
        if (!skip){
	    // fprintf(trace,"%d R 0x%x %p     %f      %f  %lld\n", nonmemops, addrval, ip, (double)hit/access, (double)(1000*rctr/(double)instctr), instctr);
	    fprintf(trace,"%d R 0x%x %p\n", nonmemops, addrval, ip);
        }
	    nonmemops = 0;	

        // fprintf(trace,"R > miss print\n");
	}
	else	// hit
	{
        hit++;
	}

}

// Print a memory write record
VOID RecordMemWrite(VOID * ip, VOID * addr)
{
    // if (nonmemops != 12)
    // {
        cache_clk++;
        access++;
        instctr++;
        nonmemops += L1_LATENCY;

        // get pointer value and store it in an usigned int
        char str[100];
        sprintf(str, "%p\n", addr);
        unsigned int addrval;
        sscanf(str,"%x", &addrval);

        int victim = -1;
        unsigned int v = 0;
        bool skip = false;

        // char buf[100];
        // sprintf(buf, "%p\n", ip);
        // unsigned int ipval;
        // sscanf(buf,"%x", &ipval);

        // fprintf(trace,"W < cache access\n");
        if (!cache_access(addrval, 'W', trace, L1, paramL1)) // miss
        {
            // fprintf(trace,"W > cache access\n");

            wctr++;
            victim = cache_fill(addrval, 'W', trace, L1, paramL1);

            // fprintf(trace,"W > cache fill\n");
            // if needed to evict a block
            // if (ipval != 0x7fbe48b439c0)
			{
                if (victim != -1)
                {
                    v = (unsigned int)victim;
                    // fprintf(trace,"%d W 0x%x %p     %f      %f  %lld\n", nonmemops, v, ip, (double)hit/access, (double)(1000*wctr/(double)instctr), instctr);
                    fprintf(trace,"%d W 0x%x %p\n", nonmemops, v, ip);
                
                    
                    nonmemops = L2_LATENCY;
                }
            }
            // fprintf(trace,"W > victim print\n");
            // else
            // {
            //     skip = true;
            // }

            if (!skip){
            // fprintf(trace,"%d W 0x%x %p     %f      %f  %lld\n", nonmemops, addrval, ip, (double)hit/access, (double)(1000*wctr/(double)instctr), instctr);
            fprintf(trace,"%d W 0x%x %p\n", nonmemops, addrval, ip);
            }
            // fprintf(trace,"W > miss print\n");

            nonmemops = 0;	
            if (CLK_PRINT && cache_clk > SKIP_THRESH)
            {
                fprintf(trace,"%lld\n", cache_clk);
            }
            
        }
        else // hit
        {
            hit++;
        }

        /* code */
    // }
    
}


// Print other instruction that are not mem type
VOID RecordOtherInst(VOID * ip, VOID * addr)
{
    // fprintf(trace,"%lld N %p %p\n", instctr, addr, ip);
    instctr++;
    cache_clk++;
    nonmemops++;
}

// Is called for every instruction and instruments reads and writes
VOID Instruction(INS ins, VOID *v)
{
    // Instruments memory accesses using a predicated call, i.e.
    // the instrumentation is called iff the instruction will actually be executed.
    //
    // On the IA-32 and Intel(R) 64 architectures conditional moves and REP 
    // prefixed instructions appear as predicated instructions in Pin.
    UINT32 memOperands = INS_MemoryOperandCount(ins);

    if (memOperands == 0)
    {
    	    INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)RecordOtherInst, IARG_ADDRINT,
                   INS_Address(ins), IARG_PTR, std::string(INS_Disassemble(ins)),
                   IARG_END);
    }

    // Iterate over each memory operand of the instruction.
    for (UINT32 memOp = 0; memOp < memOperands; memOp++)
    {
        if (INS_MemoryOperandIsRead(ins, memOp))
        {
            INS_InsertPredicatedCall(
                ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead,
                IARG_INST_PTR,
                IARG_MEMORYOP_EA, memOp,
                IARG_END);
        }
        // Note that in some architectures a single memory operand can be 
        // both read and written (for instance incl (%eax) on IA-32)
        // In that case we instrument it once for read and once for write.
        if (INS_MemoryOperandIsWritten(ins, memOp))
        {
            INS_InsertPredicatedCall(
                ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite,
                IARG_INST_PTR,
                IARG_MEMORYOP_EA, memOp,
                IARG_END);
        }
    }
}

VOID Fini(INT32 code, VOID *v)
{
    fprintf(trace, "#eof\n");
    fclose(trace);
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */
   
INT32 Usage()
{
    PIN_ERROR( "This Pintool prints a trace of memory addresses\n" 
              + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char *argv[])
{
    if (PIN_Init(argc, argv)) return Usage();

    trace = fopen("pinatrace.out", "w");
    for(int i = 0; i < NUM_SET_L1; ++i){
        L1[i] = new Cacheline[NUM_WAY_L1];
    }
    cache_init(L1, paramL1);

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();
    
    return 0;
}
