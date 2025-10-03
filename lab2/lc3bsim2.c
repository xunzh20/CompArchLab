/*
    Name 1: Xun Zhou 
    Name 2: Jeremy Tran
    UTEID 1: xz7637
    UTEID 2: jt44433
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

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
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
*/

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

  int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
  int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {                                                    
  printf("----------------LC-3b ISIM Help-----------------------\n");
  printf("go               -  run program to completion         \n");
  printf("run n            -  execute program for n instructions\n");
  printf("mdump low high   -  dump memory from low to high      \n");
  printf("rdump            -  dump the register & bus values    \n");
  printf("?                -  display this help menu            \n");
  printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  process_instruction();
  CURRENT_LATCHES = NEXT_LATCHES;
  INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
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
/* Purpose   : Simulate the LC-3b until HALTed                 */
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

  printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  printf("-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
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
  printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
  printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  printf("Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
  fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  fprintf(dumpsim_file, "Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
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
void load_program(char *program_filename) {                   
  FILE * prog;
  int ii, word, program_base;

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
    MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
    ii++;
  }

  if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

  printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */ 
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) { 
  int i;

  init_memory();
  for ( i = 0; i < num_prog_files; i++ ) {
    load_program(program_filename);
    while(*program_filename++ != '\0');
  }
  CURRENT_LATCHES.Z = 1;  
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
  if (argc < 2) {
    printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
           argv[0]);
    exit(1);
  }

  printf("LC-3b Simulator\n\n");

  initialize(argv[1], argc - 1);

  if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
    printf("Error: Can't open dumpsim file\n");
    exit(-1);
  }

  while (1)
    
    get_command(dumpsim_file);
    
}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

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
void setCC(int result){
  if (result < 0){
    NEXT_LATCHES.N = 1;
    NEXT_LATCHES.Z = 0;
    NEXT_LATCHES.P = 0;
  }else if (result == 0){
    NEXT_LATCHES.Z = 1;
    NEXT_LATCHES.P = 0;
    NEXT_LATCHES.N = 0;
  }else{
    NEXT_LATCHES.P = 1;
    NEXT_LATCHES.N = 0;
    NEXT_LATCHES.Z = 0;
  }
}
void ADD(int instruction){
  int dr = (instruction >> 9) & (0x7);
  int sr1 = (instruction >> 6) & (0x7);
  int A = (instruction >> 5) & (0x1);
  int imm5, sr2;
  int result;
  if (A == 1){
    imm5 = (instruction) & 0x1F;
    result = signext(CURRENT_LATCHES.REGS[sr1],15)+signext(imm5,4);
    NEXT_LATCHES.REGS[dr] = Low16bits(result);
  }else{
    sr2 = (instruction) & 0x3;
    result = signext(CURRENT_LATCHES.REGS[sr1],15)+signext(CURRENT_LATCHES.REGS[sr2],15);
    NEXT_LATCHES.REGS[dr] = Low16bits(result);
  }
  setCC(result);
}
void AND(int instruction){
  int dr = (instruction >> 9) & (0x7);
  int sr1 = (instruction >> 6) & (0x7);
  int A = (instruction >> 5) & (0x1);
  int imm5, sr2, result;
  if (A == 1){
    imm5 = (instruction) & 0x1F;
    result = signext(CURRENT_LATCHES.REGS[sr1],15) & signext(imm5,4);
    NEXT_LATCHES.REGS[dr] = Low16bits(result);
    
  }else{
    sr2 = (instruction) & 0x3;
    result = signext(CURRENT_LATCHES.REGS[sr1],15) & signext(CURRENT_LATCHES.REGS[sr2],15);
    NEXT_LATCHES.REGS[dr] = Low16bits(result);
  }
  setCC(result);
}
void BR(int instruction){
  int n = (instruction >> 11) & 0x1;
  int z = (instruction >> 10) & 0x1;
  int p = (instruction >> 9) & 0x1;
  int pcoffset9 = instruction & 0x1FF;
  if (((n == 1) && (CURRENT_LATCHES.N == 1)) || ((z == 1) && (CURRENT_LATCHES.Z == 1)) || ((p == 1) && (CURRENT_LATCHES.P == 1))){
    NEXT_LATCHES.PC = Low16bits(NEXT_LATCHES.PC + (signext(pcoffset9,8) << 1)); 
  }
}
void JMP(int instruction){
  int baseR = (instruction >> 6) & (0x7);
  if (baseR == 0x7){
  NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[7];
  }else{
  NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC;
  NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.REGS[baseR]);
  }
}
void JSR(int instruction){
  NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC;
  int A = (instruction >> 11) & 0x1;
  if (A == 1){
    int pcoffset11 = signext(instruction & 0x7FF,10);
    NEXT_LATCHES.PC = Low16bits(NEXT_LATCHES.PC + (pcoffset11 << 1));
  }
  else{
    int baseR = (instruction >> 6 ) & 0x7;
    NEXT_LATCHES.PC = Low16bits(signext(CURRENT_LATCHES.REGS[baseR],15));
  }
  
}
void LDB(int instruction){
  int dr = (instruction >> 9) & (0x7);
  int baseR = (instruction >> 6) & (0x7);
  int boffset6 = signext(instruction & 0x3F,5);
  int pcoffset = boffset6;
  int even_odd = boffset6 % 2;
  // printf("boffset: %d\n pcoffset: %d \n even_odd: %d\n",boffset6,pcoffset,even_odd);
  int target = Low16bits(signext(CURRENT_LATCHES.REGS[baseR],15)+pcoffset);
  // printf("target: %0.4x\n",target);
  NEXT_LATCHES.REGS[dr] = Low16bits(signext((MEMORY[target/2][boffset6 % 2])&0xFF,7));
  setCC(signext((MEMORY[target/2][boffset6 % 2])&0xFF,7));
}
void LDW(int instruction){
  int dr = (instruction >> 9) & (0x7);
  int baseR = (instruction >> 6) & (0x7);
  int offset6 = signext(instruction & 0x3F,5);
  int target = Low16bits(signext(CURRENT_LATCHES.REGS[baseR],15)+(offset6<<1));
  NEXT_LATCHES.REGS[dr] = Low16bits((MEMORY[target/2][1]<<8)+(MEMORY[target/2][0]));
  setCC(signext((MEMORY[target/2][1]<<8)+(MEMORY[target/2][0]),15));
}
void LEA(int instruction){
  int dr = (instruction >> 9) & (0x7);
  int pcoffset9 = instruction & 0x1FF; 
  NEXT_LATCHES.REGS[dr] = Low16bits((NEXT_LATCHES.PC)+(signext(pcoffset9,8)<<1));
}

