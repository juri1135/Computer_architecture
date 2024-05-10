/* Assembler code fragment for LC-2K */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#define MAXLINELENGTH 1000
#define MAXLABLES 100
typedef struct{
	char name[7]; //label 이름 최대 6글자 
	int address;
} Label;
Label labels[MAXLABLES];
int labelCount=0;

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
void addLabel(const char *,int);
int findLabel(char *);
int allNumber(const char *);
int addInstruction(int, char*, char *, char *);
int norInstruction(int, char*, char*, char*);
int lwInstruction( int, char*, char*, char*,int);
int swInstruction  (int, char*, char*, char*,int);
int beqInstruction (int, char*, char*, char*,int);
int jalrInstruction(int, char*, char*);
int haltInstruction(int);
int noopInstruction(int);
int fillInstruction(char*);

int main(int argc, char *argv[]) 
{
	char *inFileString, *outFileString;
	FILE *inFilePtr, *outFilePtr;
	char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH], 
			 arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
	int address=0;
	if (argc != 3) {
		printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
				argv[0]);
		exit(1);
	}

	inFileString = argv[1];
	outFileString = argv[2];

	inFilePtr = fopen(inFileString, "r");
	if (inFilePtr == NULL) {
		printf("error in opening %s\n", inFileString);
		exit(1);
	}
	outFilePtr = fopen(outFileString, "w");
	if (outFilePtr == NULL) {
		printf("error in opening %s\n", outFileString);
		exit(1);
	}
	
	/* here is an example for how to use readAndParse to read a line from
		 inFilePtr */
	while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
		/* reached end of file */
		if(label[0] != '\0'){
			//중복된 라벨 처리면 error 코드 출력 후 종료
			if(!(findLabel(label)==-1)){
				printf("Error! Duplicated definition of labels %s\n",label);
				exit(1);
			}
			//중복되지 않았으면 add해주기
			addLabel(label,address);
		}
		address++;
	}
	/* TODO: Phase-1 label calculation */

	/* this is how to rewind the file ptr so that you start reading from the
		 beginning of the file */
	rewind(inFilePtr);

	/* TODO: Phase-2 generate machine codes to outfile */
	//여기부터는 output file에 작성하는 거니까 다시 address 0부터 시작
	address=0;
	int machineCode=0;
	while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
		if (!strcmp(opcode, "add")) {
			machineCode=addInstruction(0,arg0,arg1,arg2);
		}
		else if (!strcmp(opcode, "nor")) {
			machineCode=norInstruction(1,arg0,arg1,arg2);
		}
		else if (!strcmp(opcode, "lw")) {
			machineCode=lwInstruction(2,arg0,arg1,arg2,address);
		}
		else if (!strcmp(opcode, "sw")) {
			machineCode=swInstruction(3,arg0,arg1,arg2,address);
		}
		else if (!strcmp(opcode, "beq")) {
			machineCode=beqInstruction(4,arg0,arg1,arg2,address);
		}
		else if (!strcmp(opcode, "jalr")) {
			machineCode=jalrInstruction(5,arg0,arg1);
		}
		else if (!strcmp(opcode, "halt")) {
			machineCode=haltInstruction(6);
		}
		else if (!strcmp(opcode, "noop")) {
			machineCode=noopInstruction(7);
		}
		else if(!strcmp(opcode,".fill")){
			machineCode=fillInstruction(arg0);
		}
		else{
			printf("Error! Unrecognized opcode %s\n",opcode);
			exit(1);
		}
		fprintf(outFilePtr,"%d\n",machineCode);
		address++;
	}
	/* after doing a readAndParse, you may want to do the following to test the
		 opcode */
	

	if (inFilePtr) {
		fclose(inFilePtr);
	}
	if (outFilePtr) {
		fclose(outFilePtr);
	}
	exit(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
		char *arg1, char *arg2)
{
	char line[MAXLINELENGTH];
	char *ptr = line;

	/* delete prior values */
	label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

	/* read the line from the assembly-language file */
	if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
		/* reached end of file */
		return(0);
	}

	/* check for line too long (by looking for a \n) */
	if (strchr(line, '\n') == NULL) {
		/* line too long */
		printf("error: line too long\n");
		exit(1);
	}

	/* is there a label? */
	ptr = line;
	if (sscanf(ptr, "%[^\t\n\r ]", label)) {
		/* successfully read label; advance pointer over the label */
		ptr += strlen(label);
	}

	/*
	 * Parse the rest of the line.  Would be nice to have real regular
	 * expressions, but scanf will suffice.
	 */
	sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%"
			"[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]", opcode, arg0, arg1, arg2);
	return(1);
}

