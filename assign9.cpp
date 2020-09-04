#include <bits/stdc++.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <unistd.h>
#include "./machine_code_generator.h"
#include "./pipeline_registers.h"
#include <string.h>

#define REG_ARRAY_SIZE 32
#define INSTRUC_SIZE 32
#define INSTRUC_ARRAY_SIZE 1024
#define MEM_ARRAY_SIZE 3072

string register_Array[REG_ARRAY_SIZE];
string instruc_Array[INSTRUC_ARRAY_SIZE];
string memory_Array[MEM_ARRAY_SIZE];

// global declarations
//pipeline registers.
IF_ID if_id_reg;
ID_EX id_ex_reg;
EX_MEM ex_mem_reg;
MEM_WB mem_wb_reg;
// ______________________
int pc = -1;
int branch_pc = 0;
int pc_Src = 0;
int return_address = 0;
int cc = 0;
int num_instructions = 0;
string j_addr = "";

int binary_to_dec(string b)    //treats input as unsigned
{
    int ans=0;
    int l = b.length();
    for(int i=0;i<l;i++)
    {
        if(b[l-i-1]=='1') ans = ans + pow(2,i);
    }
    return ans;
}

string dec_to_binary(int d,int size)
{
    string ans="";
    
    if(d==0) {ans = "0";}

    while(d != 0)
    {
        if(d%2==0) ans = "0" + ans;
        else ans = "1" + ans;
        d=d/2;
    }

    while(ans.length() < size) ans = "0"+ans;
    
    return ans;
}

string num_to_str(int num){
    string x = "";
    if(num==0) x="0";
    else if(num==1) x= "1";
    else if(num==2) x= "2";
    else if(num==3) x= "3";
    else if(num==4) x= "4";
    else if(num==5) x= "5";
    else if(num==6) x= "6";
    else if(num==7) x= "7";
    else if(num==8) x= "8";
    else if(num==9) x= "9";
    else if(num==10) x= "A";
    else if(num==11) x= "B";
    else if(num==12) x= "C";
    else if(num==13) x= "D";
    else if(num==14) x= "E";
    else if(num==15) x= "F";
    else x = "";
    return x;
}

string dec_to_hexaDec(int num){
    string number = "";
    if(num==0) return "0";

    while(num!=0){
        int x = num%16;
        number = num_to_str(x) + number;
        num = num/16;  
    }
    number = "0X" + number;
    return number;
}

string sign_extend(string s,int size)
{
    char msb = s[0];
    while(s.length() < size) s = msb+s;
    return s;
}

void initialize_instruc_array()
{
    for(int i=0;i<INSTRUC_ARRAY_SIZE;i++) instruc_Array[i] = "00000000000000000000000000000000";

    ifstream infile;
    string filename = "./machineCode";
    string line;
    infile.open(filename.c_str());
    int pos = 0;
    getline(infile, line);
    getline(infile, line);
    while(getline(infile, line)){
        instruc_Array[pos] = line;
        pos++;
    }
}

void initialize_reg_array()
{
    for(int i=0; i<32; i++) register_Array[i] = dec_to_binary(0,32) ;
    register_Array[0] = dec_to_binary(0,32) ;
    for(int i=8; i<=15; i++)    register_Array[i] = dec_to_binary(i-8,32) ;
    register_Array[29] = dec_to_binary(2000,32) ;
    register_Array[4] = dec_to_binary(111,32) ;
}
void display_reg_array(string filename){
    ofstream outfile;
    outfile.open(filename.c_str());
    outfile<<"REGISTERS\t\t\t\t\t\t\tBINARY\t\t\t\t\t\t\tDECIMAL\t\t\t\tHEXADECIMAL\n\n";
    for(int i=0; i<32; i++) {
        int num = binary_to_dec(register_Array[i]);
        outfile<<"Register array["<<i<<"]:\t\t"<<register_Array[i]<<"\t\t\t"<<num<<"\t\t\t\t\t"<<dec_to_hexaDec(num)<<endl;
    }
    outfile.close();
}