int logical_right_shift(int x, int n)
{
    int size = sizeof(int) * 8; // usually sizeof(int) is 4 bytes (32 bits)
    return (x >> n) & ~(((0x1 << size) >> n) << 1);
}
void SHF(int instruction){
  int dr = (instruction >> 9) & (0x7);
  int sr = (instruction >> 6) & (0x7);
  int A = (instruction >> 5) & (0x1);
  int B = (instruction >> 4) & (0x1);
  int amount = instruction & 0xF;
  int result;
  if ((A == 0) && (B == 0)){
    NEXT_LATCHES.REGS[dr] = Low16bits(signext(CURRENT_LATCHES.REGS[sr],15) << amount);
    result = NEXT_LATCHES.REGS[dr];
  }
  else if ((A == 0) && (B == 1)){
    NEXT_LATCHES.REGS[dr] = Low16bits(CURRENT_LATCHES.REGS[sr] >> amount);
    result = NEXT_LATCHES.REGS[dr];
  }
  else if ((A == 1) && (B == 1)){
    NEXT_LATCHES.REGS[dr] = Low16bits(signext(CURRENT_LATCHES.REGS[sr],15) >> amount);
    result = NEXT_LATCHES.REGS[dr];
  }
  setCC(signext(result,15));
}
void STB(int instruction){
  int sr = (instruction >> 9) & (0x7);
  int baseR = (instruction >> 6) & (0x7);
  int boffset6 = signext(instruction & 0x3F,5);
  
  int byteoffset = boffset6 % 2;
  MEMORY[Low16bits(signext(CURRENT_LATCHES.REGS[baseR],15)+boffset6)/2][byteoffset] = CURRENT_LATCHES.REGS[sr] & 0xFF;
}
void STW(int instruction){
  int sr = (instruction >> 9) & (0x7);
  int baseR = (instruction >> 6) & (0x7);
  int offset6 = signext(instruction & 0x3F,5)<< 1;
  MEMORY[Low16bits(signext(CURRENT_LATCHES.REGS[baseR],15)+offset6)/2][1] = CURRENT_LATCHES.REGS[sr] >> 8 & 0xFF;
  MEMORY[Low16bits(signext(CURRENT_LATCHES.REGS[baseR],15)+offset6)/2][0] = CURRENT_LATCHES.REGS[sr] & 0xFF;
}
void TRAP(int instruction){
  int trapvect8 = instruction & 0xFF;
  NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC;
  NEXT_LATCHES.PC = MEMORY[((NEXT_LATCHES.PC+trapvect8)<<1)/2][1] << 8 & 0xFF + MEMORY[((NEXT_LATCHES.PC+trapvect8)<<1)/2][0];
}
void XOR (int instruction){
  int dr = (instruction >> 9) & (0x7);
  int baseR = (instruction >> 6) & (0x7);
  int A = (instruction >>5) & 0x1;
  int imm5 = instruction & 0x1F;
  int result;
  if (A == 1){
    if (imm5 == 0x1F){
      // printf("%0.4x\n",CURRENT_LATCHES.REGS[baseR]);
      result = ~signext(CURRENT_LATCHES.REGS[baseR],15);
      // printf("%0.4x\n",result);
      NEXT_LATCHES.REGS[dr] = Low16bits(result);
    }
    else{
      result = signext(CURRENT_LATCHES.REGS[baseR],15) ^ signext(imm5,4);
      NEXT_LATCHES.REGS[dr] = Low16bits(result);
    }
  }else{
    int sr2 = instruction & 0x7;
    result = signext(CURRENT_LATCHES.REGS[baseR],15) ^ signext(CURRENT_LATCHES.REGS[sr2],15);
    NEXT_LATCHES.REGS[dr] = Low16bits(result);
  }
  
  setCC(result);
}
void process_instruction(){
  /*  function: process_instruction
   *  
   *    Process one instruction at a time  
   *       -Fetch one instruction
   *       -Decode 
   *       -Execute
   *       -Update NEXT_LATCHES
   */     
  int instruction;
  // printf("instruction\n%0.4x\n",instruction);
  instruction = ((MEMORY[CURRENT_LATCHES.PC/2][1]<<8)+ MEMORY[CURRENT_LATCHES.PC/2][0]);
  // printf("%0.4x\n",instruction);
  NEXT_LATCHES.PC = CURRENT_LATCHES.PC +2;
  // printf("%0.4x\n", NEXT_LATCHES.PC);

  int opcode;
  // printf("opcode\n%0.4x\n",opcode);
  opcode = (instruction >> 12) & 0xF;
  // printf("%0.4x\n",opcode); 
    // int imm5 = 0x1F;
  // int sr1 = 1;
  // CURRENT_LATCHES.REGS[sr1] = 0;
  // printf("%0.4x,%0.4x\n",signext(CURRENT_LATCHES.REGS[sr1],16),signext(imm5,5));
  // printf("%0.4x\n",Low16bits(signext(CURRENT_LATCHES.REGS[sr1],16)+signext(imm5,5)));
  // NEXT_LATCHES.REGS[sr1] = Low16bits(signext(CURRENT_LATCHES.REGS[sr1],16)+signext(imm5,5));
  // int result = signext(0x001f,5);
  if (opcode == 0x1){
    ADD(instruction);
  }
  else if (opcode == 0x5){
    AND(instruction);
  }
  else if (opcode == 0){
    BR(instruction);
  }
  else if (opcode == 0xC){
    JMP(instruction);
  }
  else if (opcode == 0x4){
    JSR(instruction);
  }
  else if (opcode == 0x2){
    LDB(instruction);
  }
  else if (opcode == 0x6){
    LDW(instruction);
  }
  else if (opcode == 0xE){
    LEA(instruction);
  }
  else if (opcode == 0x9){
    XOR(instruction);
  }
  else if (opcode == 0xD){
    SHF(instruction);
  }
  else if (opcode == 0x3){
    STB(instruction);
  }
  else if (opcode == 0x7){
    STW(instruction);
  }
  else if (opcode == 0xF){
    TRAP(instruction);
  }
}