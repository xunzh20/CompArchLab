/*
	Name 1: Xun Zhou
	Name 2: Jeremy Tran
	UTEID 1: xz7637
	UTEID 2: jt44433
*/

#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */

#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255
typedef struct {
	int address;
	char label[MAX_LABEL_LEN + 1];
} TableEntry;
TableEntry symbolTable[MAX_SYMBOLS];
int symbolTable_counter;
#define MAX_LINE_LENGTH 255
enum
{
     DONE, OK, EMPTY_LINE
};


int toNum( char * pStr )
{
     char * t_ptr;
     char * orig_pStr;
     int t_length,k;
     int lNum, lNeg = 0;
     long int lNumLong;

     orig_pStr = pStr;
     if( *pStr == '#' )				/* decimal */
     { 
          pStr++;
          if( *pStr == '-' )				/* dec is negative */
          {
               lNeg = 1;
               pStr++;
          }
          t_ptr = pStr;
          
          t_length = strlen(t_ptr);
          for(k=0;k < t_length;k++)
          {
               if (!isdigit(*t_ptr))
               {
                    printf("Error: invalid decimal operand, %s\n",orig_pStr);
                    exit(4);
               }
               t_ptr++;
          }
          lNum = atoi(pStr);
          if (lNeg)
               lNum = -lNum;
     
          return lNum;
     }
     else if( *pStr == 'x' )	/* hex     */
     {
          pStr++;
          if( *pStr == '-' )				/* hex is negative */
          {
               lNeg = 1;
               pStr++;
          }
          t_ptr = pStr;
          t_length = strlen(t_ptr);

          for(k=0;k < t_length;k++)
          {
               if (!isxdigit(*t_ptr))
               {
                    printf("Error: invalid hex operand, %s\n",orig_pStr);
                    exit(4);
               }
               t_ptr++;
          }
          lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
          lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
          if( lNeg )
               lNum = -lNum;
          return lNum;
     }
     else
     {
          printf( "Error: invalid operand, %s\n", orig_pStr);
          exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
     }
}

int isOpcode(char* a){
     if ( strcmp(a, "add") == 0){
          return 0;
     }
     else if (strcmp (a , "and") == 0){
          return 0;
     }
     else if (strcmp(a , "brn") == 0){
          return 0;
     }
     else if (strcmp(a , "brnz") == 0){
          return 0;
     }
     else if (strcmp(a , "brnp") == 0){
          return 0;
     }
     else if (strcmp(a , "brz") == 0){
          return 0;
     }
     else if (strcmp(a, "br") == 0){
          return 0;
     }
     else if (strcmp(a , "brp") == 0){
          return 0;
     }
     else if (strcmp(a , "brzp") == 0){
          return 0;
     }
     else if (strcmp(a , "brnzp") == 0){
          return 0;
     }
     else if (strcmp(a ,"halt") == 0){
          return 0;
     }
     else if (strcmp(a , "jmp") == 0){
          return 0;
     }
     else if (strcmp(a , "jsr") == 0){
          return 0;
     }
     else if (strcmp(a , "jsrr") == 0){
          return 0;
     }
     else if (strcmp(a , "ldb") == 0){
          return 0;
     }
     else if (strcmp(a , "ldw") == 0){
          return 0;
     }
     else if (strcmp(a , "lea") == 0){
          return 0;
     }
     else if (strcmp(a , "nop") == 0){
          return 0;
     }
     else if (strcmp(a , "not") == 0){
          return 0;
     }
     else if (strcmp(a , "ret") == 0){
          return 0;
     }
     else if (strcmp(a , "lshf") == 0){
          return 0;
     }
     else if (strcmp(a , "rshfa") == 0){
          return 0;
     }
     else if (strcmp(a , "rti") == 0){
          return 0;
     }
     else if (strcmp(a , "stb") == 0){
          return 0;
     }
     else if (strcmp(a, "stw") == 0){
          return 0;
     }
     else if (strcmp(a, "trap") == 0){
          return 0;
     }
     else if (strcmp(a , "xor") == 0){
          return 0;
     }
     else if (strcmp(a , "rshfl") == 0){
          return 0;
     }
     else {
          return -1;
     }
}