void display_mem_array(string filename){
    ofstream outfile;
    outfile.open(filename.c_str());
    for(int i=0; i<MEM_ARRAY_SIZE; i++) outfile<<"Memory array["<<i<<"]:\t"<<memory_Array[i]<<"\t"<<binary_to_dec(memory_Array[i])<<endl;
    outfile.close();
}

void initialize_pipeline_registers(){
    // if_id_reg
    if_id_reg.isActive = false;  //to start reading instructions
    if_id_reg.curr_instruc = "";
    if_id_reg.next_pc = "";

    //id_ex_reg
    id_ex_reg.isActive = false;
    id_ex_reg.curr_instruc = "";
    id_ex_reg.next_pc = "";

    //ex_mem_reg
    ex_mem_reg.isActive = false;

    //mem_wb_reg
    mem_wb_reg.isActive = false;

}

void IF(){
    if(pc_Src==0)   pc = pc+1;
    else if(pc_Src==1)  pc = branch_pc;
    else pc = pc;   //for stalls
    // cout<<pc<<endl;
    while(instruc_Array[pc].compare("11111111111111111111111111111111") == 0)  pc++;
    if_id_reg.curr_instruc = instruc_Array[pc];
    if_id_reg.next_pc = dec_to_binary(pc+1, 32);
    if_id_reg.isActive = true;
    
    cout<<"CLOCK_CYCLES : "<<cc<<"\tIF_STAGE : "<<if_id_reg.curr_instruc<<endl;
    if(instruc_Array[pc].substr(0,6).compare("000011")==0) {return_address = pc+1;cout << "BOB : " << pc+1 << endl;}
    num_instructions++;
}

void FLUSH_INSTRUCTIONS(){
    if_id_reg.curr_instruc = "11111111111111111111111111111111";    //flushing ID stage instruction.
    
    //flushing EX stage instruction.
    id_ex_reg.Control.Mem_read = "0";
    id_ex_reg.Control.Mem_write = "0";
    id_ex_reg.Control.Reg_write = "0";
    id_ex_reg.Control.Branch = "0";
    num_instructions = num_instructions -2; //two extra instructions are removed.
    return;
}

