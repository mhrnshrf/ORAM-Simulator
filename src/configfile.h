
#ifndef __CONFIG_FILE_IN_H__
#define __CONFIG_FILE_IN_H__

#include "params.h"

#define 	EOL 	10
#define 	CR 	13
#define 	SPACE	32
#define		TAB	9

typedef enum {
	processor_clk_multiplier_token,
	robsize_token,
	max_retire_token,
	max_fetch_token,
	pipelinedepth_token,

	num_channels_token,
	num_ranks_token,
	num_banks_token,
	num_rows_token,
	num_columns_token,
	cache_line_size_token,
	address_bits_token,

	dram_clk_frequency_token,
	t_rcd_token,
	t_rp_token,
	t_cas_token,
	t_rc_token,
	t_ras_token,
	t_rrd_token,
	t_faw_token,
	t_wr_token,
	t_wtr_token,
	t_rtp_token,
	t_ccd_token,
	t_rfc_token,
	t_refi_token,
	t_cwd_token,
	t_rtrs_token,
	t_pd_min_token,
	t_xp_token,
	t_xp_dll_token,
	t_data_trans_token,

	vdd_token,
	idd0_token,
	idd2p0_token,
	idd2p1_token,
	idd2n_token,
	idd3p_token,
	idd3n_token,
	idd4r_token,
	idd4w_token,
	idd5_token,

	wq_capacity_token,
	address_mapping_token,
	wq_lookup_latency_token,

	TIMEOUT_ENABLE_token,
	SUBTREE_ENABLE_token , 
	CACHE_ENABLE_token   , 
	WRITE_BYPASS_token   , 
	RHO_ENABLE_token     , 
	TIMING_ENABLE_token  , 
	PREFETCH_ENABLE_token,
	EARLY_ENABLE_token  , 
	SNAPSHOT_ENABLE_token,
	NONSEC_ENABLE_token  , 
	BK_EVICTION_token    , 
	RHO_BK_EVICTION_token    , 
	SNAP_CACHE_token     , 
	RAND_ENABLE_token    , 
	WSKIP_ENABLE_token   , 
	SKIP_ENABLE_token    , 
	LINGER_BASE_token    , 
	DUMMY_ENABLE_token   , 
	DYNAMIC_EP_token     , 
	META_ENABLE_token    , 
	SIM_ENABLE_token     , 
	WAIT_ENABLE_token    , 
	LLC_DIRTY_token      , 

	TRACE_SIZE_token  			 ,
	QUEUE_SIZE_token  			 ,                  
	PAGE_SIZE_token ,
	L1_LATENCY_token ,
	L2_LATENCY_token ,
	MAINMEM_LATENCY_token ,
	WARMUP_CACHE_token ,
	TIMEOUT_THRESHOLD_token ,
	TOP_BOUNDARY_token ,
	MID_BOUNDARY_token ,
	TOP_CACHE_token ,
	CAP_LEVEL_token ,
	RHO_EMPTY_TOP_token ,
	RHO_TOP_CACHE_token ,
	T1_INTERVAL_token ,
	T2_INTERVAL_token ,
	RING_A_token ,
	RING_REV_token  ,
	EP_TURN_token  ,
	SKIP_LIMIT_token  ,
	SKIP_L1_token  ,
	SKIP_L2_token  ,
	DUMMY_TH_token  ,
	DEP_TH_token ,
	WL_CAP_token ,
	DEAD_ENABLE_token ,
	DEADQ_SIZE_token ,
	DEAD_GATHER_OFFSET_token ,
	REMOTE_ALLOC_RATIO_token ,
	NVM_ENABLE_token ,
	NVM_START_token ,
	WARMUP_TREE_token ,
	SURONLY_ENABLE_token ,
	CB_ENABLE_token ,
	CB_GREEN_MAX_token ,
	NVM_LATENCY_token ,
	REMOTE_START_OFF_token ,
	NVM_CHANNEL_token ,
	DYNAMIC_S_token ,
	S_INC_token ,
	REMOTE_CAP_token ,
	SHADQ_SIZE_token ,

	comment_token,
	unknown_token
}token_t;