int labeloffset9(int pc, char* arg){
     int i;
     int offset = 0x00;
     int labellocation;
     for (i = 0; i < MAX_SYMBOLS; i++){
          if ( strcmp(symbolTable[i].label, arg) == 0 ){
               labellocation = symbolTable[i].address;
               // printf("0x%.4x\n",labellocation);
          }
     }
     offset |= (labellocation - pc) >> 1;
     offset &= 0x000001FF;
     // printf("%.4x\n", offset);
     return offset;

}

int labeloffset11(int pc, char* arg){
     int i;
     int offset = 0x00;
     int labellocation;
     for (i = 0; i < MAX_SYMBOLS; i++){
          if ( strcmp(symbolTable[i].label, arg) == 0 ){
               labellocation = symbolTable[i].address;
               // printf("0x%.4x\n",labellocation);
          }
     }
     offset |= (labellocation - pc) >> 1;
     offset &= 0x000007FF;
     // printf("%.4x\n", offset);
     return offset;

}

int findreg (char* argument){
     return argument[1] - '0';
}
int turntoMachineCode( int pc, char* opCode, char* Arg1, char* Arg2, char* Arg3, char* Arg4){
     int instruction = 0;
     if (strcmp(".orig", opCode) == 0){
          instruction |= toNum(Arg1);
     }
     else if (strcmp("add", opCode) == 0){
          instruction |= 0x1000;
          instruction |= (findreg(Arg1) << 9); 
          instruction |= (findreg(Arg2) << 6); 
          
          if (Arg3[0] == 'r'){
               instruction |= (findreg(Arg3));
          } else {
               instruction |= (toNum(Arg3)&0x001F);
               instruction |= 0x0020;
          }
     }
     else if (strcmp("and",opCode) == 0){
          instruction |= 0x5000;
          instruction |= (findreg(Arg1) << 9); 
          instruction |= (findreg(Arg2) << 6); 
          if (Arg3[0] == 'r'){
               instruction |= (findreg(Arg3));
          } else {
               instruction |= (toNum(Arg3)&0x001F);
               instruction |= 0x0020;
          }
     }
     else if (strcmp("br",opCode) == 0){
          instruction = 0x0E00;
          // printf("%d\n",instruction);
          // printf("%d\n",labeloffset(beginning_memory, line, Arg1));
          // printf("%s\n", Arg1);
          instruction |= labeloffset9(pc, Arg1);
          // printf("%d\n", instruction);
          return instruction;
     }
     else if (strcmp("brz",opCode) == 0){
          instruction |= 0x0400;
          instruction |= labeloffset9(pc,Arg1);
     }
     else if (strcmp("brp",opCode) == 0){
          instruction |= 0x0200;
          // printf("%s\n",Arg1);
          instruction |= labeloffset9(pc,Arg1);

     }
     else if (strcmp("brn",opCode) == 0){
          instruction |= 0x0800;
          instruction |= labeloffset9(pc,Arg1);
     }
     else if (strcmp("brzp",opCode) == 0){
          instruction |= 0x0600;
          instruction |= labeloffset9(pc,Arg1);
     }
     else if (strcmp("brnp",opCode) == 0){
          instruction |= 0x0A00;
          instruction |= labeloffset9(pc,Arg1);
     }
     else if (strcmp("brnz",opCode) == 0){
          instruction |= 0x0C00;
          instruction |= labeloffset9(pc,Arg1);
     }
     else if (strcmp("brnzp",opCode) == 0){
          instruction |= 0x0E00;
          instruction |= labeloffset9(pc,Arg1);
     }

     else if (strcmp("jmp",opCode) == 0){
          instruction |= 0xC000;
          instruction |= (findreg(Arg1) << 6);
     }
     else if (strcmp("ret",opCode) == 0){
          instruction |= 0xC000;
          instruction |= 0xC1C0;
     }
     else if (strcmp("jsr",opCode) == 0){
          instruction |= 0x4800;
          instruction |= labeloffset11 (pc, Arg1);
     }
     else if (strcmp("jsrr",opCode) == 0){
          instruction |= 0x4000;
          instruction |= (findreg(Arg1) << 6);
     }
     else if (strcmp("ldb",opCode) == 0){
          instruction |= 0x2000;
          instruction |= (findreg(Arg1) << 9);
          instruction |= (findreg(Arg2) << 6);
          instruction |= ((toNum(Arg3))& 0x3F);
     }
     else if (strcmp("ldw",opCode) == 0){
          instruction |= 0x6000;
          instruction |= (findreg(Arg1) << 9);
          instruction |= (findreg(Arg2) << 6);
          instruction |= ((toNum(Arg3)& 0x3F));
     }
     else if (strcmp("lea",opCode) == 0){
          instruction |= 0xE000;
          instruction |= (findreg(Arg1) << 9);
          instruction |= labeloffset9 (pc, Arg2);
     
          // printf("%s\n", Arg2);
     }
     else if (strcmp("not",opCode) == 0){
          instruction |= 0x9000;
          instruction |= (findreg(Arg1) << 9);
          instruction |= (findreg(Arg2) << 6);
          instruction |= 0x003F;
     }
     else if (strcmp("rti",opCode) == 0){
          instruction |= 0x8000;
     }
     else if (strcmp("lshf",opCode) == 0){
          instruction |= 0xD000;
          instruction |= (findreg(Arg1) << 9);
          instruction |= (findreg(Arg2) << 6);
          instruction |= (toNum(Arg3) & 0xF);
     }
     else if (strcmp("rshfl",opCode) == 0){
          instruction |= 0xD010;
          instruction |= (findreg(Arg1) << 9);
          instruction |= (findreg(Arg2) << 6);
          // printf("%s\n", Arg3);
          instruction |= (toNum(Arg3) & 0xF);
     }
     else if (strcmp("rshfa",opCode) == 0){
          instruction |= 0xD030;
          instruction |= (findreg(Arg1) << 9);
          instruction |= (findreg(Arg2) << 6);
          instruction |= (toNum(Arg3) & 0xF);
     }
     else if (strcmp("stb",opCode) == 0){
          instruction |= 0x3000;
          instruction |= (findreg(Arg1) << 9);
          instruction |= (findreg(Arg2) << 6);
          // printf("%s\n%d\n",Arg3, toNum(Arg3)); 

          instruction |= ((toNum(Arg3))& 0x3F);
     }
     else if (strcmp("stw",opCode) == 0){
          instruction |= 0x7000;
          instruction |= (findreg(Arg1) << 9);
          instruction |= (findreg(Arg2) << 6);
          instruction |= ((toNum(Arg3)& 0x3F));
     }
     else if (strcmp("trap",opCode) == 0){
          instruction |= 0xF000;
          instruction |= (toNum(Arg1) & 0xFF);
     }
     else if (strcmp("xor",opCode) == 0){
          instruction |= 0x9000;
          instruction |= (findreg(Arg1) << 9); 
          instruction |= (findreg(Arg2) << 6); 
          
          if (Arg3[0] == 'r'){
               instruction |= (findreg(Arg3));
          } else if (Arg3 != NULL) {
               instruction |= (toNum(Arg3) & 0x1F);
               instruction |= 0x0020;
          } else {
               instruction |= 0x003F;
          }
     }
     else if (strcmp("halt",opCode) == 0){
          instruction |= 0xF025;
     }
     else if (strcmp("nop",opCode) == 0){
          instruction |= 0x0000;
     }
     else if (strcmp(".fill",opCode) == 0){
          instruction |= (toNum(Arg1)& 0xFF);
     }
     return instruction;
}

