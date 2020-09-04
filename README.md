# MIPS Pipelined-Processor with Forwarding InC++
### IMPLEMENTATION:
The assignment is an extension of assignment 8 with forwarding. If forwarding is possible, the data is fetched from a suitable pipeline register, else the pipeline is stalled. Only for data hazards, forwarding is implemented.
### FEATURES:
If data required by some instrcution is evaluated in an R-type instruction, forwarding is possible and is done from the suitable pipeline register. For a load instruction immediatly followed by an instruction which requires the result of load, forwarding is not possible and pipeline is stalled for a clock cycle after which forwarding is possible and is done.
