/*
    Name 1: Your Name
    UTEID 1: Your UTEID
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N - Lab 5                                           */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         pagetable    page table in LC-3b machine language   */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void eval_micro_sequencer();
void cycle_memory();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64
#define INITIAL_STATE_NUMBER 18

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
enum CS_BITS {                                                  
    IRD,
    EXCEPT_CHECK,
    J_Select,
    COND2, COND1, COND0,
    J5, J4, J3, J2, J1, J0,
    LD_MAR,
    LD_MDR,
    LD_IR,
    LD_BEN,
    LD_REG,
    LD_CC,
    LD_PC,
    LD_PC_OLD,
    LD_SSP,
    LD_USP,
    LD_PSR,
    LD_VECTOR,
    LD_PROTECTED,
    LD_PageFault,
    LD_UNALIGNED,
    LD_VA,
    LD_J,
    LD_MDRTEMP,
    LD_RWSTORE,
    GATE_PC,
    GATE_MDR,
    GATE_ALU,
    GATE_MARMUX,
    GATE_SSP,
    GATE_USP,
    GATE_PC_OLD,
    GATE_SHF,
    GATE_PSR,
    PCMUX1, PCMUX0,
    DRMUX1, DRMUX0,
    SR1MUX1, SR1MUX0,
    ADDR1MUX,
    ADDR2MUX1, ADDR2MUX0,
    MARMUX,
    VECTORMUX1, VECTORMUX0,
    PSRMUX1, PSRMUX0,
    ALUBMUX1, ALUBMUX0,
    MAR_SOURCE1,MAR_SOURCE0,
    ALUK1, ALUK0,
    PTE_WB_MUX,
    MDR_MUX,
    MIO_EN,
    R_W,
    DATA_SIZE,
    LSHF1,
/* MODIFY: you have to add all your new control signals */
    CONTROL_STORE_BITS
} CS_BITS;

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD(int *x)           { return(x[IRD]); }
int GetEXCEPT_CHECK(int *x)  { return(x[EXCEPT_CHECK]);}
int GetJ_Select(int *x)      { return(x[J_Select]);}
int GetCOND(int *x)          { return((x[COND2] << 2) + (x[COND1] << 1) + x[COND0]); }
int GetJ(int *x)             { return((x[J5] << 5) + (x[J4] << 4) +
				      (x[J3] << 3) + (x[J2] << 2) +
				      (x[J1] << 1) + x[J0]); }
int GetLD_MAR(int *x)        { return(x[LD_MAR]); }
int GetLD_MDR(int *x)        { return(x[LD_MDR]); }
int GetLD_IR(int *x)         { return(x[LD_IR]); }
int GetLD_BEN(int *x)        { return(x[LD_BEN]); }
int GetLD_REG(int *x)        { return(x[LD_REG]); }
int GetLD_CC(int *x)         { return(x[LD_CC]); }
int GetLD_PC(int *x)         { return(x[LD_PC]); }
int GetLD_PC_OLD(int *x)     { return(x[LD_PC_OLD]);}
int GetLD_SSP(int *x)        { return(x[LD_SSP]);}
int GetLD_USP(int *x)        { return(x[LD_USP]);}
int GetLD_PSR(int *x)        { return(x[LD_PSR]);}
int GetLD_VECTOR(int *x)     { return(x[LD_VECTOR]);}
int GetLD_PROTECTED(int *x)  { return(x[LD_PROTECTED]);}
int GetLD_PAGEFAULT(int *x)  { return(x[LD_PageFault]);}
int GetLD_UNALIGNED(int *x)  { return(x[LD_UNALIGNED]);}
int GetLD_VA(int *x)         { return(x[LD_VA]);}
int GetLD_J(int *x)          { return(x[LD_J]);}
int GetLD_MDRTEMP(int *x)    { return(x[LD_MDRTEMP]);}
int GetLD_RWSTORE(int *x)    { return(x[LD_RWSTORE]);}

int GetGATE_PC(int *x)       { return(x[GATE_PC]); }
int GetGATE_MDR(int *x)      { return(x[GATE_MDR]); }
int GetGATE_ALU(int *x)      { return(x[GATE_ALU]); }
int GetGATE_MARMUX(int *x)   { return(x[GATE_MARMUX]); }

int GetGATE_SSP(int *x)      { return(x[GATE_SSP]);}
int GetGATE_USP(int *x)      { return(x[GATE_USP]);}
int GetGATE_PC_OLD(int *x)   { return(x[GATE_PC_OLD]);}
int GetGATE_SHF(int *x)      { return(x[GATE_SHF]);}
int GetGATE_PSR(int *x)      { return(x[GATE_PSR]);}

int GetPCMUX(int *x)         { return((x[PCMUX1] << 1) + x[PCMUX0]); }
int GetDRMUX(int *x)         { return((x[DRMUX1] << 1) + x[DRMUX0]); }
int GetSR1MUX(int *x)        { return((x[SR1MUX1] << 1) + x[SR1MUX0]); }

