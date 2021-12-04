#!/bin/bash
#SBATCH --job-name=one-nonsec
#SBATCH --account=xtang
#SBATCH --nodes=1  #number of nodes requested
#SBATCH --ntasks-per-node=1
#SBATCH --cluster=smp # mpi, gpu and smp are available in H2P
#SBATCH --partition=smp# available: smp, high-mem, opa, gtx1080, titanx, k40
#SBATCH --mail-user=mhrnshrf@gmail.com #send email to this address if ...
#SBATCH --mail-type=END,FAIL # ... job ends or fails
#SBATCH --time=0-03:00:00 # 6 days walltime in dd-hh:mm format
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=16g
#SBATCH --array=1-13 # job array index

#module purge #make sure the modules environment is sane
#module load intel/2017.1.132 intel-mpi/2017.1.132 fhiaims/160328_3

#mkdir $SLURM_SCRATCH/trace
cp trace/*  $SLURM_SCRATCH # Copy inputs to scratch
mkdir $SLURM_SCRATCH/input
cp ../ORAM-Simulator/input/* $SLURM_SCRATCH/input
mkdir $SLURM_SCRATCH/bin
cp ../ORAM-Simulator/bin/usimm $SLURM_SCRATCH/bin
cd $SLURM_SCRATCH
# Set a trap to copy any temp files you may need
run_on_exit(){
 cp -r $SLURM_SCRATCH/*.txt $SLURM_SUBMIT_DIR/log
 cp -r $SLURM_SCRATCH/*.csv $SLURM_SUBMIT_DIR/log
}
trap run_on_exit EXIT 

echo "Running simulator..."

# get file name
file=`ls * | head -n $SLURM_ARRAY_TASK_ID | tail -n 1`
 
echo "Memory trace: "$file


bin/usimm input/4channel.cfg $file  $SLURM_JOB_NAME > "$SLURM_JOB_NAME-$file.txt" 


crc-job-stats.py # gives stats of job, wall time, etc.
