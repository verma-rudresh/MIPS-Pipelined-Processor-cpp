# MIPS Pipelined-Processor with Forwarding InC++
### IMPLEMENTATION:
The assignment is an extension of assignment 8 with forwarding. If forwarding is possible, the data is fetched from a suitable pipeline register, else the pipeline is stalled. Only for data hazards, forwarding is implemented.
### FEATURES:
If data required by some instrcution is evaluated in an R-type instruction, forwarding is possible and is done from the suitable pipeline register. For a load instruction immediatly followed by an instruction which requires the result of load, forwarding is not possible and pipeline is stalled for a clock cycle after which forwarding is possible and is done.

### Functions of control signals
### _______CONTROL SIGNALS_______  

### 1. Reg_dest:
EFFECT WHEN DESSERTED(0): The register destination number for the 
                          Write register comes from the rt field (bits 20:16).  
EFFECT WHEN ASSERTED(1):  The register destination number for the Write register comes
                          from the rd field (bits 15:11).
### 2. Reg_write:
EFFECT WHEN DESSERTED(0): None.
EFFECT WHEN ASSERTED(1): The register on the Write register input is written with the value
                         on the Write data input.
               
### 3. ALU_src:
EFFECT WHEN DESSERTED(0): The second ALU operand comes from the second
                          register file output (Read data 2).
EFFECT WHEN ASSERTED(1): The second ALU operand is the sign-extended, lower 16 bits of
                         the instruction.
### 4. PC_src:
EFFECT WHEN DESSERTED(0): The PC is replaced by the output of the adder that
                          computes the value of PC + 4.
EFFECT WHEN ASSERTED(1): The PC is replaced by the output of the adder that computes
                         the branch target.    
### 5. Mem_read:
EFFECT WHEN DESSERTED(0): None.
EFFECT WHEN ASSERTED(1): Data memory contents designated by the address input are
                         put on the Read data output.
### 6. Mem_write:
EFFECT WHEN DESSERTED(0): None.
EFFECT WHEN ASSERTED(1): Data memory contents designated by the address input are
                         replaced by the value on the Write data input.
### 7. Mem_to_reg: 
EFFECT WHEN DESSERTED(0): The value fed to the register Write data input
                          comes from the ALU.
EFFECT WHEN ASSERTED(1): The value fed to the register Write data input comes from the
                         data memory.
### 8. ALU_op: when it is "01", then the instruction is R-format.
when it is "00", then the instruction is load-store type.
when it is "10", then the instruction is beq.
The ALU_op is a input for the ALU CONTROL UNIT which gives the four bits vector as the output.