int GetADDR1MUX(int *x)      { return(x[ADDR1MUX]); }
int GetADDR2MUX(int *x)      { return((x[ADDR2MUX1] << 1) + x[ADDR2MUX0]); }
int GetMARMUX(int *x)        { return(x[MARMUX]); }

int GetVECTOR_MUX(int *x)    { return((x[VECTORMUX1] << 1) + x[VECTORMUX0]);}
int GetPSR_MUX (int *x)      { return((x[PSRMUX1] << 1) + x[PSRMUX0]);}
int GetALUB_MUX (int *x)     { return((x[ALUBMUX1] << 1) + x[ALUBMUX0]);}
int GetMAR_SOURCE(int *x)    { return((x[MAR_SOURCE1] << 1) + x[MAR_SOURCE0]);}


int GetALUK(int *x)          { return((x[ALUK1] << 1) + x[ALUK0]); }
int GetPTE_WB_MUX(int *x)    { return(x[PTE_WB_MUX]);}
int GetMDR_MUX(int *x)       { return(x[MDR_MUX]);}
int GetMIO_EN(int *x)        { return(x[MIO_EN]); }
int GetR_W(int *x)           { return(x[R_W]); }
int GetDATA_SIZE(int *x)     { return(x[DATA_SIZE]); } 
int GetLSHF1(int *x)         { return(x[LSHF1]); }
/* MODIFY: you can add more Get functions for your new control signals */

/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
   There are two write enable signals, one for each byte. WE0 is used for 
   the least significant byte of a word. WE1 is used for the most significant 
   byte of a word. */

#define WORDS_IN_MEM    0x2000 /* 32 frames */ 
#define MEM_CYCLES      5
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */
int BUS;	/* value of the bus */