int isNumber(char *string)
{
	/* return 1 if string is a number */
	int i;
	return( (sscanf(string, "%d", &i)) == 1);
}

void addLabel(const char *label, int address){
	strcpy(labels[labelCount].name, label);
    labels[labelCount].address = address;
    labelCount++;
}
//lables에 lable 있으면 주소 return, 없으면 -1 return
int findLabel(char *name){
	for(int i=0; i<labelCount; i++){
		if(strcmp(labels[i].name,name)==0){
			return labels[i].address;
		}
	}
	return -1;
}
//이진수를 십진수로 변환하는 함수, 근데 %d로 출력하면 그만이라 삭제
// int twoTodec(int num){
// 	int ans=0;
//     unsigned int num=0b100000010000000000000111;
// 	for(int i=0; i<32; i++){
// 		if((num&1)==1){
// 		    ans += (1 << i);
// 		    printf("i is %d, ans is %d\n", i, ans);
// 		}
// 		num=num>>1;
// 	}
//     printf("%d",ans);
//     return 0;
// }
//오직 레지스터가 숫자인지 아닌지. 만약 맨 처음에 -면 out of range 띄우고 종료
int allNumber(const char* str){
	//돌면서 하나라도 숫자가 아니면 return 0
	int len=strlen(str);
	for(int i=1; i<len; i++){
		if((!isdigit(str[i]))){
			return 0;
		}
	}
	//여기까지 왔으면 맨 처음 빼곤 다 숫자임 맨 처음이 -면 out of range error 
	if(str[0]=='-'){
		printf("Error! Register is out of range [0,7] %s\n",str);
		exit(1);
	}
	//만약 맨 처음이 -가 아닌데 숫자도 아니라면 return 0
	if(!isdigit(str[0])){
		return 0;
	}
	return 1;
}
void ErrorChecking(char* arg0,char* arg1,char* arg2){
	//숫자면 allNumber 1 반환 !니까 하나라도 숫자가 아니면 error 
	if(!allNumber(arg0)|!allNumber(arg1)|!allNumber(arg2)){
		printf("Error! Non-integer register arguments\n");
		exit(1);
	}
	int regA=atoi(arg0);
	int regB=atoi(arg1);
	int des=atoi(arg2);

	if(regA>7||regB>7||des>7){
		printf("Error! Register is out of range [0,7]\n");
		exit(1);
	}
}
void ErrorCheckingIJ(char* arg0,char* arg1){
	if(!allNumber(arg0)|!allNumber(arg1)){
		printf("Error! Non-integer register arguments\n");
		exit(1);
	}
	int regA=atoi(arg0);
	int regB=atoi(arg1);

	if(regA>7||regB>7){
		printf("Error! Register is out of range [0,7]\n");
		exit(1);
	}
}
void offsetCheck(char *offset){
	int len=strlen(offset);
	for(int i=0; i<len; i++){
		if(!isdigit(offset[i])){
			printf("Error! offset is nither integer and label %s\n",offset);
			exit(1);
		}
	}
}
int addInstruction(int opcode,char* arg0,char* arg1,char* arg2){
	//todo arg0~2가 올바른 정수값인지+0~7인지 
	//arg0=regA, arg1=regB, arg2=des
	ErrorChecking(arg0,arg1,arg2);
	int regA=atoi(arg0);
	int regB=atoi(arg1);
	int des=atoi(arg2);
	return ((opcode<<22)|(regA<<19)|(regB<<16)|des);
}
int norInstruction(int opcode,char *arg0,char *arg1,char *arg2){
	//todo arg0~2가 올바른 정수값인지+0~7인지 
	ErrorChecking(arg0,arg1,arg2);
	int regA=atoi(arg0);
	int regB=atoi(arg1);
	int des=atoi(arg2);
	return ((opcode<<22)|(regA<<19)|(regB<<16)|des);
}
int lwInstruction(int opcode,char *arg0,char *arg1,char *arg2, int pc){
	//todo arg2가 16bit인지, arg0, arg1 error checking + label도 가능해서 그것도 확인 
	ErrorCheckingIJ(arg0,arg1);
	int regA=atoi(arg0);
	int regB=atoi(arg1);
	//label이면 0 반환

	if(isNumber(arg2)){
		offsetCheck(arg2);
		int offset=atoi(arg2);
		if((offset>32767)||(offset<-32768)){
			printf("Error! offsetFields does not fit in 16 bits %s\n",arg2);
			exit(1);
		}
		return ((opcode<<22)|(regA<<19)|(regB<<16)|offset);
	}
	else{
		int label=findLabel(arg2);
		if(label==-1){
			printf("Error! Use of undefined labels %s\n",arg2);
			exit(1);
		}
		return ((opcode<<22)|(regA<<19)|(regB<<16)|label);
	}
}
int swInstruction(int opcode,char *arg0,char *arg1,char *arg2, int pc){
	//todo arg2가 16bit인지, arg0, arg1 error checking + label도 가능해서 그것도 확인 
	ErrorCheckingIJ(arg0,arg1);
	int regA=atoi(arg0);
	int regB=atoi(arg1);
	//label이면 0 반환
	if(isNumber(arg2)){
		offsetCheck(arg2);
		int offset=atoi(arg2);
		if((offset>32767)||(offset<-32768)){
			printf("Error! offsetFields does not fit in 16 bits %s\n",arg2);
			exit(1);
		}
		return ((opcode<<22)|(regA<<19)|(regB<<16)|offset);
	}
	else{
		int label=findLabel(arg2);
		if(label==-1){
			printf("Error! Use of undefined labels %s\n",arg2);
			exit(1);
		}
		return ((opcode<<22)|(regA<<19)|(regB<<16)|label);
	}
}
int beqInstruction(int opcode,char *arg0,char *arg1,char *arg2, int pc){
	//todo arg2가 16bit인지, arg0, arg1 error checking + label도 가능해서 그것도 확인 
	ErrorCheckingIJ(arg0,arg1);
	int regA=atoi(arg0);
	int regB=atoi(arg1);
	//label이면 0 반환
	if(isNumber(arg2)){
		offsetCheck(arg2);
		int offset=atoi(arg2);
		if((offset>32767)||(offset<-32768)){
			printf("Error! offsetFields does not fit in 16 bits %s\n",arg2);
			exit(1);
		}
		return ((opcode<<22)|(regA<<19)|(regB<<16)|offset);
	}
	else{
		int label=findLabel(arg2);
		if(label==-1){
			printf("Error! Use of undefined labels %s\n",arg2);
			exit(1);
		}
		int offset=(label-pc-1);
		offset=offset&0xffff;
		return ((opcode<<22)|(regA<<19)|(regB<<16)|offset);
	}
}
int jalrInstruction(int opcode,char *arg0,char *arg1){
	ErrorCheckingIJ(arg0,arg1);
	int regA=atoi(arg0);
	int regB=atoi(arg1);
	return ((opcode<<22)|(regA<<19)|(regB<<16));
}
int haltInstruction(int opcode){
	return opcode<<22;
}
int noopInstruction(int opcode){
	return opcode<<22;
}
int fillInstruction(char *arg0){
	//isNumber가 1이면 숫자니까 그냥 그대로, 0이면 label이니까 labels에서 주소 받아와야 함
	if(isNumber(arg0)){
		return atoi(arg0);
	}
	if(findLabel(arg0)==-1){
		printf("Error! Use of undefined labels %s\n",arg0);
		exit(1);
	}
	return findLabel(arg0);
}
