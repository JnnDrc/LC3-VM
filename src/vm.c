//----------------------------------------------------------
//LC-3 Virtual machine   | Maquina virtual LC-3
//Based in these articles| Baseado nesses artigos:
// - https://www.andreinc.net/2021/12/01/writing-a-simple-vm-in-less-than-125-lines-of-c#virtual-machines
// - https://www.jmeiners.com/lc3-vm/#:lc3.c_2
//----------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//----------------------------------------------------------
//Macros & Helpers

#define VM_LC3 static inline

typedef unsigned char u8;           // 8 bits unsigned integer  | inteiro sem sinal de 8 bits
typedef unsigned short u16;         // 16 bits unsigned integer | inteiro sem sinal de 16 bits
typedef unsigned int u32;           // 32 bits unsigned integer | inteiro sem sinal de 32 bits
#define U16_MAX 0b1111111111111111  // Max value for unsigned 16bit integer | Valor máximo para inteiro sem sinal de 16 bits

//? Uncomment these to use rti or res | descomente-os para poder usar rti ou res
//#define RTI_IMPLEMENTED   //!NEED IMPLEMENTATION TO USE | PRECISA DE IMPLEMENTAÇÃO PARA USAR
//#define RES_VALID        


#define OPC(inst)   ((inst)>>12)            // macro to get OP code from instruction    | Macro para pegar o OP Code da instrução
#define DR(inst)    (((inst)>>9)&0x7)       // macro to get destination register        | Macro para pegar o registrador de destino
#define SR1(inst)   (((inst)>>6)&0x7)       // macro to get source register 1           | Macro para pegar o registrador fonte 1
#define SR2(inst)   ((inst)&0x7)            // macro to get source register 2           | Macro para pegar o registrador fonte 2
#define IMM(inst)   ((inst)&0x1F)           // macro to get immediate value             | Macro para pegar o valor imediato
#define FIMM(inst)  ((inst>>5)&1)           // macro to get the 5t bit of opcode        | Macro para pegar o o quinto bit do opcode
#define OFF6(inst)  sigext((inst)&0x3F, 6)  // macro to get offset 6bits                | Macro para pegar o offset de 6 bits
#define OFF9(inst)  sigext((inst)&0x1FF, 9) // macro to get offset 9bits                | Macro para pegar o offset de 9 bits
#define OFF11(inst) sigext((inst)&0x7FF, 11)// macro to get offset 11bits               | Macro para pegar o offset de 11 bits
#define FCND(inst)  (((inst)>>9)&0x7)       // macro to get the condition flag          | Macro para pegar a bandeira de condição
#define JSRM(inst)  (((inst)>>11)&1)        // macro to get the jsr mode                | Macro para pegar o modo da operação jsr
#define BR(inst)    (((inst)>>6)&0x7)       // macro to get the base register           | Macro para pegar registrador da base
#define TRP(inst)   ((inst)&0xFF)           // macro to get the trap vector             | Macro para pegar o vetor da operação trap
#define CHR1(ptr)   ((char)((*ptr)&0xFF))   // macro to get the first char of a word    | Macro para pegar o primeiro char de uma word
#define CHR2(ptr)   ((char)((*ptr>>8)&0xFF))// macro to get the seconde char of a word  | Macro para pegar o segundo char de uma word
#define TXTCLR(ptr) (30+(((*ptr)>>8)&0x0F)) // macro to get the text color
#define BCKCLR(ptr) (40+(((*ptr)>>12)&0xFF))// macro to get the background color

#define CLRRESET "\x1b[0m"      // macro to reset color


//extend bits to 16.                        | estende os bits para 16.
//if it's negative, fill with ones, else    | se for negativo, pre-enche com uns, se não
//with zeros                                | com zeros
VM_LC3 u16 sigext(u16 _x, int bits)
{
    if((_x >> (bits - 1))&1){           
        _x |= (0xFFFF << bits);         
    }
    return _x;
}
#define SIGEXTIMM(inst) (sigext(IMM(inst),5))//macro to extend imm5     | Macro para extender o valor imediato de 5 bits

u8 running = 1;    //1 = true 0 = false   
//----------------------------------------------------------
//Memory | Memória

u16 PC_START = 0x3000;              //where programs start to be read/written
u16 memory[U16_MAX + 1] = {0};      //memory array

//function to read data in memory address
VM_LC3 u16 memread(u16 address){return memory[address];}
//function to write data in memory address
VM_LC3 u16 memwrite(u16 address, u16 data){memory[address] = data;}
//----------------------------------------------------------
//Registers | Registradores

enum regs {R0 = 0, R1, R2, R3, R4, R5, R6, R7, RPC, RCND, RCNT};    //list of registers
u16 reg[RCNT] = {0};                                           //registers
//----------------------------------------------------------
//Flags | Bandeiras de estado

enum flags {FP = 0b001, FZ = 0b010, FN = 0b100};    //cpu flags