token_t tokenize(char * input){
  size_t length;
  length = strlen(input);
  if(strncmp(input, "//",2) == 0) {
	return comment_token;
  } else if (strncmp(input, "PROCESSOR_CLK_MULTIPLIER",length) == 0) {
	return processor_clk_multiplier_token;
  } else if (strncmp(input, "ROBSIZE",length) == 0) {
	return robsize_token;
  } else if (strncmp(input, "MAX_RETIRE",length) == 0) {
	return max_retire_token;
  } else if (strncmp(input, "MAX_FETCH",length) == 0) {
	return max_fetch_token;
  } else if (strncmp(input, "PIPELINEDEPTH",length) == 0) {
	return pipelinedepth_token;
  } else if (strncmp(input, "NUM_CHANNELS",length) == 0) {
	return num_channels_token;
  } else if (strncmp(input, "NUM_RANKS",length) == 0) { 
	return num_ranks_token;
  } else if (strncmp(input, "NUM_BANKS",length) == 0) {
	return num_banks_token;
  } else if (strncmp(input, "NUM_ROWS",length) == 0) {
	return num_rows_token;
  } else if (strncmp(input, "NUM_COLUMNS",length) == 0) {
	return num_columns_token;
  } else if (strncmp(input, "CACHE_LINE_SIZE",length) == 0) {
	return cache_line_size_token;
  } else if (strncmp(input, "ADDRESS_BITS",length) == 0) {
	return address_bits_token;
  } else if (strncmp(input, "DRAM_CLK_FREQUENCY",length) == 0) {
	return dram_clk_frequency_token;
  } else if (strncmp(input, "T_RC",length) == 0) {
	return t_rc_token;
  } else if (strncmp(input, "T_RP",length) == 0) {
    return t_rp_token;
  } else if (strncmp(input, "T_CAS",length) == 0) {
    return t_cas_token;
  } else if (strncmp(input, "T_RCD",length) == 0) {
    return t_rcd_token;
  } else if (strncmp(input, "T_RAS",length) == 0) {
    return t_ras_token;
  } else if (strncmp(input, "T_RRD",length) == 0) {
    return t_rrd_token;
  } else if (strncmp(input, "T_FAW",length) == 0) {
    return t_faw_token;
  } else if (strncmp(input, "T_WR",length) == 0) {
	return t_wr_token;
  } else if (strncmp(input, "T_WTR",length) == 0) {
	return t_wtr_token;
  } else if (strncmp(input, "T_RTP",length) == 0) {
	return t_rtp_token;
  } else if (strncmp(input, "T_CCD",length) == 0) {
    return t_ccd_token;
  } else if (strncmp(input, "T_RFC",length) == 0) {
	return t_rfc_token;
  } else if (strncmp(input, "T_REFI",length) == 0) {
	return t_refi_token;
  } else if (strncmp(input, "T_CWD",length) == 0) {
	return t_cwd_token;
  } else if (strncmp(input, "T_RTRS",length) == 0) {
	return t_rtrs_token;
  } else if (strncmp(input, "T_PD_MIN",length) == 0) {
	return t_pd_min_token;
  } else if (strncmp(input, "T_XP",length) == 0) {
	return t_xp_token;
  } else if (strncmp(input, "T_XP_DLL",length) == 0) {
	return t_xp_dll_token;
  } else if (strncmp(input, "T_DATA_TRANS",length) == 0) {
	return t_data_trans_token;
  } else if (strncmp(input, "VDD",length) == 0) {
	return vdd_token;
  } else if (strncmp(input, "IDD0",length) == 0) {
	return idd0_token;
  } else if (strncmp(input, "IDD2P0",length) == 0) {
	return idd2p0_token;
  } else if (strncmp(input, "IDD2P1",length) == 0) {
	return idd2p1_token;
  } else if (strncmp(input, "IDD2N",length) == 0) {
	return idd2n_token;
  } else if (strncmp(input, "IDD3P",length) == 0) {
	return idd3p_token;
  } else if (strncmp(input, "IDD3N",length) == 0) {
	return idd3n_token;
  } else if (strncmp(input, "IDD4R",length) == 0) {
	return idd4r_token;
  } else if (strncmp(input, "IDD4W",length) == 0) {
	return idd4w_token;
  } else if (strncmp(input, "IDD5",length) == 0) {
	return idd5_token;
  } else if (strncmp(input, "WQ_CAPACITY",length) == 0) {
	return wq_capacity_token;
  } else if (strncmp(input, "ADDRESS_MAPPING",length) == 0) {
	return address_mapping_token;
  } else if (strncmp(input, "WQ_LOOKUP_LATENCY",length) == 0) {
	return wq_lookup_latency_token;
  } else if (strncmp(input, "TIMEOUT_ENABLE",length) == 0) {
	return TIMEOUT_ENABLE_token;
  } else if (strncmp(input, "SUBTREE_ENABLE",length) == 0) {
	return SUBTREE_ENABLE_token;
  } else if (strncmp(input, "CACHE_ENABLE",length) == 0) {
	return CACHE_ENABLE_token;
  } else if (strncmp(input, "WRITE_BYPASS",length) == 0) {
	return WRITE_BYPASS_token;
  } else if (strncmp(input, "RHO_ENABLE",length) == 0) {
	return RHO_ENABLE_token;
  } else if (strncmp(input, "TIMING_ENABLE",length) == 0) {
	return TIMING_ENABLE_token;
  } else if (strncmp(input, "PREFETCH_ENABLE",length) == 0) {
	return PREFETCH_ENABLE_token;
  } else if (strncmp(input, "EARLY_ENABLE",length) == 0) {
	return EARLY_ENABLE_token;
  } else if (strncmp(input, "SNAPSHOT_ENABLE",length) == 0) {
	return SNAPSHOT_ENABLE_token;
  } else if (strncmp(input, "NONSEC_ENABLE",length) == 0) {
	return NONSEC_ENABLE_token;
  } else if (strncmp(input, "BK_EVICTION",length) == 0) {
	return BK_EVICTION_token;
  } else if (strncmp(input, "RHO_BK_EVICTION",length) == 0) {
	return RHO_BK_EVICTION_token;
  } else if (strncmp(input, "SNAP_CACHE",length) == 0) {
	return SNAP_CACHE_token;
  } else if (strncmp(input, "RAND_ENABLE",length) == 0) {
	return RAND_ENABLE_token;
  } else if (strncmp(input, "WSKIP_ENABLE",length) == 0) {
	return WSKIP_ENABLE_token;
  } else if (strncmp(input, "SKIP_ENABLE",length) == 0) {
	return SKIP_ENABLE_token;
  } else if (strncmp(input, "LINGER_BASE",length) == 0) {
	return LINGER_BASE_token;
  } else if (strncmp(input, "DUMMY_ENABLE",length) == 0) {
	return DUMMY_ENABLE_token;
  } else if (strncmp(input, "DYNAMIC_EP",length) == 0) {
	return DYNAMIC_EP_token;
  } else if (strncmp(input, "META_ENABLE",length) == 0) {
	return META_ENABLE_token;
  } else if (strncmp(input, "SIM_ENABLE",length) == 0) {
	return SIM_ENABLE_token;
  } else if (strncmp(input, "WAIT_ENABLE",length) == 0) {
	return WAIT_ENABLE_token;
  } else if (strncmp(input, "LLC_DIRTY",length) == 0) {
	return LLC_DIRTY_token;
  } else if (strncmp(input, "TRACE_SIZE",length) == 0) {
	return TRACE_SIZE_token;
  } else if (strncmp(input, "QUEUE_SIZE",length) == 0) {
	return QUEUE_SIZE_token;
  } else if (strncmp(input, "PAGE_SIZE",length) == 0) {
	return PAGE_SIZE_token;
  } else if (strncmp(input, "L1_LATENCY",length) == 0) {
	return L1_LATENCY_token;
  } else if (strncmp(input, "L2_LATENCY",length) == 0) {
	return L2_LATENCY_token;
  } else if (strncmp(input, "MAINMEM_LATENCY",length) == 0) {
	return MAINMEM_LATENCY_token;
  } else if (strncmp(input, "WARMUP_CACHE",length) == 0) {
	return WARMUP_CACHE_token;
  } else if (strncmp(input, "TIMEOUT_THRESHOLD",length) == 0) {
	return TIMEOUT_THRESHOLD_token;
  } else if (strncmp(input, "TOP_BOUNDARY",length) == 0) {
	return TOP_BOUNDARY_token;
  } else if (strncmp(input, "MID_BOUNDARY",length) == 0) {
	return MID_BOUNDARY_token;
  } else if (strncmp(input, "TOP_CACHE",length) == 0) {
	return TOP_CACHE_token;
  } else if (strncmp(input, "RHO_EMPTY_TOP",length) == 0) {
	return RHO_EMPTY_TOP_token;
  } else if (strncmp(input, "RHO_TOP_CACHE",length) == 0) {
	return RHO_TOP_CACHE_token;
  } else if (strncmp(input, "T1_INTERVAL",length) == 0) {
	return T1_INTERVAL_token;
  } else if (strncmp(input, "T2_INTERVAL",length) == 0) {
	return T2_INTERVAL_token;
  } else if (strncmp(input, "RING_A",length) == 0) {
	return RING_A_token;
  } else if (strncmp(input, "EP_TURN",length) == 0) {
	return EP_TURN_token;
  } else if (strncmp(input, "SKIP_LIMIT",length) == 0) {
	return SKIP_LIMIT_token;
  } else if (strncmp(input, "SKIP_L1",length) == 0) {
	return SKIP_L1_token;
  } else if (strncmp(input, "SKIP_L2",length) == 0) {
	return SKIP_L2_token;
  } else if (strncmp(input, "DUMMY_TH",length) == 0) {
	return DUMMY_TH_token;
  } else if (strncmp(input, "DEP_TH",length) == 0) {
	return DEP_TH_token;
  } else if (strncmp(input, "WL_CAP",length) == 0) {
	return WL_CAP_token;
  } else if (strncmp(input, "DEAD_ENABLE",length) == 0) {
	return DEAD_ENABLE_token;
  } else if (strncmp(input, "DEADQ_SIZE",length) == 0) {
	return DEADQ_SIZE_token;
  } else if (strncmp(input, "DEAD_GATHER_OFFSET",length) == 0) {
	return DEAD_GATHER_OFFSET_token;
  } else if (strncmp(input, "REMOTE_ALLOC_RATIO",length) == 0) {
	return REMOTE_ALLOC_RATIO_token;
  } else if (strncmp(input, "NVM_ENABLE",length) == 0) {
	return NVM_ENABLE_token;
  } else if (strncmp(input, "NVM_START",length) == 0) {
	return NVM_START_token;
  } else if (strncmp(input, "WARMUP_TREE",length) == 0) {
	return WARMUP_TREE_token;
  } else if (strncmp(input, "SURONLY_ENABLE",length) == 0) {
	return SURONLY_ENABLE_token;
  } else if (strncmp(input, "CB_ENABLE",length) == 0) {
	return CB_ENABLE_token;
  } else if (strncmp(input, "CB_GREEN_MAX",length) == 0) {
	return CB_GREEN_MAX_token;
  } else if (strncmp(input, "NVM_LATENCY",length) == 0) {
	return NVM_LATENCY_token;
  } else if (strncmp(input, "REMOTE_START_OFF",length) == 0) {
	return REMOTE_START_OFF_token;
  } else if (strncmp(input, "NVM_CHANNEL",length) == 0) {
	return NVM_CHANNEL_token;
  } else if (strncmp(input, "DYNAMIC_S",length) == 0) {
	return DYNAMIC_S_token;
  } else if (strncmp(input, "S_INC",length) == 0) {
	return S_INC_token;
  } else if (strncmp(input, "REMOTE_CAP",length) == 0) {
	return REMOTE_CAP_token;
  } else if (strncmp(input, "SHADQ_SIZE",length) == 0) {
	return SHADQ_SIZE_token;

  }else {
	printf("PANIC :Unknown token %s\n",input);
	return unknown_token;
  }
}