void ID(){
    if(!if_id_reg.isActive) return;
    id_ex_reg.isActive = true;
    string cur_instruc = if_id_reg.curr_instruc;
    
    if(cur_instruc.compare("11111111111111111111111111111111") == 0){//flushed instruction
        id_ex_reg.Control.Mem_read = "0";
        id_ex_reg.Control.Mem_write = "0";
        id_ex_reg.Control.Branch = "0";
        id_ex_reg.Control.Reg_write = "0";
        id_ex_reg.curr_instruc = cur_instruc;
        cout<<"CLOCK_CYCLES : "<<cc<<"\tID_STAGE : "<<cur_instruc<<endl;
        return;
    }

    //checking the stalls, without forwarding.
    int stall = 0;
    string Rs = cur_instruc.substr(6,5);
    string Rt = cur_instruc.substr(11,5);
    string Rd = cur_instruc.substr(16,5);
    string address = cur_instruc.substr(16,16);
    string opcode = cur_instruc.substr(0,6);
    string func = cur_instruc.substr(26,6);
    
    //if its a jump or jal, we need the jump address of 26 bit
    if(opcode.compare("000011")==0 || opcode.compare("000010")==0)
        j_addr = cur_instruc.substr(6,26);
    
    //if the previous instruction is lw, then only stalling is done

    if(ex_mem_reg.Control.Mem_read.compare("1") == 0 && ex_mem_reg.dest_reg.compare(if_id_reg.curr_instruc.substr(6,5)) == 0)
        stall = 1;

    //if not lw bgtz blez then rt is also possible.
    else if(opcode.compare("100011")!=0 && opcode.compare("000111")!=0 && opcode.compare("000110")!=0){
        if(ex_mem_reg.Control.Mem_read.compare("1") == 0 && ex_mem_reg.dest_reg.compare(if_id_reg.curr_instruc.substr(11,5)) == 0)
            stall = 1;
    }
    
    //if we have to stall, then we will not send the details of if_id_reg to id_ex_reg and will wait for one cycle
    if(stall == 1){
        pc_Src = 2; //this will make the pc to remain at the previous value.
        num_instructions--;
        //setting all the control signals to zero
        id_ex_reg.Control.Mem_read = "0";
        id_ex_reg.Control.Mem_write = "0";
        id_ex_reg.Control.Reg_write = "0";
        id_ex_reg.Control.Branch = "0";
        id_ex_reg.curr_instruc = cur_instruc;
        cout<<"CLOCK CYCLES : "<<cc<<"\tID_STAGE : "<<cur_instruc<<endl;
        return;
    }

    //setting the fields.
    id_ex_reg.curr_instruc = cur_instruc;
    id_ex_reg.opcode = opcode;
    id_ex_reg.next_pc = if_id_reg.next_pc;
    id_ex_reg.rd = Rd;
    id_ex_reg.rt = Rt;
    id_ex_reg.Offset_sign_extended = sign_extend(address, 32);
    id_ex_reg.Reg_read_data_1 = register_Array[binary_to_dec(Rs)];
    id_ex_reg.Reg_read_data_2 = register_Array[binary_to_dec(Rt)];

    //checking forwarding.
    if(ex_mem_reg.Control.Reg_write.compare("1") == 0 && ex_mem_reg.dest_reg.compare(Rs) == 0){//for R format instruction after passing EX stage, where dest_reg is Rd.
        id_ex_reg.Reg_read_data_1 = ex_mem_reg.ALU_result;
    }
    else if(mem_wb_reg.Control.Reg_write.compare("1") == 0 && mem_wb_reg.dest_reg.compare(Rs) == 0){//after passing MEM stage.
        if(mem_wb_reg.Control.Mem_to_reg.compare("0") == 0) id_ex_reg.Reg_read_data_1 = mem_wb_reg.ALU_result;//R format instruction where Rd is dest reg
        else id_ex_reg.Reg_read_data_1 = mem_wb_reg.Mem_data_read;//lw, where dest reg is Rt.
    }

    if(opcode.compare("100011")!=0 && opcode.compare("000111")!=0 && opcode.compare("000110")!=0){ //if not lw bgtz blez then 2nd operand is a register (rt)
        if(ex_mem_reg.Control.Reg_write.compare("1") == 0 && ex_mem_reg.dest_reg.compare(Rt) == 0){//for R format instruction after passing EX stage, where dest_reg is Rd.
            id_ex_reg.Reg_read_data_2 = ex_mem_reg.ALU_result;
        }
        else if(mem_wb_reg.Control.Reg_write.compare("1") == 0 && mem_wb_reg.dest_reg.compare(Rt) == 0){//after passing MEM stage.
            if(mem_wb_reg.Control.Mem_to_reg.compare("0") == 0) id_ex_reg.Reg_read_data_2 = mem_wb_reg.ALU_result;//R format instruction where Rd is dest reg
            else id_ex_reg.Reg_read_data_2 = mem_wb_reg.Mem_data_read;//lw, where dest reg is Rt.
        }
    }


    //checking the opcode and setting controls
    //if R-format or lw instruction
    if((opcode.compare("000000")==0 && func.compare("001000")!=0) || opcode.compare("100011")==0)
        id_ex_reg.Control.Reg_write = "1";
    else 
        id_ex_reg.Control.Reg_write = "0";

    //if r-format then 1 else if lw then 0
    if(opcode.compare("000000")==0 && func.compare("001000")!=0) 
        id_ex_reg.Control.Reg_dest = "1";
    else 
        id_ex_reg.Control.Reg_dest = "0"; 

    //if r-format or branch then 0 else for lw and sw 1
    if(opcode.compare("100011")==0  ||  opcode.compare("101011")==0) 
        id_ex_reg.Control.ALU_src = "1";
    else 
        id_ex_reg.Control.ALU_src = "0";
    
    //if branch then 1 else 0 for r-format and lw sw
    if(opcode.compare("100011")==0  ||  opcode.compare("101011")==0 ||  (opcode.compare("000000")==0 && func.compare("001000")!=0)) 
        id_ex_reg.Control.Branch = "0";
    else 
        id_ex_reg.Control.Branch = "1";

    //if lw then 1 else 0
    if(opcode.compare("100011")==0) 
        id_ex_reg.Control.Mem_read = "1";
    else 
        id_ex_reg.Control.Mem_read = "0"; 

    //if sw then 1 else 0
    if(opcode.compare("101011")==0) 
        id_ex_reg.Control.Mem_write = "1";
    else 
        id_ex_reg.Control.Mem_write = "0";

    //if r-format then 0 else 1 for lw
    if(opcode.compare("000000")==0 && func.compare("001000")!=0) 
        id_ex_reg.Control.Mem_to_reg = "0";
    else if(opcode.compare("100011")==0) 
        id_ex_reg.Control.Mem_to_reg = "1";

    //if r format then 10 else if lw sw then 00 else for branch 01
    if(opcode.compare("000000")==0) 
        id_ex_reg.Control.ALU_op = "10";
    else if(opcode.compare("100011")==0 || opcode.compare("101011")==0) 
        id_ex_reg.Control.ALU_op = "00";
    else 
        id_ex_reg.Control.ALU_op = "01";

    //if it is the end instruction
    if(cur_instruc.compare("00000000000000000000000000000000") == 0){//last instruction
        id_ex_reg.Control.Reg_write = "2";  //this shows that here our program is going to end.
    }

    cout<<"CLOCK CYCLES : "<<cc<<"\tID_STAGE : "<<cur_instruc<<endl;
}

