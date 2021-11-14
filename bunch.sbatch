#!/bin/bash
#SBATCH --job-name=q1k
#SBATCH --account=xtang
#SBATCH --nodes=1  #number of nodes requested
#SBATCH --ntasks-per-node=1
#SBATCH --cluster=smp # mpi, gpu and smp are available in H2P
#SBATCH --partition=smp# available: smp, high-mem, opa, gtx1080, titanx, k40
#SBATCH --mail-user=mhrnshrf@gmail.com #send email to this address if ...
#SBATCH --mail-type=END,FAIL # ... job ends or fails
#SBATCH --time=0-06:00:00 # 6 days walltime in dd-hh:mm format
#SBATCH --cpus-per-task=7
#SBATCH --mem=128g

#module purge #make sure the modules environment is sane
#module load intel/2017.1.132 intel-mpi/2017.1.132 fhiaims/160328_3

cp trace4/gcc  $SLURM_SCRATCH # Copy inputs to scratch
mkdir $SLURM_SCRATCH/input
cp ../ORAM-Simulator/input/* $SLURM_SCRATCH/input
cp ../ORAM-Simulator/bin/usimm $SLURM_SCRATCH
cd $SLURM_SCRATCH
# Set a trap to copy any temp files you may need
run_on_exit(){
 cp -r $SLURM_SCRATCH/*.txt $SLURM_SUBMIT_DIR/log
 cp -r $SLURM_SCRATCH/*.csv $SLURM_SUBMIT_DIR/log
}
trap run_on_exit EXIT 

LOGGNAME="test"
echo "Running simulator..."
./usimm input/4channel.cfg gcc  $SLURM_JOB_NAME > "$SLURM_JOB_NAME-gcc.txt" 
#./usimm input/4channel.cfg mcf  $SLURM_JOB_NAME >"$SLURM_JOB_NAME-mcf.txt" 
#./usimm input/4channel.cfg omnetpp  $SLURM_JOB_NAME > "$SLURM_JOB_NAME-omnetpp.txt" 
#./usimm input/4channel.cfg xalancbmk  $SLURM_JOB_NAME > "$SLURM_JOB_NAME-xalancbmk.txt" 
#./usimm input/4channel.cfg x264  $SLURM_JOB_NAME > "$SLURM_JOB_NAME-x264.txt" 
#./usimm input/4channel.cfg deepsjeng  $SLURM_JOB_NAME > "$SLURM_JOB_NAME-deepsjeng.txt" 
#./usimm input/4channel.cfg bwaves  $SLURM_JOB_NAME > "$SLURM_JOB_NAME-bwaves.txt" 
#./usimm input/4channel.cfg lbm  $SLURM_JOB_NAME > "$SLURM_JOB_NAME-lbm.txt" 
#./usimm input/4channel.cfg wrf  $SLURM_JOB_NAME > "$SLURM_JOB_NAME-wrf.txt" 
#./usimm input/4channel.cfg cam4  $SLURM_JOB_NAME > "$SLURM_JOB_NAME-cam4.txt"  
#./usimm input/4channel.cfg imagick  $SLURM_JOB_NAME > "$SLURM_JOB_NAME-imagick.txt"  
#./usimm input/4channel.cfg fotonik3d  $SLURM_JOB_NAME > "$SLURM_JOB_NAME-fotonik3d.txt" 
#./usimm input/4channel.cfg roms  $SLURM_JOB_NAME > "$SLURM_JOB_NAME-roms.txt"
#./usimm input/4channel.cfg pop2  $SLURM_JOB_NAME > "$SLURM_JOB_NAME-pop2.txt"
#./usimm input/4channel.cfg leela  $SLURM_JOB_NAME > "$SLURM_JOB_NAME-leela.txt"
#./usimm input/4channel.cfg nab  $SLURM_JOB_NAME > "$SLURM_JOB_NAME-nab.txt"
#./usimm input/4channel.cfg xz  $SLURM_JOB_NAME > "$SLURM_JOB_NAME-xz.txt"
#./usimm input/4channel.cfg deepsjeng  $SLURM_JOB_NAME > "$SLURM_JOB_NAME-deepsjeng.txt"


crc-job-stats.py # gives stats of job, wall time, etc.