typedef struct System_Latches_Struct{

int PC,		/* program counter */
    MDR,	/* memory data register */
    MAR,	/* memory address register */
    IR,		/* instruction register */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P,		/* p condition bit */
    BEN;        /* ben register */

int READY;	/* ready bit */
  /* The ready bit is also latched as you dont want the memory system to assert it 
     at a bad point in the cycle*/

int REGS[LC_3b_REGS]; /* register file. */

int MICROINSTRUCTION[CONTROL_STORE_BITS]; /* The microintruction */

int STATE_NUMBER; /* Current State Number - Provided for debugging */ 

/* For lab 4 */
int INTV; /* Interrupt vector register */
int EXCV; /* Exception vector register */
int SSP; /* Initial value of system stack pointer */
/* MODIFY: you should add here any other registers you need to implement interrupts and exceptions */
int PC_OLD;
int PSR;
int USP;
/* For lab 5 */
int PTBR; /* This is initialized when we load the page table */
int VA;   /* Temporary VA register */
/* MODIFY: you should add here any other registers you need to implement virtual memory */

} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;
int PTBR = 0x1000;
/* For lab 5 */
#define PAGE_NUM_BITS 9
#define PTE_PFN_MASK 0x3E00
#define PTE_VALID_MASK 0x0004
#define PAGE_OFFSET_MASK 0x1FF

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int CYCLE_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void help() {                                                    
    printf("----------------LC-3bSIM Help-------------------------\n");
    printf("go               -  run program to completion       \n");
    printf("run n            -  execute program for n cycles    \n");
    printf("mdump low high   -  dump memory from low to high    \n");
    printf("rdump            -  dump the register & bus values  \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
int time_interrupt = 0; /* this variable is used to indicated that 300 cycles has passed*/
void cycle() {                                                
  if (CYCLE_COUNT == 300){
    time_interrupt = 1;
  }
  if ((time_interrupt == 1) && (CURRENT_LATCHES.STATE_NUMBER == 51)){
    time_interrupt = 0;
  }
//   eval_bus_drivers();
  eval_micro_sequencer();   
  cycle_memory();
  eval_bus_drivers();
  drive_bus();
  latch_datapath_values();


  CURRENT_LATCHES = NEXT_LATCHES;

  CYCLE_COUNT++;

}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
    int i;

    if (RUN_BIT == FALSE) {
	printf("Can't simulate, Simulator is halted\n\n");
	return;
    }

    printf("Simulating for %d cycles...\n\n", num_cycles);
    for (i = 0; i < num_cycles; i++) {
	if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
	}
	cycle();
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed.                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
    if (RUN_BIT == FALSE) {
	printf("Can't simulate, Simulator is halted\n\n");
	return;
    }

    printf("Simulating...\n\n");
    while (CURRENT_LATCHES.PC != 0x0000)
	cycle();
    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
    int address; /* this is a byte address */

    printf("\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	printf("  0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	fprintf(dumpsim_file, " 0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
    int k; 

    printf("\nCurrent register/bus values :\n");
    printf("-------------------------------------\n");
    printf("Cycle Count  : %d\n", CYCLE_COUNT);
    printf("PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%0.4x\n", BUS);
    printf("MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	printf("%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%0.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	fprintf(dumpsim_file, "%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
    char buffer[20];
    int start, stop, cycles;

    printf("LC-3b-SIM> ");

    scanf("%s", buffer);
    printf("\n");

    switch(buffer[0]) {
    case 'G':
    case 'g':
	go();
	break;

    case 'M':
    case 'm':
	scanf("%i %i", &start, &stop);
	mdump(dumpsim_file, start, stop);
	break;

    case '?':
	help();
	break;
    case 'Q':
    case 'q':
	printf("Bye.\n");
	exit(0);

    case 'R':
    case 'r':
	if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
	else {
	    scanf("%d", &cycles);
	    run(cycles);
	}
	break;

    default:
	printf("Invalid Command\n");
	break;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */ 
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename) {                 
    FILE *ucode;
    int i, j, index;
    char line[200];

    printf("Loading Control Store from file: %s\n", ucode_filename);

    /* Open the micro-code file. */
    if ((ucode = fopen(ucode_filename, "r")) == NULL) {
	printf("Error: Can't open micro-code file %s\n", ucode_filename);
	exit(-1);
    }

    /* Read a line for each row in the control store. */
    for(i = 0; i < CONTROL_STORE_ROWS; i++) {
	if (fscanf(ucode, "%[^\n]\n", line) == EOF) {
	    printf("Error: Too few lines (%d) in micro-code file: %s\n",
		   i, ucode_filename);
	    exit(-1);
	}

	/* Put in bits one at a time. */
	index = 0;

	for (j = 0; j < CONTROL_STORE_BITS; j++) {
	    /* Needs to find enough bits in line. */
	    if (line[index] == '\0') {
		printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
		       ucode_filename, i);
		exit(-1);
	    }
	    if (line[index] != '0' && line[index] != '1') {
		printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
		       ucode_filename, i, j);
		exit(-1);
	    }

	    /* Set the bit in the Control Store. */
	    CONTROL_STORE[i][j] = (line[index] == '0') ? 0:1;
	    index++;
	}

	/* Warn about extra bits in line. */
	if (line[index] != '\0')
	    printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
		   ucode_filename, i);
    }
    printf("\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
    int i;

    for (i=0; i < WORDS_IN_MEM; i++) {
	MEMORY[i][0] = 0;
	MEMORY[i][1] = 0;
    }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename, int is_virtual_base) {                   
    FILE * prog;
    int ii, word, program_base, pte, virtual_pc;

    /* Open program file. */
    prog = fopen(program_filename, "r");
    if (prog == NULL) {
	printf("Error: Can't open program file %s\n", program_filename);
	exit(-1);
    }

    /* Read in the program. */
    if (fscanf(prog, "%x\n", &word) != EOF)
	program_base = word >> 1;
    else {
	printf("Error: Program file is empty\n");
	exit(-1);
    }

    if (is_virtual_base) {
      if (CURRENT_LATCHES.PTBR == 0) {
	printf("Error: Page table base not loaded %s\n", program_filename);
	exit(-1);
      }

      /* convert virtual_base to physical_base */
      virtual_pc = program_base << 1;
      pte = (MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][1] << 8) | 
	     MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][0];

      printf("virtual base of program: %04x\npte: %04x\n", program_base << 1, pte);
		if ((pte & PTE_VALID_MASK) == PTE_VALID_MASK) {
	      program_base = (pte & PTE_PFN_MASK) | ((program_base << 1) & PAGE_OFFSET_MASK);
   	   printf("physical base of program: %x\n\n", program_base);
	      program_base = program_base >> 1; 
		} else {
   	   printf("attempting to load a program into an invalid (non-resident) page\n\n");
			exit(-1);
		}
    }
    else {
      /* is page table */
     CURRENT_LATCHES.PTBR = program_base << 1;
    }

    ii = 0;
    while (fscanf(prog, "%x\n", &word) != EOF) {
	/* Make sure it fits. */
	if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
		   program_filename, ii);
	    exit(-1);
	}

	/* Write the word to memory array. */
	MEMORY[program_base + ii][0] = word & 0x00FF;
	MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;;
	ii++;
    }

    if (CURRENT_LATCHES.PC == 0 && is_virtual_base) 
      CURRENT_LATCHES.PC = virtual_pc;

    printf("Read %d words from program into memory.\n\n", ii);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */ 
/*             and set up initial state of the machine         */
/*                                                             */
/***************************************************************/
void initialize(char *argv[], int num_prog_files) { 
    int i;
    init_control_store(argv[1]);

    init_memory();
    load_program(argv[2],0);
    for ( i = 0; i < num_prog_files; i++ ) {
	load_program(argv[i + 3],1);
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
    CURRENT_LATCHES.SSP = 0x3000; /* Initial value of system stack pointer */
    CURRENT_LATCHES.USP = 0xFE00;
    CURRENT_LATCHES.PSR = 0x8002;
    CURRENT_LATCHES.PTBR = 0x1000;
/* MODIFY: you can add more initialization code HERE */

    NEXT_LATCHES = CURRENT_LATCHES;

    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
    FILE * dumpsim_file;

    /* Error Checking */
    if (argc < 4) {
	printf("Error: usage: %s <micro_code_file> <page table file> <program_file_1> <program_file_2> ...\n",
	       argv[0]);
	exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv, argc - 3);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
	printf("Error: Can't open dumpsim file\n");
	exit(-1);
    }

    while (1)
	get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code, except for the places indicated 
   with a "MODIFY:" comment.
   You are allowed to use the following global variables in your
   code. These are defined above.

   CONTROL_STORE
   MEMORY
   BUS

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */
/***************************************************************/

int signext(int num, int firstbit){
  int mask = 0;
  if ((num >> firstbit) == 0x1){
    mask = 0xFFFFFFFF << firstbit;
    // printf("%0.4x\n",mask);
  }
  // printf("%0.4x\n",num | mask);
  return num | mask;
}
int Exception_for_62;
int protected, unaligned;
int J_bits_store;
int RW_store;
void eval_micro_sequencer() {

  /* 
   * Evaluate the address of the next state according to the 
   * micro sequencer logic. Latch the next microinstruction.
   */
    int next_state_number = 0;
    // printf("this is IRD, %d\n",GetIRD(CURRENT_LATCHES.MICROINSTRUCTION));

    if (GetIRD(CURRENT_LATCHES.MICROINSTRUCTION)){
        next_state_number = CURRENT_LATCHES.IR >>12 & 0xF;
    }else{
        // printf("PSR is %0.4x\n",CURRENT_LATCHES.PSR);
        // printf("BUS is %0.4x\n",BUS);
        // printf("cond is %0.4x\n",GetCOND(CURRENT_LATCHES.MICROINSTRUCTION));
        int J = GetJ(CURRENT_LATCHES.MICROINSTRUCTION);
        int conds = GetCOND(CURRENT_LATCHES.MICROINSTRUCTION); 
        // for J0
        int addrmode = 0;
        if ((conds == 3) && ((CURRENT_LATCHES.IR & 0x800) >> 11 == 0x1)){
            J = J + 1;
        }
        else if ((conds == 1) && (CURRENT_LATCHES.READY)){
            J = J + 2;
        }
        else if ((conds == 2) && (CURRENT_LATCHES.BEN)){
            J = J + 4;
        }else if ((conds == 6) && (time_interrupt)){
            J = J + 8;
        }else if ((conds == 4) && ((CURRENT_LATCHES.PSR & 0x8000))){
            J = J + 16;
        }
        // }else if ((conds == 7) && (((CURRENT_LATCHES.MAR & 0x0001) == 1))){
        //     J = J + 32;
        // }
        int address_of_next_state = 0;
        if (GetIRD(CURRENT_LATCHES.MICROINSTRUCTION)){
            address_of_next_state |= J;
        }
        else{
            address_of_next_state = (CURRENT_LATCHES.IR & 0xF000) >> 12;
        }
        // printf("%0.4x", J);
        
        
        next_state_number = J;
    }
    // printf("excecption,%d\n",Exception_for_62);

    if (GetJ_Select(CURRENT_LATCHES.MICROINSTRUCTION)){
        if (J_bits_store == 0x02){
            next_state_number = 29;
        }
        else if (J_bits_store == 0x06){
            next_state_number = 25;
        }
        else if (J_bits_store == 0x07){
            next_state_number = 23;
        }else if (J_bits_store == 0x03){
            next_state_number = 24;
        }
        else if (J_bits_store == 15){
            next_state_number = 33;
        }
        else if (J_bits_store == 42){
            next_state_number = 44;
        }
        else if (J_bits_store == 46){
            next_state_number = 45;
        }
        else if (J_bits_store == 49){
            next_state_number = 48;
        }
        else if (J_bits_store == 8){
            next_state_number = 37;
        }
        else if (J_bits_store == 40){
            next_state_number = 53;
        }
        else if (J_bits_store == 43){
            next_state_number = 30;
        }
    }
    // printf("this is a test, %d\n", next_state_number);
    NEXT_LATCHES.STATE_NUMBER = next_state_number;
    memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[next_state_number],sizeof(int)*CONTROL_STORE_BITS);
}
// #define PAGE_NUM_BITS 9
// #define PTE_PFN_MASK 0x3E00
// #define PTE_VALID_MASK 0x0004
// #define PAGE_OFFSET_MASK 0x1FF
int pre_MDR_data;
int page_fault;
void cycle_memory() {
 
  /* 
   * This function emulates memory and the WE logic. 
   * Keep track of which cycle of MEMEN we are dealing with.  
   * If fourth, we need to latch Ready bit at the end of 
   * cycle to prepare microsequencer for the fifth cycle.  
   */
    static int passed_cycles = 0;
    int datasize = GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION);
    int rw = GetR_W(CURRENT_LATCHES.MICROINSTRUCTION);
    int MAR = CURRENT_LATCHES.MAR;
    if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION)){
        passed_cycles ++;
        if (passed_cycles == 4){
            NEXT_LATCHES.READY = 1;
        }
        else if (passed_cycles == 5){
            passed_cycles = 0;
            NEXT_LATCHES.READY = 0;
            if (rw == 0){
                // printf("%0.4x\n", MAR);
                pre_MDR_data = Low16bits(MEMORY[CURRENT_LATCHES.MAR >> 1][1] << 8) | MEMORY[CURRENT_LATCHES.MAR >> 1][0];
                // printf("pre_MDR_data%0.4x\n", pre_MDR_data);
                // does not care about datasize
                // printf("protected%d\n", protected);
                // printf("page_fault%d\n", page_fault);
            }
            else{
                int WE0 = 0;
                int WE1 = 0;
                // printf("%0.4x\n", MAR);
                if (datasize == 0){
                    if (MAR & 0x0001){
                        WE1 = 1;
                        WE0 = 0;
                    }else{
                        WE1 = 0;
                        WE0 = 1;
                    }
                }else{
                    WE1 = 1;
                    WE0 = 1;
                }
                // printf("WE1%d,WE0%d\n",WE1, WE0);
                if ((WE1 == 1) && (WE0 == 0)){ // byte size
                    
                    MEMORY[MAR >> 1][1] = Low16bits((CURRENT_LATCHES.MDR & 0xFF00) >> 8);
                    // printf("%0.4x\n",MEMORY[MAR >> 1][1]);
                }
                else if ((WE1 == 0) && (WE0 == 1)){
                    MEMORY[MAR >> 1][0] = Low16bits(CURRENT_LATCHES.MDR & 0x00FF);
                }
                else if ((WE1 == 1) && (WE0 == 1)){
                    MEMORY[MAR >> 1][1] = Low16bits((CURRENT_LATCHES.MDR & 0xFF00) >> 8);
                    MEMORY[MAR >> 1][0] = Low16bits(CURRENT_LATCHES.MDR & 0x00FF);
                }
            }
        }
        
    }
}

