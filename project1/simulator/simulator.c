/* LC-2K Instruction-level simulator */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000 
typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;
int instructionCount=0;
void classification(stateType*, int*,int*,int*,int*);
void addInstruction(stateType*, int,int,int);
void norInstruction(stateType*, int,int,int);
void lwInstruction(stateType*,int,int,int);
void swInstruction(stateType*, int,int,int);
void beqInstruction(stateType*,int,int,int);
void jalrInstruction(stateType*, int,int);
void haltInstruction(stateType*,FILE*);
void printState(stateType *);
int convertNum(int num);

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
            state.numMemory++) {

        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }
    //todo
    for(int i=0; i<8; i++){
        state.reg[i]=0;
    }
    while(1){
        printState(&state);
        //여기서 선언해두고 call by ref로 값 변경 + 혹시 값 충돌할 수도 있으니까 매번 0으로 초기화
        int opcode=0, arg0=0, arg1=0, arg2=0; 
        classification(&state, &opcode,&arg0,&arg1,&arg2);
        state.pc++;
        instructionCount++;
        switch(opcode){
            case 0: addInstruction(&state,arg0,arg1,arg2); break;
            case 1: norInstruction(&state,arg0,arg1,arg2); break;
            case 2: lwInstruction(&state,arg0,arg1,arg2); break;
            case 3: swInstruction(&state,arg0,arg1,arg2); break;
            case 4: beqInstruction(&state,arg0,arg1,arg2); break;
            case 5: jalrInstruction(&state,arg0,arg1); break;
            case 6: haltInstruction(&state,filePtr);
            case 7: break;
        }
    }
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i = 0; i < statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i = 0; i < NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}

int convertNum(int num)
{
   /* convert a 16-bit number into a 32-bit Linux integer */
   if (num & (1 << 15)) {
      num -= (1 << 16);
   }
   return (num);
}
//type별로 field가 달라서 나누려고 했으나 I,R 말고는 하위 16bit 다 0
//r이랑 i도 어차피 r은 하위 16bit 중 상위 13bit가 0이라서 그냥 16bit 그대로 쓰면 됨
void classification(stateType *statePtr, int *opcode, int *arg0, int *arg1, int *arg2){
    int instruction=statePtr->mem[statePtr->pc];
    *opcode=(instruction>>22)&0b111;
    *arg0=(instruction>>19)&0b111;
    *arg1=(instruction>>16)&0b111;
    *arg2=(instruction)&0xffff;
}
void addInstruction(stateType *statePtr,int regA, int regB, int des){
    statePtr->reg[des]=statePtr->reg[regA]+statePtr->reg[regB];
}
void norInstruction(stateType *statePtr,int regA, int regB, int des){
    statePtr->reg[des]=~(statePtr->reg[regA]|statePtr->reg[regB]);
}
void lwInstruction(stateType *statePtr,int regA, int regB, int offset){
    //todo 이거 32비트로 확장해야 함!! sign extension!!!
    offset=convertNum(offset);
    statePtr->reg[regB]=statePtr->mem[offset+(statePtr->reg[regA])];
    
}
void swInstruction(stateType *statePtr,int regA, int regB, int offset){
    offset=convertNum(offset);
    statePtr->mem[offset+(statePtr->reg[regA])]=statePtr->reg[regB];
}
void beqInstruction(stateType *statePtr,int regA, int regB, int offset){
    if(statePtr->reg[regA]==statePtr->reg[regB]){
        offset=convertNum(offset);
        (statePtr->pc)+=(offset);
    }
    
}
void jalrInstruction(stateType *statePtr,int regA, int regB){
    statePtr->reg[regB]=(statePtr->pc);
    statePtr->pc=statePtr->reg[regA];
}
void haltInstruction(stateType *state, FILE *filePtr){
    printf("machine halted\n");
    printf("total of %d instructions executed\n",instructionCount);
    printf("final state of machine:");
    printState(state);
    fclose(filePtr);
    exit(0);
}