void EX(){
    if(!id_ex_reg.isActive) return;
    ex_mem_reg.isActive = true;
    int op1 = binary_to_dec(id_ex_reg.Reg_read_data_1);    //rs, 1st operand
    int op2 = 0;    //second operand.

    //checking if the instruction is a flushed instruction.
    if(id_ex_reg.Control.Branch.compare("0")==0 && id_ex_reg.Control.Mem_read.compare("0")==0 && id_ex_reg.Control.Mem_write.compare("0")==0 && id_ex_reg.Control.Reg_write.compare("0")==0){
        
        //setting the fields.
        ex_mem_reg.Control.Branch = "0";
        ex_mem_reg.Control.Mem_write = "0";
        ex_mem_reg.Control.Mem_read = "0";
        ex_mem_reg.Control.Reg_write = "0";
        ex_mem_reg.curr_instruc = id_ex_reg.curr_instruc;
        cout<<"CLOCK CYCLES : "<<cc<<"\tEX_STAGE : "<<id_ex_reg.curr_instruc<<endl;
        return;
    }
    //finding second operand.
    if(id_ex_reg.Control.ALU_src.compare("0") == 0)
    op2 = binary_to_dec(id_ex_reg.Reg_read_data_2);
    else op2 = binary_to_dec(id_ex_reg.Offset_sign_extended);
    
    //finding ALU_control.
    string opcode = id_ex_reg.opcode;
    string ALU_control = "";

    if(id_ex_reg.Control.ALU_op.compare("00") == 0) //lw and sw type
        ALU_control = "0010";

    else if(id_ex_reg.Control.ALU_op.compare("10") == 0){   //R-type
        string func = id_ex_reg.Offset_sign_extended.substr(26,6);
        if(func.compare("100000")==0) ALU_control = "0010";         //add
        if(func.compare("100010")==0) ALU_control = "0110";         //sub               
        if(func.compare("000000")==0) ALU_control = "1000";         //sll
        if(func.compare("000010")==0) ALU_control = "1001";         //srl
        if(func.compare("001000")==0) ALU_control = "0000";         //jr
    }
    else   //branch-type
    {
        
        if(opcode.compare("000101")==0) ALU_control = "1100";      //bne
        if(opcode.compare("000100")==0) ALU_control = "1101";      //beq
        if(opcode.compare("000111")==0) ALU_control = "1110";      //bgtz
        if(opcode.compare("000110")==0) ALU_control = "1111";      //blez
        if(opcode.compare("000010")==0) ALU_control = "0001";      //j
        if(opcode.compare("000011")==0) ALU_control = "0011";      //jal
    }   

    //finding ALU_result
    int shamt = binary_to_dec(id_ex_reg.Offset_sign_extended.substr(21,5));
    int shift_op = binary_to_dec(id_ex_reg.Reg_read_data_2);
    if(ALU_control.compare("0010") == 0)        ex_mem_reg.ALU_result = dec_to_binary(op1+op2, 32);  //add
    else if(ALU_control.compare("0110") == 0)   ex_mem_reg.ALU_result = dec_to_binary(op1-op2, 32);  //sub
    else if(ALU_control.compare("1000") == 0)   ex_mem_reg.ALU_result = dec_to_binary(shift_op*pow(2,shamt), 32);    //sll
    else if(ALU_control.compare("1001") == 0)   ex_mem_reg.ALU_result = dec_to_binary(shift_op/pow(2,shamt), 32);    //srl

    else if(ALU_control.compare("1100") == 0){  //bne
        int Alu_result = op1-op2;   //sub
        ex_mem_reg.ALU_result = dec_to_binary(Alu_result, 32);
        if(Alu_result != 0) ex_mem_reg.ALU_zero = "1";
        else ex_mem_reg.ALU_zero = "0";
    }
    else if(ALU_control.compare("1101") == 0){  //beq
        int Alu_result = op1-op2;   //sub
        ex_mem_reg.ALU_result = dec_to_binary(op1-op2, 32);
        if(Alu_result != 0) ex_mem_reg.ALU_zero = "0";
        else ex_mem_reg.ALU_zero = "1";
    }
    else if(ALU_control.compare("1110") == 0){  //bgtz
        int Alu_result = op1;
        ex_mem_reg.ALU_result = dec_to_binary(op1, 32);
        if(Alu_result > 0) ex_mem_reg.ALU_zero = "1";
        else ex_mem_reg.ALU_zero = "0";
    }
    else if(ALU_control.compare("1111") == 0){  //blez
        int Alu_result = op1;
        ex_mem_reg.ALU_result = dec_to_binary(op1, 32);
        if(Alu_result <= 0) ex_mem_reg.ALU_zero = "1";
        else ex_mem_reg.ALU_zero = "0";
    }
    else if(ALU_control.compare("0001") == 0){  //j
        ex_mem_reg.ALU_zero = "2";
    }
    else if(ALU_control.compare("0011") == 0){  //jal
        ex_mem_reg.ALU_zero = "3";
    }
    else if(ALU_control.compare("0000") == 0){  //jr
        ex_mem_reg.ALU_zero = "4";
    }

    //setting controls.
    ex_mem_reg.Control.Branch = id_ex_reg.Control.Branch;
    ex_mem_reg.Control.Mem_read = id_ex_reg.Control.Mem_read;
    ex_mem_reg.Control.Mem_to_reg = id_ex_reg.Control.Mem_to_reg;
    ex_mem_reg.Control.Mem_write = id_ex_reg.Control.Mem_write;
    ex_mem_reg.Control.Reg_dest = id_ex_reg.Control.Reg_dest;
    ex_mem_reg.Control.Reg_write = id_ex_reg.Control.Reg_write;

    //setting fields.
    ex_mem_reg.curr_instruc = id_ex_reg.curr_instruc;
    ex_mem_reg.Reg_read_data_2 = id_ex_reg.Reg_read_data_2;
    if(id_ex_reg.Control.Reg_dest.compare("0") == 0)    //lw
    ex_mem_reg.dest_reg = id_ex_reg.rt;
    else    //R-format
    ex_mem_reg.dest_reg = id_ex_reg.rd;

    ex_mem_reg.new_calc_pc = id_ex_reg.Offset_sign_extended;

    cout<<"CLOCK CYCLES : "<<cc<<"\tEX_STAGE : "<<id_ex_reg.curr_instruc<<endl;

}

