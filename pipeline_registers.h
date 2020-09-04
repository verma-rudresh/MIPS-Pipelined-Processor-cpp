
# include <string>
using namespace std;

typedef struct{
    bool isActive;      //check whether the stage is active or not.
    string next_pc;             //pc+4 - 32 bits
    string curr_instruc;        //current instruction - 32 bits
}IF_ID;

typedef struct{
    bool isActive;      //check whether the stage is active or not.
    string next_pc;             //pc+4 - 32 bits
    string Reg_read_data_1;     //first operand
    string Reg_read_data_2;     //second operand
    string Offset_sign_extended;//32 bits address
    string rt;                  //instruc_20_to_16_(rt)
    string rd;                  //instruc_15_to_11_(rd)

    struct{
        string Reg_write;       //1 bit
        string Reg_dest;        //1 bit
        string ALU_src;         //1 bit
        string Branch;          //1 bit
        string Mem_read;        //1 bit
        string Mem_write;       //1 bit
        string Mem_to_reg;      //1 bit
        string ALU_op;          //2 bits
    }Control;

    string opcode;              //5 bits for stalls
    string curr_instruc;        //32 bits
}ID_EX;

typedef struct{
    bool isActive;      //check whether the stage is active or not.
    string new_calc_pc;         //32 bits
    string ALU_result;          //32 bits
    string ALU_zero;            //1 bit
    string Reg_read_data_2;     //32 bits
    string dest_reg;            //5 bits - either rt or rd depending on the type
                                // of instruction: R type or load-store type

    struct{
        string Reg_write;       //1 bit
        string Reg_dest;        //1 bit
        string Branch;          //1 bit
        string Mem_read;        //1 bit
        string Mem_write;       //1 bit
        string Mem_to_reg;      //1 bit
    }Control;

    string curr_instruc;        //32 bits
}EX_MEM;

typedef struct{
    bool isActive;      //check whether the stage is active or not.
    string Mem_data_read;       //32 bits
    string ALU_result;          //32 bits
    string dest_reg;            //5 bits

    struct{
        string Reg_write;       //1 bit
        string Mem_to_reg;      //i bit
    }Control;

    string curr_instruc;
}MEM_WB;

// extern IF_ID if_id_reg;
// extern ID_EX id_ex_reg;
// extern EX_MEM ex_mem_reg;
// extern MEM_WB mem_wb_reg;

//Functions of control signals
/*
_______CONTROL SIGNALS_______                              
1. Reg_dest:
EFFECT WHEN DESSERTED(0): The register destination number for the 
                          Write register comes from the rt field (bits 20:16).  
EFFECT WHEN ASSERTED(1):  The register destination number for the Write register comes
                          from the rd field (bits 15:11).

2. Reg_write:
EFFECT WHEN DESSERTED(0): None.
EFFECT WHEN ASSERTED(1): The register on the Write register input is written with the value
                         on the Write data input.
               
3. ALU_src:
EFFECT WHEN DESSERTED(0): The second ALU operand comes from the second
                          register file output (Read data 2).
EFFECT WHEN ASSERTED(1): The second ALU operand is the sign-extended, lower 16 bits of
                         the instruction.

4. PC_src:
EFFECT WHEN DESSERTED(0): The PC is replaced by the output of the adder that
                          computes the value of PC + 4.
EFFECT WHEN ASSERTED(1): The PC is replaced by the output of the adder that computes
                         the branch target.    

5. Mem_read:
EFFECT WHEN DESSERTED(0): None.
EFFECT WHEN ASSERTED(1): Data memory contents designated by the address input are
                         put on the Read data output.

6. Mem_write:
EFFECT WHEN DESSERTED(0): None.
EFFECT WHEN ASSERTED(1): Data memory contents designated by the address input are
                         replaced by the value on the Write data input.

7. Mem_to_reg: 
EFFECT WHEN DESSERTED(0): The value fed to the register Write data input
                          comes from the ALU.
EFFECT WHEN ASSERTED(1): The value fed to the register Write data input comes from the
                         data memory.

8. ALU_op: when it is "01", then the instruction is R-format.
        when it is "00", then the instruction is load-store type.
        when it is "10", then the instruction is beq.
    The ALU_op is a input for the ALU CONTROL UNIT which gives the four bits vector as the output.

*/            