int MARMUX_data = 0;
int PCMUX_data = 0;
int ALU_data;
int SHF_data;
int MDR_data;
int PC_data;
int ADDER_RESULT;
int DR = 0;
int VECTOR;

int PSR_DATA;
int VECTOR_DATA;

void eval_bus_drivers() {

  /* 
   * Datapath routine emulating operations before driving the bus.
   * Evaluate the input of tristate drivers 
   *             Gate_MARMUX,
   *		 Gate_PC,
   *		 Gate_ALU,
   *		 Gate_SHF,
   *		 Gate_MDR.
   */    
    int ADDR1MUX_data = 0;
    
    int ADDR2MUX_data = 0;
    int LSHF1_data = 0;
    
    
    int ADDR1MUX = GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION);
    int SR1;
    int SR1MUX = GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION);
    if (SR1MUX == 0){
        SR1 = (CURRENT_LATCHES.IR >> 9) & 0x7;
    }else if (SR1MUX == 1){
        SR1 = (CURRENT_LATCHES.IR >> 6) & 0x7;
    }else if (SR1MUX == 2){
        SR1 = 0x6;
    }
    // printf("this is SR1MUX, %d\n",SR1MUX);
    // printf("this is SR1, %d\n",SR1);
    int DRMUX = GetDRMUX (CURRENT_LATCHES.MICROINSTRUCTION);
    
    if (DRMUX == 0){
        DR = (CURRENT_LATCHES.IR >> 9) & 0x7;
    }else if (DRMUX == 1){
        DR = 0x7;
    }else if (DRMUX == 2){
        DR = 0x6;
    }

    if ((ADDR1MUX == 0)){
        ADDR1MUX_data = CURRENT_LATCHES.PC;
    }else {
        ADDR1MUX_data = CURRENT_LATCHES.REGS[SR1];
    }

    
    
    int ADDR2MUX = GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION);//zero, offset6, pc9, pc11
    if (ADDR2MUX == 0){
        ADDR2MUX_data = 0;
    }else if (ADDR2MUX == 1){
        ADDR2MUX_data = Low16bits(signext(CURRENT_LATCHES.IR & 0x3F,5));
    }else if (ADDR2MUX == 2){
        ADDR2MUX_data = Low16bits(signext(CURRENT_LATCHES.IR & 0x1FF,8));
    }else if (ADDR2MUX == 3){
        ADDR2MUX_data = Low16bits(signext(CURRENT_LATCHES.IR & 0x7FF,10));
    }
    
   
    int LSHF1 = GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION);
    if (LSHF1 == 1){
        LSHF1_data = Low16bits(ADDR2MUX_data << 1);
    }else{
        LSHF1_data = ADDR2MUX_data;
    }
    ADDER_RESULT = Low16bits(LSHF1_data + ADDR1MUX_data);

    int MARMUX = GetMARMUX(CURRENT_LATCHES.MICROINSTRUCTION);//7,ADDER
    
    if(MARMUX == 0){
        MARMUX_data = Low16bits((0x00FF & CURRENT_LATCHES.IR)<< 1); 
    }else if (MARMUX == 1){
        MARMUX_data = Low16bits(ADDER_RESULT);
    }