int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char
** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4){
     char * lRet, * lPtr;
     int i;

     if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
          return( DONE );

     for( i = 0; i < strlen( pLine ); i++ )
          pLine[i] = tolower( pLine[i] );
     
     
     /* convert entire line to lowercase */
     *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

     /* ignore the comments */
     lPtr = pLine;

     while( *lPtr != ';' && *lPtr != '\0' &&
     *lPtr != '\n' ) 
          lPtr++;

     *lPtr = '\0';
     if( !(lPtr = strtok( pLine, "\t\n ," ) ) ) 
          return( EMPTY_LINE );

     if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ) /* found a label */
     {
          *pLabel = lPtr;
          if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
     }
     
     *pOpcode = lPtr;

     if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
     
     *pArg1 = lPtr;
     
     if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

     *pArg2 = lPtr;
     
     if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

     *pArg3 = lPtr;

     if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

     *pArg4 = lPtr;

     return( OK );
}

/* Note: MAX_LINE_LENGTH, OK, EMPTY_LINE, and DONE are defined values */

int main(int argc, char* argv[]) {

// int result;
// char *opcode = "br";
// char *arg1 = "loop";
// char *arg2 = "r2";
// char *arg3 = "#5";
// char *arg4;

// int pc;
// pc = 0x300A;
// int i;
// for(i = 0; i < strlen("loop"); i++){
//      symbolTable[0].label[i] = "loop"[i];
// }
// symbolTable[0].address = 0x3004;
// result = turntoMachineCode(pc, opcode,arg1,arg2,arg3,arg4);
// printf("0x%.4X\n", result);
     char *prgName   = NULL;
     char *iFileName = NULL;
     char *oFileName = NULL;

     prgName   = argv[0];
     iFileName = argv[1];
     oFileName = argv[2];

     symbolTable_counter = 0;
     char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1,
          *lArg2, *lArg3, *lArg4;

     int lRet, lInstr;

     FILE * lInfile;
     FILE* pOutfile;

     lInfile = fopen(iFileName, "r" );	/* open the input file */
     pOutfile = fopen(oFileName, "w" );

     if (!lInfile) {
       printf("Error: Cannot open file %s\n", argv[1]);
       exit(4);
		 }
     if (!pOutfile) {
       printf("Error: Cannot open file %s\n", argv[2]);
       exit(4);
     }
     int pctracker = 0;
     int beginning_memory;

do
     {
          // printf("%d\n", round);
          lRet = readAndParse( lInfile, lLine, &lLabel,
               &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );        
          if( lRet != DONE && lRet != EMPTY_LINE )
          {
               if (strcmp(lOpcode, ".orig")==0){
                    // printf("start of the program\n");
                    // printf("%s\n",lArg1);
                    beginning_memory = toNum(lArg1);
                    pctracker = beginning_memory;
                    // printf("%.4x\n", beginning_memory);
               }
               else{
                    pctracker += 2;
                    if (!(strcmp(lLabel, "") == 0)){
                    
                    // printf("%.4x\n", pctracker);
                         symbolTable[symbolTable_counter].address = pctracker -2 ;
                         strcpy(symbolTable[symbolTable_counter].label, lLabel);
                         // printf("%s\n", symbolTable[symbolTable_counter].label);
                         // int i;
                         // for(i = 0; i < strlen(lLabel); i++){
                         // symbolTable[0].label[i] = lLabel[i];
                         // }    
                    symbolTable_counter ++;
                    }
                 }
          }
          
     } while( lRet != DONE );

     // printf("\nfirst parse done\n");
     rewind(lInfile);
     // int j;
     // for (j = 0; j < 3; j++){
     //      printf ("%s, %.4x\n", symbolTable[j].label, symbolTable[j].address);
     // }
     // pctracker = 0;
     
     rewind(lInfile);
     do
     {
          lRet = readAndParse( lInfile, lLine, &lLabel,
               &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
          if( lRet != DONE && lRet != EMPTY_LINE )
          {
               if (strcmp(lOpcode, ".orig") == 0){
                    lInstr = turntoMachineCode(pctracker,lOpcode, lArg1, lArg2, lArg3, lArg4);
                    pctracker = beginning_memory;
                    fprintf(pOutfile, "0x%.4X\n", lInstr);
               }
               else if (!(strcmp(lOpcode, ".end") == 0)){
                    
                    pctracker += 2;
                    lInstr = turntoMachineCode(pctracker,lOpcode, lArg1, lArg2, lArg3, lArg4);
                    fprintf(pOutfile, "0x%.4X\n", lInstr);
               }
          }
     } while( lRet != DONE );
     fclose(lInfile);
     fclose(pOutfile);
}