void read_config_file(FILE * fin)
{
	char 	c;
	char 	input_string[256];
	int	input_int;
	float   input_float;

	while ((c = fgetc(fin)) != EOF){
		if((c != EOL) && (c != CR) && (c != SPACE) && (c != TAB)){
			fscanf(fin,"%s",&input_string[1]);
			input_string[0] = c;
		} else {
			fscanf(fin,"%s",&input_string[0]);
		}
		token_t input_field = tokenize(&input_string[0]);

		switch(input_field)
		{
			case comment_token:
				while (((c = fgetc(fin)) != EOL) && (c != EOF)){
					/*comment, to be ignored */
				}
				break;

			case processor_clk_multiplier_token:
				fscanf(fin,"%d",&input_int);
				PROCESSOR_CLK_MULTIPLIER =  input_int;
				break;

			case robsize_token:
				fscanf(fin,"%d",&input_int);
				ROBSIZE =  input_int;
				break;

			case max_retire_token:
				fscanf(fin,"%d",&input_int);
				MAX_RETIRE =  input_int;
				break;

			case max_fetch_token:
				fscanf(fin,"%d",&input_int);
				MAX_FETCH =  input_int;
				break;

			case pipelinedepth_token:
				fscanf(fin,"%d",&input_int);
				PIPELINEDEPTH =  input_int;
				break;


			case num_channels_token:
				fscanf(fin,"%d",&input_int);
				NUM_CHANNELS =  input_int;
				break;


			case num_ranks_token:
				fscanf(fin,"%d",&input_int);
				NUM_RANKS =  input_int;
				break;

			case num_banks_token:
				fscanf(fin,"%d",&input_int);
				NUM_BANKS =  input_int;
				break;

			case num_rows_token:
				fscanf(fin,"%d",&input_int);
				NUM_ROWS =  input_int;
				break;

			case num_columns_token:
				fscanf(fin,"%d",&input_int);
				NUM_COLUMNS =  input_int;
				break;

			case cache_line_size_token:
				fscanf(fin,"%d",&input_int);
				CACHE_LINE_SIZE =  input_int;
				break;

			case address_bits_token:
				fscanf(fin,"%d",&input_int);
				ADDRESS_BITS =  input_int;
				break;

			case dram_clk_frequency_token:
				fscanf(fin,"%d",&input_int);
				DRAM_CLK_FREQUENCY =  input_int;
				break;

			case t_rcd_token:
				fscanf(fin,"%d",&input_int);
				T_RCD = input_int*PROCESSOR_CLK_MULTIPLIER;
				break;

			case t_rp_token:
				fscanf(fin,"%d",&input_int);
				T_RP = input_int*PROCESSOR_CLK_MULTIPLIER;
				break;

			case t_cas_token:
				fscanf(fin,"%d",&input_int);
				T_CAS = input_int*PROCESSOR_CLK_MULTIPLIER;
				break;

			case t_rc_token:
				fscanf(fin,"%d",&input_int);
				T_RC = input_int*PROCESSOR_CLK_MULTIPLIER;
				break;

			case t_ras_token:
				fscanf(fin,"%d",&input_int);
				T_RAS = input_int*PROCESSOR_CLK_MULTIPLIER;
				break;

			case t_rrd_token:
				fscanf(fin,"%d",&input_int);
				T_RRD = input_int*PROCESSOR_CLK_MULTIPLIER;
				break;

			case t_faw_token:
				fscanf(fin,"%d",&input_int);
				T_FAW = input_int*PROCESSOR_CLK_MULTIPLIER;
				break;

			case t_wr_token:
				fscanf(fin,"%d",&input_int);
				T_WR = input_int*PROCESSOR_CLK_MULTIPLIER;
				break;

			case t_wtr_token:
				fscanf(fin,"%d",&input_int);
				T_WTR = input_int*PROCESSOR_CLK_MULTIPLIER;
				break;

			case t_rtp_token:
				fscanf(fin,"%d",&input_int);
				T_RTP = input_int*PROCESSOR_CLK_MULTIPLIER;
				break;

			case t_ccd_token:
				fscanf(fin,"%d",&input_int);
				T_CCD = input_int*PROCESSOR_CLK_MULTIPLIER;
				break;

			case t_rfc_token:
				fscanf(fin,"%d",&input_int);
				T_RFC = input_int*PROCESSOR_CLK_MULTIPLIER;
				break;

			case t_refi_token:
				fscanf(fin,"%d",&input_int);
				T_REFI = input_int*PROCESSOR_CLK_MULTIPLIER;
				break;

			case t_cwd_token:
				fscanf(fin,"%d",&input_int);
				T_CWD = input_int*PROCESSOR_CLK_MULTIPLIER;
				break;

			case t_rtrs_token:
				fscanf(fin,"%d",&input_int);
				T_RTRS = input_int*PROCESSOR_CLK_MULTIPLIER;
				break;

			case t_pd_min_token:
				fscanf(fin,"%d",&input_int);
				T_PD_MIN = input_int*PROCESSOR_CLK_MULTIPLIER;
				break;

			case t_xp_token:
				fscanf(fin,"%d",&input_int);
				T_XP = input_int*PROCESSOR_CLK_MULTIPLIER;
				break;

			case t_xp_dll_token:
				fscanf(fin,"%d",&input_int);
				T_XP_DLL = input_int*PROCESSOR_CLK_MULTIPLIER;
				break;

			case t_data_trans_token:
				fscanf(fin,"%d",&input_int);
				T_DATA_TRANS = input_int*PROCESSOR_CLK_MULTIPLIER;
				break;
			
			case vdd_token:
				fscanf(fin,"%f",&input_float);
				VDD = input_float;
				break;

			case idd0_token:
				fscanf(fin,"%f",&input_float);
				IDD0 = input_float;
				break;
			
			case idd2p0_token:
				fscanf(fin,"%f",&input_float);
				IDD2P0 = input_float;
				break;

			case idd2p1_token:
				fscanf(fin,"%f",&input_float);
				IDD2P1 = input_float;
				break;

			case idd2n_token:
				fscanf(fin,"%f",&input_float);
				IDD2N = input_float;
				break;

			case idd3p_token:
				fscanf(fin,"%f",&input_float);
				IDD3P = input_float;
				break;
			
			case idd3n_token:
				fscanf(fin,"%f",&input_float);
				IDD3N = input_float;
				break;

			case idd4r_token:
				fscanf(fin,"%f",&input_float);
				IDD4R = input_float;
				break;
			
			case idd4w_token:
				fscanf(fin,"%f",&input_float);
				IDD4W = input_float;
				break;
			
			case idd5_token:
				fscanf(fin,"%f",&input_float);
				IDD5 = input_float;
				break;

			case wq_capacity_token:
				fscanf(fin,"%d",&input_int);
				WQ_CAPACITY = input_int;
				break;

			case address_mapping_token:
				fscanf(fin,"%d",&input_int);
				ADDRESS_MAPPING= input_int;
				break;
			
			case wq_lookup_latency_token:
				fscanf(fin,"%d",&input_int);
				WQ_LOOKUP_LATENCY = input_int;
				break;
			case TIMEOUT_ENABLE_token:
				fscanf(fin,"%d",&input_int);
				TIMEOUT_ENABLE = input_int;
				break;
			case SUBTREE_ENABLE_token:
				fscanf(fin,"%d",&input_int);
				SUBTREE_ENABLE = input_int;
				break;
			case CACHE_ENABLE_token:
				fscanf(fin,"%d",&input_int);
				CACHE_ENABLE = input_int;
				break;
			case WRITE_BYPASS_token:
				fscanf(fin,"%d",&input_int);
				WRITE_BYPASS = input_int;
				break;
			case RHO_ENABLE_token:
				fscanf(fin,"%d",&input_int);
				RHO_ENABLE = input_int;
				break;
			case TIMING_ENABLE_token:
				fscanf(fin,"%d",&input_int);
				TIMING_ENABLE = input_int;
				break;
			case PREFETCH_ENABLE_token:
				fscanf(fin,"%d",&input_int);
				PREFETCH_ENABLE = input_int;
				break;
			case EARLY_ENABLE_token:
				fscanf(fin,"%d",&input_int);
				EARLY_ENABLE = input_int;
				break;
			case SNAPSHOT_ENABLE_token:
				fscanf(fin,"%d",&input_int);
				SNAPSHOT_ENABLE = input_int;
				break;
			case NONSEC_ENABLE_token:
				fscanf(fin,"%d",&input_int);
				NONSEC_ENABLE = input_int;
				break;
			case BK_EVICTION_token:
				fscanf(fin,"%d",&input_int);
				BK_EVICTION = input_int;
			case RHO_BK_EVICTION_token:
				fscanf(fin,"%d",&input_int);
				RHO_BK_EVICTION = input_int;
				break;
			case SNAP_CACHE_token:
				fscanf(fin,"%d",&input_int);
				SNAP_CACHE = input_int;
				break;
			case RAND_ENABLE_token:
				fscanf(fin,"%d",&input_int);
				RAND_ENABLE = input_int;
				break;
			case WSKIP_ENABLE_token:
				fscanf(fin,"%d",&input_int);
				WSKIP_ENABLE = input_int;
				break;
			case SKIP_ENABLE_token:
				fscanf(fin,"%d",&input_int);
				SKIP_ENABLE = input_int;
				break;
			case LINGER_BASE_token:
				fscanf(fin,"%d",&input_int);
				LINGER_BASE = input_int;
				break;
			case DUMMY_ENABLE_token:
				fscanf(fin,"%d",&input_int);
				DUMMY_ENABLE = input_int;
				break;
			case DYNAMIC_EP_token:
				fscanf(fin,"%d",&input_int);
				DYNAMIC_EP = input_int;
				break;
			case META_ENABLE_token:
				fscanf(fin,"%d",&input_int);
				META_ENABLE = input_int;
				break;
			case SIM_ENABLE_token:
				fscanf(fin,"%d",&input_int);
				SIM_ENABLE = input_int;
				break;
			case WAIT_ENABLE_token:
				fscanf(fin,"%d",&input_int);
				WAIT_ENABLE = input_int;
				break;
			case LLC_DIRTY_token:
				fscanf(fin,"%d",&input_int);
				LLC_DIRTY = input_int;
				break;
			case TRACE_SIZE_token:
				fscanf(fin,"%d",&input_int);
				TRACE_SIZE = input_int;
				break;
			case QUEUE_SIZE_token:
				fscanf(fin,"%d",&input_int);
				QUEUE_SIZE = input_int;
				break;
			case PAGE_SIZE_token:
				fscanf(fin,"%d",&input_int);
				PAGE_SIZE = input_int;
				break;
			case L1_LATENCY_token:
				fscanf(fin,"%d",&input_int);
				L1_LATENCY = input_int;
				break;
			case L2_LATENCY_token:
				fscanf(fin,"%d",&input_int);
				L2_LATENCY = input_int;
				break;
			case MAINMEM_LATENCY_token:
				fscanf(fin,"%d",&input_int);
				MAINMEM_LATENCY = input_int;
				break;
			case WARMUP_CACHE_token:
				fscanf(fin,"%d",&input_int);
				WARMUP_CACHE = input_int;
				break;
			case TIMEOUT_THRESHOLD_token:
				fscanf(fin,"%d",&input_int);
				TIMEOUT_THRESHOLD = input_int;
				break;
			case TOP_BOUNDARY_token:
				fscanf(fin,"%d",&input_int);
				TOP_BOUNDARY = input_int;
				break;
			case MID_BOUNDARY_token:
				fscanf(fin,"%d",&input_int);
				MID_BOUNDARY = input_int;
				break;
			case TOP_CACHE_token:
				fscanf(fin,"%d",&input_int);
				TOP_CACHE = input_int;
				break;
			case RHO_EMPTY_TOP_token:
				fscanf(fin,"%d",&input_int);
				RHO_EMPTY_TOP = input_int;
				break;
			case RHO_TOP_CACHE_token:
				fscanf(fin,"%d",&input_int);
				RHO_TOP_CACHE = input_int;
				break;
			case T1_INTERVAL_token:
				fscanf(fin,"%d",&input_int);
				T1_INTERVAL = input_int;
				break;
			case T2_INTERVAL_token:
				fscanf(fin,"%d",&input_int);
				T2_INTERVAL = input_int;
				break;
			case RING_A_token:
				fscanf(fin,"%d",&input_int);
				RING_A = input_int;
				break;
			case EP_TURN_token:
				fscanf(fin,"%d",&input_int);
				EP_TURN = input_int;
				break;
			case SKIP_LIMIT_token:
				fscanf(fin,"%d",&input_int);
				SKIP_LIMIT = input_int;
				break;
			case SKIP_L1_token:
				fscanf(fin,"%d",&input_int);
				SKIP_L1 = input_int;
				break;
			case SKIP_L2_token:
				fscanf(fin,"%d",&input_int);
				SKIP_L2 = input_int;
				break;
			case DUMMY_TH_token:
				fscanf(fin,"%d",&input_int);
				DUMMY_TH = input_int;
				break;
			case DEP_TH_token:
				fscanf(fin,"%d",&input_int);
				DEP_TH = input_int;
				break;
			case WL_CAP_token:
				fscanf(fin,"%d",&input_int);
				WL_CAP = input_int;
				break;
			case DEAD_ENABLE_token:
				fscanf(fin,"%d",&input_int);
				DEAD_ENABLE = input_int;
				break;
			case DEADQ_SIZE_token:
				fscanf(fin,"%d",&input_int);
				DEADQ_SIZE = input_int;
				break;
			case DEAD_GATHER_OFFSET_token:
				fscanf(fin,"%d",&input_int);
				DEAD_GATHER_OFFSET = input_int;
				break;
			case REMOTE_ALLOC_RATIO_token:
				fscanf(fin,"%f",&input_float);
				REMOTE_ALLOC_RATIO = input_float;
				break;
			case NVM_ENABLE_token:
				fscanf(fin,"%d",&input_int);
				NVM_ENABLE = input_int;
				break;
			case NVM_START_token:
				fscanf(fin,"%d",&input_int);
				NVM_START = input_int;
				break;
			case WARMUP_TREE_token:
				fscanf(fin,"%d",&input_int);
				WARMUP_TREE = input_int;
				break;
			case SURONLY_ENABLE_token:
				fscanf(fin,"%d",&input_int);
				SURONLY_ENABLE = input_int;
				break;
			case CB_ENABLE_token:
				fscanf(fin,"%d",&input_int);
				CB_ENABLE = input_int;
				break;
			case CB_GREEN_MAX_token:
				fscanf(fin,"%d",&input_int);
				CB_GREEN_MAX = input_int;
				break;
			case NVM_LATENCY_token:
				fscanf(fin,"%d",&input_int);
				NVM_LATENCY = input_int;
				break;
			case REMOTE_START_OFF_token:
				fscanf(fin,"%d",&input_int);
				REMOTE_START_OFF = input_int;
				break;
			case NVM_CHANNEL_token:
				fscanf(fin,"%d",&input_int);
				NVM_CHANNEL = input_int;
				break;
			case DYNAMIC_S_token:
				fscanf(fin,"%d",&input_int);
				DYNAMIC_S = input_int;
				break;
			case S_INC_token:
				fscanf(fin,"%d",&input_int);
				S_INC = input_int;
				break;
			case REMOTE_CAP_token:
				fscanf(fin,"%d",&input_int);
				REMOTE_CAP = input_int;
				break;
			case SHADQ_SIZE_token:
				fscanf(fin,"%d",&input_int);
				SHADQ_SIZE = input_int;
				break;

			case unknown_token:
			default:
				printf("PANIC: bad token in cfg file\n");
				break;

		}
	}
}