//update the condition register | atualiza o registrador de condição
VM_LC3 void updflg(enum regs r)
{
    if (!reg[r]) reg[RCND] = FZ;
    else if (reg[r]>>15) reg[RCND] = FN;
    else reg[RCND] = FP;
}

//----------------------------------------------------------
//Ops   | Operações

#define OPS     (16)    //Number of op codes | Numero de op codes
#define TRPVECS (10)    //Number of trap sub-routines | numero de sub-rotinas trap
#define TRPOFF  (32)    //Trap offset

typedef void (*op_executor_f)(u16 inst);
typedef void (*trp_executor_f)();

/*0x0|0b0000*/ VM_LC3 void br  (u16 inst)
{
     if(reg[RCND] & FCND(inst)) reg[RPC] += OFF9(inst);
}
/*0x1|0b0001*/ VM_LC3 void add (u16 inst)
{
    reg[DR(inst)] = reg[SR1(inst)] + (FIMM(inst) ? SIGEXTIMM(inst) : reg[SR2(inst)]);
    updflg(DR(inst));
}
/*0x2|0b0010*/ VM_LC3 void ld  (u16 inst)
{
    reg[DR(inst)] = memread(reg[RPC] + OFF9(inst));
    updflg(DR(inst));
}
/*0x3|0b0011*/ VM_LC3 void st  (u16 inst)
{
    memwrite(reg[RPC] + OFF9(inst), reg[DR(inst)]);
}
/*0x4|0b0100*/ VM_LC3 void jsr (u16 inst) 
{ 
    reg[R7] = reg[RPC];
    reg[RPC] = (JSRM(inst) ? reg[RPC] + OFF11(inst) : reg[BR(inst)]);
}
/*0x5|0b0101*/ VM_LC3 void and (u16 inst) 
{ 
    reg[DR(inst)] = reg[SR1(inst)] & (FIMM(inst) ? SIGEXTIMM(inst) : reg[SR2(inst)]);
    updflg(DR(inst));
}
/*0x6|0b0110*/ VM_LC3 void ldr (u16 inst) 
{
    reg[DR(inst)] = memread(reg[SR1(inst)] + OFF6(inst));
    updflg(DR(inst));
}
/*0x7|0b0111*/ VM_LC3 void str (u16 inst)
{ 
    memwrite(reg[SR1(inst)] + OFF6(inst),reg[DR(inst)]);
}
/*0x8|0b1000*/ VM_LC3 void rti (u16 inst) 
{ 
    //!NOT IMPLEMENTED | NÃO IMPLEMENTADO
    #if defined RTI_IMPLEMENTED
        //rti implementation here
    #else
        exit(1);
    #endif
}
/*0x9|0b1001*/ VM_LC3 void not (u16 inst) 
{ 
    reg[DR(inst)] = ~reg[SR1(inst)];
    updflg(DR(inst));
}
/*0xA|0b1010*/ VM_LC3 void ldi (u16 inst)
{ 
    reg[DR(inst)] = memread(memread(reg[RPC] + OFF9(inst)));
    updflg(DR(inst));
}
/*0xB|0b1011*/ VM_LC3 void sti (u16 inst)
{ 
    memwrite(memread(reg[RPC] + OFF9(inst)),reg[DR(inst)]);
}
/*0xC|0b1100*/ VM_LC3 void jmp (u16 inst)
{ 
    reg[RPC] = reg[BR(inst)];
}
/*0xD|0b1101*/ VM_LC3 void res (u16 inst)
{   
    //! NOT EXIST IN OFFICIAL LC-3 | NÃO EXISTE NO LC-3 OFICIAL
    #if defined RES_VALID
        reg[RPC] = PC_START;
        for (register u8 i = 0; i < RCNT; i++)
        {
            reg[i] = 0;
        }
    #else
        exit(1);
    #endif
}
/*0xE|0b1110*/ VM_LC3 void lea (u16 inst)
{ 
    reg[DR(inst)] = reg[RPC] + OFF9(inst);
    updflg(DR(inst));
}

VM_LC3 void trpgetc(void)
{
    reg[R0] = getc(stdin);
}
VM_LC3 void trpout(void)
{
    fprintf(stdout,"%c", (char)reg[R0]);
}
VM_LC3 void trpputs(void)
{
    u16* p = memory + reg[R0];
    while (*p)
    {   
        fprintf(stdout,"%c",(char)(*p));
        p++;
    }
}
VM_LC3 void trpin(void)
{
    reg[R0] = getc(stdin);
    fprintf(stdout,"%c", (char)reg[R0]);
}
VM_LC3 void trpputsp(void)
{
    u16* p = memory + reg[R0];
    while (*p)
    {
        if(CHR1(p) == '\0')break;
        else fprintf(stdout,"%c",CHR1(p));

        if(CHR2(p) == '\0')break;
        else fprintf(stdout,"%c",CHR2(p));
        p++;
    }
}
VM_LC3 void trpin16(void)
{
    fscanf(stdin,"%hu",&reg[R0]);
}
VM_LC3 void trpoutu16(void)
{   
    fprintf(stdout,"%hu\n",reg[R0]);
}
VM_LC3 void trpouti16(void)
{   
    //! NOT EXIST IN OFFICIAL LC-3, Added by me | NÃO EXISTE NO LC-3 OFICIAL, adicionado por mim
    fprintf(stdout,"%d\n",(short)reg[R0]);
}
VM_LC3 void trpputsc(void)
{
    u16* p = memory + reg[R0];
    while (*p)
    {   
        fprintf(stdout,"\033[%d;%dm%c""\x1b[0m",TXTCLR(p),BCKCLR(p),(char)(*p));
        p++;
    }
}
VM_LC3 void trphlt(void)
{
    running = 0;
}
trp_executor_f trp_execute[TRPVECS] = {trpgetc, trpout, trpputs, trpin, trpputsp,trphlt, trpin16, trpoutu16,trpouti16,trpputsc};

