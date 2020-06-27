# ORAM-Simulator
ORAM Simulator is written in C programming language. It simulates the behaviour of Oblivious RAM and memory part it is integrated with USIMM simulator to simulate the memory operations. USIMM is the Utah SImulated Memory Module.
<!-- reference shoud be added--> 

## Simulator Features
ORAM simulator implements a baseline ORAM that [Freecursive ORAM ](https://dl.acm.org/doi/10.1145/2775054.2694353) paper introduced. In this baseline, position map blocks and data blocks are stored in a unified full binary tree and one can not tell data access and position map access apart. On top of this baseline, ORAM Simulator implements other features. Some of these features are from prior work and other ones are contribution of [MIR-ORAM](https://dl.acm.org/doi/10.1145/2775054.2694353) paper. The list below summerizes all features. 
### MIR-ORAM Feautres
* __Volcano__     
* __Write Bypass__ 
* __Prefetch__    
### Prior Work Feautres
* __Cache__     
* __Subtree Address Mapping__ introduced by [Design space exploration and optimization of path oblivious RAM in secure processors](https://dl.acm.org/doi/10.1145/2508148.2485971) paper.
* __Rho__ introduced by [Relaxed Hierarchical ORAM](https://dl.acm.org/doi/10.1145/3297858.3304045) paper.
* __Timing Channel Protection__      


## ORAM Components
ORAM consists of several components: 
* __ORAM Tree__ a full binary tree that maintains all memory blocks
* __Position Map__ a table that maps each block to its correspoding path label
* __Stash__ a small fully assoicative cache that is a temporary buffer
* __PLB__ a small direct mapped cache that is position map lookaside buffer