void MEM(){
    if(!ex_mem_reg.isActive) return;
    mem_wb_reg.isActive = true;

    if(ex_mem_reg.Control.Branch.compare("1") == 0 && ex_mem_reg.ALU_zero.compare("1") == 0){//conditional jump/ branch
        pc_Src = 1;
        branch_pc = binary_to_dec(ex_mem_reg.new_calc_pc);
        FLUSH_INSTRUCTIONS();
    }
    else if(ex_mem_reg.Control.Branch.compare("1") == 0 && ex_mem_reg.ALU_zero.compare("2") == 0){//j
        pc_Src = 1;
        branch_pc =  binary_to_dec(j_addr);
        FLUSH_INSTRUCTIONS();
    }
    else if(ex_mem_reg.Control.Branch.compare("1") == 0 && ex_mem_reg.ALU_zero.compare("3") == 0){//jal
        pc_Src = 1;
        branch_pc = binary_to_dec(j_addr);
        register_Array[31] =dec_to_binary(return_address,32);
        FLUSH_INSTRUCTIONS();
    }
    else if(ex_mem_reg.Control.Branch.compare("1") == 0 && ex_mem_reg.ALU_zero.compare("4") == 0){//jr
        pc_Src = 1;
        branch_pc = binary_to_dec(register_Array[31]);
        FLUSH_INSTRUCTIONS();
    }
    else pc_Src = 0;

    string mem_data = "00000000000000000000000000000000";
    if(ex_mem_reg.Control.Mem_read.compare("1") == 0){//lw
        mem_data = memory_Array[binary_to_dec(ex_mem_reg.ALU_result)];
    }
    else if(ex_mem_reg.Control.Mem_write.compare("1") == 0){//sw
        memory_Array[binary_to_dec(ex_mem_reg.ALU_result)] = ex_mem_reg.Reg_read_data_2;
    }
    

    //setting fields.
    mem_wb_reg.Mem_data_read = mem_data;
    mem_wb_reg.ALU_result = ex_mem_reg.ALU_result;
    mem_wb_reg.curr_instruc = ex_mem_reg.curr_instruc;
    mem_wb_reg.dest_reg = ex_mem_reg.dest_reg;
    mem_wb_reg.Control.Reg_write = ex_mem_reg.Control.Reg_write;
    mem_wb_reg.Control.Mem_to_reg = ex_mem_reg.Control.Mem_to_reg;

    cout<<"CLOCK CYCLES : "<<cc<<"\tMEM_STAGE: "<<ex_mem_reg.curr_instruc<<endl;

}