/*0xF|0b1111*/ VM_LC3 void trp (u16 inst)
{ 
    trp_execute[TRP(inst) - TRPOFF]();
}  


op_executor_f op_execute[OPS] = {br, add, ld, st, jsr, and, ldr, str, rti, not, ldi, sti, jmp, res, lea, trp};

//----------------------------------------------------------
//Load programs and main loop

void run(u16 offset)
{   
    fprintf(stdout,"\x1b[36m""Starting VM...\n\n""\x1b[0m");
    reg[RPC] = PC_START + offset;
    while (running)
    {
        u16 instruction = memread(reg[RPC]++);
        op_execute[OPC(instruction)](instruction);
    }
}
void load_image(const char* image_path, u16 offset)
{
    FILE* img = fopen(image_path, "rb");
    if(img == NULL){
        fprintf(stderr,"\x1b[31m""ERR: Failed to open program image file %s\n""\x1b[0m",image_path);
        exit(1);
    }


    u16* p = memory + PC_START + offset;
    fread(p,sizeof(u16),(U16_MAX-PC_START),img);

    fclose(img);
}

void debug_memory(u16 offset)
{
    u16* p = memory + PC_START + offset;
    u16 mempos = PC_START + offset;
    while (*p)
    {
        fprintf(stdout,"\x1b[33m""memory[0x%04X] : 0x%04X\n""\x1b[0m",mempos,*p);
        p++;
        mempos++;
    }
}

void help(void);

int main(int argc, char const *argv[])
{   
    size_t runoff = 0x0;
    size_t loadoff = 0x0;
    u32 arg = 2;
    u8 dbg = 0;
    if(argc < 2)
    {
        fprintf(stdout,"\x1b[31m""ERR: Missing program to run\n""\x1b[0m");
        fprintf(stdout,"\x1b[33m""\nEXPECT: lc3-vm (OBLIGATORY)[program_path] (OPTIONAL)[vm_flags]""\x1b[0m");
        fprintf(stdout,"\x1b[33m""\nHELP: type lc3-vm --help for flags list""\x1b[0m");
        return 1;
    }
    if(!strcmp(argv[1],"--help"))
    {
        help();
        return 0;
    }


    fprintf(stdout,"\x1b[36m""Setting up VM\n",PC_START + runoff);
    for(u32 i = 2; i < argc; i++)
    {
        if(!strcmp(argv[i], "--dbg")) dbg = 1;

        if(!strcmp(argv[i],"--runoff")) sscanf(argv[i+1],"%x",&runoff);

        if(!strcmp(argv[i],"--loadoff")) sscanf(argv[i+1],"%x",&loadoff);
    }
    

    fprintf(stdout,"\x1b[36m""Run origin setted to %X\n""\x1b[0m",PC_START + runoff);
    fprintf(stdout,"\x1b[36m""Load program image origin setted to %X\n""\x1b[0m",PC_START + loadoff);

    fprintf(stdout,"\x1b[36m""Loading program to memory...\n""\x1b[0m");
    load_image(argv[1],loadoff);
    fprintf(stdout,"\x1b[36m""Program Loaded to memory\n""\x1b[0m");

    if(dbg) debug_memory(loadoff);

    run(runoff);
    
    fprintf(stdout,"\x1b[36m""\nVM exited with success\n""\x1b[0m");
    return 0;
}

void help(void)
{
    fprintf(stdout,"\x1b[36m""----LC-3 Virtual Machine Help Manual----\n""\x1b[0m");
    fprintf(stdout,"\x1b[36m""flags:                                  \n""\x1b[0m");
    fprintf(stdout,"\x1b[36m"" --dbg     : enable debug mode          \n""\x1b[0m");
    fprintf(stdout,"\x1b[36m"" --runoff  : set run offset             \n""\x1b[0m");
    fprintf(stdout,"\x1b[36m""      --runoff value(hex)               \n""\x1b[0m");
    fprintf(stdout,"\x1b[36m"" --loadoff : set load program offset    \n""\x1b[0m");
    fprintf(stdout,"\x1b[36m""      --loadoff value(hex)              \n""\x1b[0m");
    fprintf(stdout,"\x1b[36m""----------------------------------------\n""\x1b[0m");
}