void print_params()
{
	printf("----------------------------------------------------------------------------------------\n");
	printf("------------------------\n");
	printf("- SIMULATOR PARAMETERS -\n");
	printf("------------------------\n");
	printf("\n-------------\n");
	printf("- PROCESSOR -\n");
	printf("-------------\n");
	printf("PROCESSOR_CLK_MULTIPLIER:   %6d\n", PROCESSOR_CLK_MULTIPLIER);
	printf("ROBSIZE:                    %6d\n", ROBSIZE);
	printf("MAX_FETCH:                  %6d\n", MAX_FETCH);
	printf("MAX_RETIRE:                 %6d\n", MAX_RETIRE);
	printf("PIPELINEDEPTH:              %6d\n", PIPELINEDEPTH);          

	printf("\n---------------\n");
	printf("- DRAM Config -\n");
	printf("---------------\n");
	printf("NUM_CHANNELS:               %6d\n", NUM_CHANNELS);
  printf("NUM_RANKS:                  %6d\n", NUM_RANKS);
  printf("NUM_BANKS:                  %6d\n", NUM_BANKS);
  printf("NUM_ROWS:                   %6d\n", NUM_ROWS);
  printf("NUM_COLUMNS:                %6d\n", NUM_COLUMNS);

	printf("\n---------------\n");
	printf("- DRAM Timing -\n");
	printf("---------------\n");
	printf("T_RCD:                      %6d\n", T_RCD);
  printf("T_RP:                       %6d\n", T_RP);
  printf("T_CAS:                      %6d\n", T_CAS);
  printf("T_RC:                       %6d\n", T_RC);
  printf("T_RAS:                      %6d\n", T_RAS);
  printf("T_RRD:                      %6d\n", T_RRD);
  printf("T_FAW:                      %6d\n", T_FAW);
  printf("T_WR:                       %6d\n", T_WR);
  printf("T_WTR:                      %6d\n", T_WTR);
  printf("T_RTP:                      %6d\n", T_RTP);
  printf("T_CCD:                      %6d\n", T_CCD);
  printf("T_RFC:                      %6d\n", T_RFC);
  printf("T_REFI:                     %6d\n", T_REFI);
  printf("T_CWD:                      %6d\n", T_CWD);
  printf("T_RTRS:                     %6d\n", T_RTRS);
  printf("T_PD_MIN:                   %6d\n", T_PD_MIN);
  printf("T_XP:                       %6d\n", T_XP);
  printf("T_XP_DLL:                   %6d\n", T_XP_DLL);
  printf("T_DATA_TRANS:               %6d\n", T_DATA_TRANS);

	printf("\n---------------------------\n");
	printf("- DRAM Idd Specifications -\n");
	printf("---------------------------\n");

	printf("VDD:                        %05.2f\n", VDD);
  printf("IDD0:                       %05.2f\n", IDD0);
  printf("IDD2P0:                     %05.2f\n", IDD2P0);
  printf("IDD2P1:                     %05.2f\n", IDD2P1);
  printf("IDD2N:                      %05.2f\n", IDD2N);
  printf("IDD3P:                      %05.2f\n", IDD3P);
  printf("IDD3N:                      %05.2f\n", IDD3N);
  printf("IDD4R:                      %05.2f\n", IDD4R);
  printf("IDD4W:                      %05.2f\n", IDD4W);
  printf("IDD5:                       %05.2f\n", IDD5);

	printf("\n-------------------\n");
	printf("- DRAM Controller -\n");
	printf("-------------------\n");
	printf("WQ_CAPACITY:                %6d\n", WQ_CAPACITY);
  printf("ADDRESS_MAPPING:            %6d\n", ADDRESS_MAPPING);
  printf("WQ_LOOKUP_LATENCY:          %6d\n", WQ_LOOKUP_LATENCY);
	printf("\n----------------------------------------------------------------------------------------\n");


}
	

#endif // __CONFIG_FILE_IN_H__