// Pc MUX

// ALU
    int SR2MUX = CURRENT_LATCHES.IR & 0x0020;
    // printf("SR2MUX: %0.4x\n", SR2MUX);
    int SR2MUX_data = 0;
    if (SR2MUX == 0){
        SR2MUX_data = CURRENT_LATCHES.REGS[CURRENT_LATCHES.IR & 0x7];
        // printf("%0.4x", SR2MUX_data);
    }else{
        SR2MUX_data = Low16bits(signext(CURRENT_LATCHES.IR & 0x1F,4));
        // printf("%0.4x", SR2MUX_data);
    }
    // printf("this is sr2mux %d\n", SR2MUX_data);
    //ALUB
    int ALUBMUX_DATA;
    if (GetALUB_MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0){
        ALUBMUX_DATA = SR2MUX_data; 
    }else if (GetALUB_MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        ALUBMUX_DATA = 2;
    }else if (GetALUB_MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 3){
        ALUBMUX_DATA = -2;
    }
    // printf("this is alubdata %d\n", ALUBMUX_DATA);
    // printf("this is aluk %d\n", GetALUK(CURRENT_LATCHES.MICROINSTRUCTION));
    int ALUK = GetALUK(CURRENT_LATCHES.MICROINSTRUCTION);//ADD AND XOR PASSA
    if (ALUK == 0){
        ALU_data = Low16bits(CURRENT_LATCHES.REGS[SR1] + ALUBMUX_DATA);
    }else if (ALUK == 1){
        ALU_data = Low16bits(CURRENT_LATCHES.REGS[SR1] & ALUBMUX_DATA);
    }else if (ALUK == 2){
        ALU_data = Low16bits(CURRENT_LATCHES.REGS[SR1] ^ ALUBMUX_DATA);
    }else{
        ALU_data = Low16bits(CURRENT_LATCHES.REGS[SR1]);
    }
