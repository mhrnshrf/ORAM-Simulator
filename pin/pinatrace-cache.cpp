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

#define CACHE_SIZE 524288   // in bytes ~~~> 512 KB
#define NUM_WAY 2           // bytes ~~~> # way per set
#define BLOCK_SIZE 64       // bytes ~~~> cacheline size
#define ADDR_WIDTH 32       // bits
#define L1_LATENCY 3        // L1 latency in terms of # cycles 
#define L2_LATENCY 10        // L2 latency in terms of # cycles 

enum reqType {CREAD = 'R', CWRITE = 'W'};
enum status {MISS = false, HIT = true};
enum eviction{NO_EVICT = -1};


enum{
  NUM_SET = (unsigned int)(CACHE_SIZE/BLOCK_SIZE)/NUM_WAY,  // # sets
  INDEX_WIDTH = (unsigned int)log2(NUM_SET), 
  OFFSET_WIDTH = (unsigned int)log2(BLOCK_SIZE),
  TAG_WIDTH = (unsigned int)ADDR_WIDTH - INDEX_WIDTH - OFFSET_WIDTH,
};


typedef struct Cacheline{
  bool valid;
  bool dirty;
  unsigned int tag;
  unsigned int addr;
} Cacheline;



Cacheline L1[NUM_SET][NUM_WAY];     // the cache
char LRU[NUM_SET][NUM_WAY];                  // a array to keep track of lru for eviction

// invalidate all cahce blocks upon init
void cache_init(){
    for (unsigned int i = 0; i < NUM_SET; i++)
    {
        for (unsigned int j = 0; j < NUM_WAY; j++)
        {
            L1[i][j].valid = false;
            L1[i][j].dirty = false;
            L1[i][j].tag = -1;
            
            LRU[i][j] = 0;
        }
    }
}

void update_LRU(unsigned int index, unsigned int way){
    if (LRU[index][way] == 255)
    {
        LRU[index][way] = 0;
    }
    else
    {
        LRU[index][way]++;
    } 
}

void reset_LRU(unsigned int index, unsigned int way){
    LRU[index][way] = 1;

}


int find_spot(unsigned int index){
    for (unsigned int j = 0; j < NUM_WAY; j++)
    {
        if (!L1[index][j].valid)
        {
            return j;
        }
    }
    return -1;  
}


// find the cacheline with the least recently used
int find_victim(unsigned int index) {
    int victim = -1;
    for (int j = 0; j < NUM_WAY; j++)
    {
        int min = 256;
        if (LRU[index][j] < min)
        {
            victim = j;
        }
    }

    return victim;
}



unsigned int get_index(unsigned int addr){
    unsigned int index = addr << TAG_WIDTH;
    index = index >> (TAG_WIDTH+OFFSET_WIDTH);
    return index;

}

unsigned int get_tag(unsigned int addr){
    unsigned int tag = addr >> (INDEX_WIDTH+OFFSET_WIDTH);
    return tag;
}


// return true on hit and false on miss
bool cache_access(unsigned int addr, char type){
    unsigned int index = get_index(addr);
    unsigned int tag = get_tag(addr);

    for (unsigned int j = 0; j < NUM_WAY; j++)
    {
        // hit
        if (L1[index][j].tag == tag && L1[index][j].valid)
        {   
            if (type == CWRITE)
            {
                L1[index][j].dirty = true;
            }
            update_LRU(index, j);  
            return true;    
        }        
    }
    // miss
    return false;
}


// try to fill the cache with new data, it may lead to eviction ~~~> is called when miss happens
int cache_fill(unsigned int addr,  char type){
    unsigned int index = get_index(addr);
    unsigned int tag = get_tag(addr);

    int victim = -1;
   
    // miss only
    int way = find_spot(index);

    // miss & evict
    if (way == -1)
    {
        way = find_victim(index);
        if (L1[index][way].dirty)
        {
            victim = L1[index][way].addr;
        }
    }


    // cacheline fill
    if (type == CWRITE)
    {
        L1[index][way].dirty = true;
    }
    L1[index][way].valid = true;
    L1[index][way].tag = tag;
    L1[index][way].addr = addr;
    reset_LRU(index, way);  

    return victim;  
}

//  Mehrnoosh.




#include <stdio.h>
#include "pin.H"


FILE * trace;
int nonmemops = 0;

// Print a memory read record
VOID RecordMemRead(VOID * ip, VOID * addr)
{
	if (!cache_access(*(unsigned int*)addr, 'R')) // miss
	{
	    fprintf(trace,"%d R %p %p\n", nonmemops, addr, ip);
	    nonmemops = 0;	

		long long int victim = cache_fill(*(unsigned int*)addr, 'R');
        unsigned long long int v = (unsigned long long int)victim;
		// if needed to evict a block
		if (victim != -1)
		{
			fprintf(trace,"%d W 0x%llx %p\n", nonmemops, v,  ip);
		}
	}
	else	// hit
	{
		nonmemops++;
	}

}

// Print a memory write record
VOID RecordMemWrite(VOID * ip, VOID * addr)
{
	if (!cache_access(*(unsigned int*)addr, 'W')) // miss
	{
	    fprintf(trace,"%d W %p %p\n", nonmemops, addr, ip);
	    nonmemops = 0;	

		long long int victim = cache_fill(*(unsigned int*)addr, 'W');
        unsigned long long int v = (unsigned long long int)victim;
		// if needed to evict a block
		if (victim != -1)
		{
			fprintf(trace,"%d W 0x%llx %p\n", nonmemops, v, ip);
		}
	}
	else // hit
	{
		nonmemops++;
	}
}


// Print other instruction that are not mem type
VOID RecordOtherInst(VOID * ip, VOID * addr)
{
    //fprintf(trace,"%p: N %p\n", ip, addr);
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

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();
    
    return 0;
}
