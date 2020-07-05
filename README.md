# MIR-ORAM Simulator
MIR-ORAM Simulator is written in C programming language. It simulates the behaviour of path **O**blivious **RAM** and on top of that implements several features for **M**emory request **I**ntensity **R**eduction. For the memory side, it is integrated with [USIMM](http://utaharch.blogspot.com/2012/02/usimm.html) (the Utah SImulated Memory Module) that simulates the memory operations. 
<!-- reference shoud be added--> 

## Simulator Features
ORAM simulator implements a baseline ORAM that [Freecursive ORAM ](https://dl.acm.org/doi/10.1145/2775054.2694353) paper introduced. In this baseline, position map blocks and data blocks are stored in a unified full binary tree and one can not tell data access and position map access apart. On top of this baseline, MIR-ORAM Simulator implements other features. Some of these features are from prior work and other ones are contribution of [MIR-ORAM](https://dl.acm.org/doi/10.1145/2775054.2694353) paper. The list below summerizes all features. 
### MIR-ORAM Feautres
* __Volcano__     
* __Write Bypass__ 
* __Prefetch__    
### Prior Work Feautres
* __Cache__   a set associative last level cache
* __Subtree__ an efficient address mapping introduced by [Design space exploration and optimization of path oblivious RAM in secure processors](https://dl.acm.org/doi/10.1145/2508148.2485971) paper.
* __Rho__ the state-of-the-art introduced by [Relaxed Hierarchical ORAM](https://dl.acm.org/doi/10.1145/3297858.3304045) paper.
* __Timing Channel Protection__ a mechanisim to prevent information leakage from memory requests timing 


## Simulator Components
The baseline Freecursive ORAM consists of several components: 
* __ORAM Tree__ a full binary tree that maintains all memory blocks
* __Position Map__ a table that maps each block in the address space to its correspoding path label
* __Stash__ a small fully assoicative cache that is a temporary buffer
* __PLB__ a small direct mapped cache that is called **P**osition map **L**ookaside **B**uffer