//SHF
    int shift_amount = CURRENT_LATCHES.IR & 0xF;
    int shift_control = (CURRENT_LATCHES.IR & 0x30) >> 4;
    // printf("shift_control %0.4x\n", shift_control);
    if (shift_control == 0){
        SHF_data = Low16bits(CURRENT_LATCHES.REGS[SR1] << shift_amount);
        // printf("%0.4x", SHF_data);
    }
    else if (shift_control == 1){
        SHF_data = Low16bits(CURRENT_LATCHES.REGS[SR1] >> shift_amount);
    }
    else if (shift_control == 3){
        SHF_data = Low16bits(signext(CURRENT_LATCHES.REGS[SR1],15) >> shift_amount);
    }
//MDR
    if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        MDR_data = CURRENT_LATCHES.MDR;
        // printf("%0.4x\n", CURRENT_LATCHES.MDR);
    }
    else{
        if ((CURRENT_LATCHES.MAR & 0x1) == 1){
            MDR_data = signext((CURRENT_LATCHES.MDR & 0xFF00) >> 8, 7);
        }else{
            MDR_data = signext((CURRENT_LATCHES.MDR & 0x00FF),7);
        }
    }
}


void drive_bus() {

  /* 
   * Datapath routine for driving the bus from one of the 5 possible 
   * tristate drivers. 
   */       
    if (GetGATE_MARMUX(CURRENT_LATCHES.MICROINSTRUCTION)){
        
        BUS = MARMUX_data;
    }
    else if (GetGATE_PC(CURRENT_LATCHES.MICROINSTRUCTION)){
        BUS = CURRENT_LATCHES.PC;
    }
    else if (GetGATE_MDR(CURRENT_LATCHES.MICROINSTRUCTION)){
        // printf("%d\n", GetGATE_MDR(CURRENT_LATCHES.MICROINSTRUCTION));
        BUS = MDR_data;
        // printf("%d\n", MDR_data);
    }
    else if (GetGATE_ALU(CURRENT_LATCHES.MICROINSTRUCTION)){
        BUS = ALU_data;
    }
    else if (GetGATE_SHF(CURRENT_LATCHES.MICROINSTRUCTION)){
        BUS = SHF_data;
    }
    else if (GetGATE_PC_OLD(CURRENT_LATCHES.MICROINSTRUCTION)){
        BUS = Low16bits(CURRENT_LATCHES.PC_OLD);
    }
    else if (GetGATE_SSP(CURRENT_LATCHES.MICROINSTRUCTION)){
        BUS = CURRENT_LATCHES.SSP;
    }
    else if (GetGATE_USP(CURRENT_LATCHES.MICROINSTRUCTION)){
        BUS = CURRENT_LATCHES.USP;
    }
    else if (GetGATE_PSR(CURRENT_LATCHES.MICROINSTRUCTION)){
        BUS = CURRENT_LATCHES.PSR;
    }
    else {
        BUS = 0; // tri-state
    }
    
    // printf("CURRENT_LATCHES.READY%d\n", CURRENT_LATCHES.READY);
    protected = (((pre_MDR_data & 0x0008) == 0) && (CURRENT_LATCHES.PSR >> 15 == 1) && (CURRENT_LATCHES.STATE_NUMBER == 57) && CURRENT_LATCHES.READY && ((CURRENT_LATCHES.IR & 0xF000) != 0xF000));
    page_fault = (((pre_MDR_data & PTE_VALID_MASK) == 0) && (CURRENT_LATCHES.STATE_NUMBER == 57) && CURRENT_LATCHES.READY);

    // printf("CURRENT_LATCHES.IR%d\n",((CURRENT_LATCHES.IR & 0xF000) != 0xF000));
    // printf("protected%d\n", protected);
    // printf("page_fault%d\n", page_fault);
    int page_and_prot = ((protected | page_fault) & CURRENT_LATCHES.READY);
    // printf("page_and_prot%d\n", page_and_prot);
    unaligned = (((BUS & 0x0001) == 1) && GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) && (GetEXCEPT_CHECK(CURRENT_LATCHES.MICROINSTRUCTION)));
    // printf("bus%0.4x\n",BUS);
    // printf("DATA_SIZE %0.4x\n",GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION));
    // printf("EXCEPT_CHECK %0.4x\n",GetEXCEPT_CHECK(CURRENT_LATCHES.MICROINSTRUCTION));
    // printf("unaligned%d\n", unaligned);
    Exception_for_62 = page_and_prot | unaligned;
    if (Exception_for_62 & GetEXCEPT_CHECK(CURRENT_LATCHES.MICROINSTRUCTION)){
        NEXT_LATCHES.STATE_NUMBER = 0x3E;
        memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[0x3E],sizeof(int)*CONTROL_STORE_BITS);
    }
}