int WB(){
    if(!mem_wb_reg.isActive)    return 0;
    string cur_instruc = mem_wb_reg.curr_instruc;

    cout<<"CLOCK CYCLES : "<<cc<<"\tWB_STAGE : "<<cur_instruc<<endl;

    if(mem_wb_reg.Control.Reg_write.compare("2") == 0)  return 1;

    string reg_write_data =  "";
    if(mem_wb_reg.Control.Mem_to_reg.compare("1") == 0)
        reg_write_data = mem_wb_reg.Mem_data_read; 
    else
        reg_write_data = mem_wb_reg.ALU_result;
    
    if(mem_wb_reg.Control.Reg_write.compare("1") == 0)
        register_Array[binary_to_dec(mem_wb_reg.dest_reg)] = reg_write_data;
    return 0;

}

void execute(){
    convert_to_machine_code("Inp");
    initialize_pipeline_registers();
    initialize_reg_array();
    initialize_instruc_array();
    for(int i=0;i<MEM_ARRAY_SIZE;i++) memory_Array[i] = "00000000000000000000000000000000";

    while(true){
        if(WB() == 1) break;
        MEM();
        EX();
        ID();
        IF();
        cc++;
        cout<<endl;
    }
}

int main(int argc, char* argv[])
{
    execute();
    cout << endl;
    cout<<"Answer: $v0 = "<<binary_to_dec(register_Array[2])<<endl;
    cout<<"Number of instructions: "<<num_instructions-3<<endl;
    cout<<"Number of clock cycles taken: "<<cc<<endl;
    float IPC = (num_instructions*1.0)/cc;
    cout<<"IPC: "<<IPC<<endl;
    display_reg_array("RegisterArray");
    display_mem_array("MemoryArray");
    return 0;
}