int protected_flag, unaligned_flag,page_fault_flag;
int MDR_Temp;

void latch_datapath_values() {

  /* 
   * Datapath routine for computing all functions that need to latch
   * values in the data path at the end of this cycle.  Some values
   * require sourcing the bus; therefore, this routine has to come 
   * after drive_bus.
   */    
    if (GetLD_VA(CURRENT_LATCHES.MICROINSTRUCTION)){
        NEXT_LATCHES.VA = CURRENT_LATCHES.MAR;
    }  
    if (GetLD_PROTECTED(CURRENT_LATCHES.MICROINSTRUCTION)){
        protected_flag = protected;
    }
    
    if (GetLD_UNALIGNED(CURRENT_LATCHES.MICROINSTRUCTION)){
        unaligned_flag = unaligned;
    }

    if (GetLD_PAGEFAULT(CURRENT_LATCHES.MICROINSTRUCTION)){
        page_fault_flag = page_fault;
    }

    if (GetLD_PC_OLD(CURRENT_LATCHES.MICROINSTRUCTION)){
        NEXT_LATCHES.PC_OLD = Low16bits(BUS);
    }

    if (GetLD_SSP(CURRENT_LATCHES.MICROINSTRUCTION)){
        int SR1 = GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION);
        if (SR1 == 0x2) {
            NEXT_LATCHES.SSP = CURRENT_LATCHES.REGS[SR1];
        }
    }
    if (GetLD_USP(CURRENT_LATCHES.MICROINSTRUCTION)){
        int SR1 = GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION);
        if (SR1 == 0x2) {
            NEXT_LATCHES.USP = CURRENT_LATCHES.REGS[SR1];
        }
    }
    if (GetLD_MDRTEMP(CURRENT_LATCHES.MICROINSTRUCTION)){
        MDR_Temp = CURRENT_LATCHES.MDR;
    }
    if (GetLD_J(CURRENT_LATCHES.MICROINSTRUCTION)){
        J_bits_store = CURRENT_LATCHES.STATE_NUMBER;
    }
    if (GetLD_RWSTORE(CURRENT_LATCHES.MICROINSTRUCTION)){
        RW_store = GetR_W(CURRENT_LATCHES.MICROINSTRUCTION);
    }
    if (GetLD_VECTOR(CURRENT_LATCHES.MICROINSTRUCTION)){
        int VECTORMUX = GetVECTOR_MUX(CURRENT_LATCHES.MICROINSTRUCTION);
        if (unaligned_flag){
            VECTOR_DATA = 0x03;
        }else{
            if (VECTORMUX == 0){
                VECTOR_DATA = 0x0;
            }else if (VECTORMUX == 1){
                VECTOR_DATA = 0x01;
            }else if (VECTORMUX == 2){
                if ((protected_flag)){
                    VECTOR_DATA = 0x04;
                }else {
                    VECTOR_DATA = 0x02;
                }
            }else if (VECTORMUX == 3){
                VECTOR_DATA = 0x05;
            }
                VECTOR = VECTOR_DATA;
        }
    }
    // printf("vector data %0.4x\n", VECTOR_DATA);
    // printf("VA %0.4x\n", CURRENT_LATCHES.VA);
    

    if (GetLD_MAR(CURRENT_LATCHES.MICROINSTRUCTION)){
        if (GetMAR_SOURCE(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
            NEXT_LATCHES.MAR = (VECTOR << 1) + 0x0200;
        // printf("A\n");
        }else if (GetMAR_SOURCE(CURRENT_LATCHES.MICROINSTRUCTION) == 0){
        // printf("%0.4x", BUS);
            NEXT_LATCHES.MAR = BUS;
        // printf("B\n");
        }else if (GetMAR_SOURCE(CURRENT_LATCHES.MICROINSTRUCTION) == 2){
            NEXT_LATCHES.MAR = (PTBR & 0xFF00) + ((CURRENT_LATCHES.MAR & 0xFE00)>>8);
        }else if (GetMAR_SOURCE(CURRENT_LATCHES.MICROINSTRUCTION) == 3){
            NEXT_LATCHES.MAR = (CURRENT_LATCHES.MDR & PTE_PFN_MASK) + (CURRENT_LATCHES.VA & PAGE_OFFSET_MASK);
        }
    }
    if (GetLD_MDR(CURRENT_LATCHES.MICROINSTRUCTION)){
        if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION)){
            // printf("%0.4x", pre_MDR_data);
            NEXT_LATCHES.MDR = Low16bits(pre_MDR_data);
        }else{
            if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
                NEXT_LATCHES.MDR = Low16bits(BUS);
            }else{
                // printf("I am here\n");
                NEXT_LATCHES.MDR = Low16bits(((BUS & 0x00FF) << 8) | (BUS & 0xFF));
            }
        }
        if (GetMDR_MUX(CURRENT_LATCHES.MICROINSTRUCTION)){
            NEXT_LATCHES.MDR = MDR_Temp;
        }
        if (GetPTE_WB_MUX(CURRENT_LATCHES.MICROINSTRUCTION)){
            NEXT_LATCHES.MDR = (CURRENT_LATCHES.MDR & 0xFFFC) | (RW_store << 1) | 1;
        }
    }

    if (GetLD_IR(CURRENT_LATCHES.MICROINSTRUCTION)){
        NEXT_LATCHES.IR = Low16bits(BUS);
    }
    if (GetLD_BEN(CURRENT_LATCHES.MICROINSTRUCTION)){
        if ((((CURRENT_LATCHES.IR >> 11) & 0x1) && CURRENT_LATCHES.N) || (((CURRENT_LATCHES.IR >> 10) & 0x1) && CURRENT_LATCHES.Z) || (((CURRENT_LATCHES.IR >> 9) & 0x1) && CURRENT_LATCHES.P)){
            NEXT_LATCHES.BEN = 1;
        }
        else{
            NEXT_LATCHES.BEN = 0;
        }
    }

    if (GetLD_REG(CURRENT_LATCHES.MICROINSTRUCTION)){
        NEXT_LATCHES.REGS[DR] = Low16bits(BUS);
    }

    if (GetLD_CC(CURRENT_LATCHES.MICROINSTRUCTION)){
        if (BUS == 0x0){
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 1;
            NEXT_LATCHES.P = 0;
        }
        else if ((BUS >> 15) & 0x01){
            NEXT_LATCHES.N = 1;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 0;
        }
        else {
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 1;
        }
    }

    if (GetLD_PC(CURRENT_LATCHES.MICROINSTRUCTION)){
        
        int PCMUX = GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION);//pc+2 bus adder
    
        if (PCMUX == 0){
            PC_data = Low16bits(CURRENT_LATCHES.PC + 2);
        }
        else if (PCMUX == 1){
            // printf("%0.4x", BUS);
            PC_data = Low16bits(BUS);
        }else if (PCMUX == 2){
            PC_data = Low16bits(ADDER_RESULT);
        }
        NEXT_LATCHES.PC = Low16bits(PC_data);
        // printf("%d\n", CURRENT_LATCHES.PC);
    }

    if (GetLD_PSR(CURRENT_LATCHES.MICROINSTRUCTION)){
            int PSRMUX = GetPSR_MUX(CURRENT_LATCHES.MICROINSTRUCTION);
            int NZP = (CURRENT_LATCHES.N <<2) + (CURRENT_LATCHES.Z << 1) + (CURRENT_LATCHES.P);
            if (PSRMUX == 0){
                NEXT_LATCHES.PSR = Low16bits((CURRENT_LATCHES.PSR & 0x8000)| NZP);
            }else if (PSRMUX == 1){
                NEXT_LATCHES.PSR = BUS;
                NEXT_LATCHES.N = (BUS & 0x0004) >> 2;
                NEXT_LATCHES.Z = (BUS & 0x0002) >> 1;
                NEXT_LATCHES.P = (BUS & 0x0001);
            }else if (PSRMUX == 2){
                NEXT_LATCHES.PSR = Low16bits(CURRENT_LATCHES.PSR & 0x7FFF);
            }else if (PSRMUX == 3){
                NEXT_LATCHES.PSR = Low16bits(CURRENT_LATCHES.PSR | 0x8000);
            }

        }
}