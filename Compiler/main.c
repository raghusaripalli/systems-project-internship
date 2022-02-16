#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<conio.h>

char instructions[14][10] =  {"MOVRM", "MOVMR", "ADD", "SUB" ,"MUL", "JUMP", "IF", "EQ", "LT", "GT", "LTEQ", "GTEQ", "PRINT", "READ"};
char registers[8][3] = { "AX", "BX", "CX", "DX", "EX", "FX", "GX", "HX" };

int mem[100];
int stack[100];
int top = -1, ilindex = -1, stindex = -1, ltindex = -1, address = 0, btindex = -1;
typedef struct {
	int no;
	int opcode;
	int params[10];
	int paraIndex;
}intermediatelanguage;
intermediatelanguage il[128];

typedef struct {
	char *name;
	int address;
	int size;
}symboltable;
symboltable st[100];

typedef struct {
	char *name;
	int address;
}blocktable;
blocktable bt[100];

int strcmp(const char* s1, const char* s2)
{
	while (*s1 && (*s1 == *s2))
		s1++, s2++;
	return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strlen(const char *str)
{
	const char *s;
	for (s = str; *s; ++s);
	return(s - str);
}

void variableTokenizer(char *inst)
{
	char *temp = (char*)calloc(100 , sizeof(char));
	int i,j,k,h,flag,size=0;
	for (i = 0, j = 0; inst[i] != '\n'; i++){
		if (inst[i] == ' ' && j != 0){
			break;
		}
		else{
			temp[j++] = inst[i];
		}
	}
	if (strcmp(temp, "DATA")==0){
		++i;
		j = 0;
		flag = 0;
		size = 0;
		temp[j] = '\0';
		for (; inst[i] != '\n' && inst[i] != ']'; i++){
			if (inst[i] == '['){
				flag = 1;
				continue;
			}
			if (flag == 0){
				if (inst[i] != ' ')
					temp[j++] = inst[i];
			}
			else{
				size = (size * 10) + (inst[i] - '0');
			}
		}
		temp[j] = '\0';
		st[++stindex].name = temp;
		if (size == 0){
			st[stindex].size = 1;
			st[stindex].address = address++;
		}
		else{
			st[stindex].size = size;
			st[stindex].address = address;
			address += size;
		}
	}
	else{
		temp[0] = '\0';
		j = 0;
		for (k = ++i; inst[k]!='\n'; k++)
		{
			if (inst[k] == '=')
				break;
			if (inst[k] != ' ')
			{
				temp[j++] = inst[k];
			}
		}
		int value = 0;
		for (h = ++k; inst[h]!='\n' && inst[h]; h++)
		{
			if (inst[h] != ' ')
			{
				value = value * 10 + inst[h] - '0';
			}
		}
		temp[j] = '\0';
		st[++stindex].name = temp;
		mem[address] = value;
		st[stindex].address = address++;
		st[stindex].size = 0;
	}
	return;
}

int register_search(char a)
{
	int i;
	for (i = 0; i < 8; i++)
	{
		if (a == registers[i][0])
			break;
	}
	return i;
}

int isLabel(char *temp)
{
	if (temp[strlen(temp) - 1] == ':'){
		temp[strlen(temp) - 1] = '\0';
		return 1;
	}
	else return 0;
}

void instructionTokenizer(char *inst){
	il[++ilindex].paraIndex = -1;
	char *temp = (char*)calloc(100, sizeof(char));
	int i, j, k, h, flag, size = 0;
	for (i = 0, j = 0; inst[i] != '\n'; i++){
		if (inst[i] == ' ' && j != 0){
			break;
		}
		else{
			if (inst[i] != '\t')
				temp[j++] = inst[i];
		}
	}
	temp[j] = '\0';
	if (strcmp(temp, "READ") == 0){
		while (inst[i] == ' ')i++;
		il[ilindex].no = ilindex + 1;
		il[ilindex].opcode = 14;
		for (j = 0; j < 8; j++){
			if (inst[i] == registers[j][0]){
				il[ilindex].params[++il[ilindex].paraIndex] = j;
				break;
			}
		}
	}
	else if (strcmp(temp, "MOV") == 0)
	{
		int k, number = 0;
		il[ilindex].no = ilindex + 1;
		while (inst[i] == ' ') i++;
		if (inst[i] <= 'H' && inst[i + 1] == 'X')
		{
			il[ilindex].opcode = 2;
			for (k = 0; k < 8; k++)
			{
				if (inst[i] == registers[k][0])
					break;
			}
			il[ilindex].params[++il[ilindex].paraIndex] = i;
			j = 0;
			temp[j] = '\0';
			for (k = i + 3; k < strlen(inst) && k != '['; k++)
				temp[j++] = inst[k];
			temp[j] = '\0';
			if (inst[k] == '[')
				while (inst[k] != ']')
					number = (number * 10) + (inst[k++] - '0');
			if (inst[k] == ']')
				k++;
			for (k = 0; k <= stindex; k++)
			{
				if (strcmp(temp, st[k].name) == 0)
				{
					il[ilindex].params[++il[ilindex].paraIndex] = k + number;
					break;
				}
			}
		}
		else
		{
			j = 0;
			il[ilindex].opcode = 1;
			for (k = i; inst[k] != ',' && inst[k] != '[' && inst[k] != ' '; k++)
				temp[j++] = inst[k];
			temp[j] = '\0';
			if (inst[k] == '['){
				++k;
				while (inst[k] != ']'){
					number = (number * 10) + (inst[k++] - '0');
				}

			}
			if (inst[k] == ']') k++;

			for (h = 0; h <= stindex; h++)
			{
				if (strcmp(temp, st[h].name) == 0)
				{
					il[ilindex].params[++il[ilindex].paraIndex] = h + number;
					break;
				}
			}
			for (i = 0; i < 8; i++)
			{
				if (inst[k + 2] == registers[i][0]) break;
			}
			il[ilindex].params[++il[ilindex].paraIndex] = i;
		}
	}
	else if (temp[0] == 'P')
	{
		while (inst[i] == ' ')
			i++;
		il[ilindex].no = ilindex + 1;
		il[ilindex].opcode = 13;
		if (inst[i + 1] == 'X')
			il[ilindex].params[++il[ilindex].paraIndex] = register_search(inst[i]);
		else
		{
			j = 0;
			temp[0] = '\0';
			for (k = i; inst[k] != '[' && inst[k] != '\n'; k++)
				temp[j++] = inst[k];
			temp[j] = '\0';
			if (inst[k] != '[')
			{
				for (h = 0; h <= stindex; h++)
					if (strcmp(temp, st[h].name) == 0)
						break;
				il[ilindex].params[++il[ilindex].paraIndex] = st[h].address;
			}
			else
			{
				k++;
				int number = 0;
				for (i = 0; i <= stindex; i++)
					if (strcmp(temp, st[i].name) == 0)
						break;
				for (h = k; inst[h] != ']'; h++)
					number = (number * 10) + (inst[h] - '0');
				il[ilindex].params[++il[ilindex].paraIndex] = i + number;
			}
		}
	}
	else if (strcmp(temp, "ADD") == 0)
	{
		il[ilindex].no = ilindex + 1;
		il[ilindex].opcode = 3;
		while (inst[i] == ' ') i++;
		for (i = 4; inst[i] != '\n';){
			il[ilindex].params[++il[ilindex].paraIndex] = register_search(inst[i]);
			i += 2;
			while (inst[i] == ',' || inst[i] == ' ') i++;
		}
	}
	else if (strcmp(temp, "SUB") == 0)
	{
		il[ilindex].no = ilindex + 1;
		il[ilindex].opcode = 4;
		while (inst[i] == ' ') i++;
		for (i = 4; inst[i] != '\n';){
			il[ilindex].params[++il[ilindex].paraIndex] = register_search(inst[i]);
			i += 2;
			while (inst[i] == ',' || inst[i] == ' ') i++;
		}
	}
	else if (strcmp(temp, "MUL") == 0)
	{
		il[ilindex].no = ilindex + 1;
		il[ilindex].opcode = 5;
		while (inst[i] == ' ') i++;
		for (i = 4; inst[i] != '\n';){
			il[ilindex].params[++il[ilindex].paraIndex] = register_search(inst[i]);
			i += 2;
			while (inst[i] == ',' || inst[i] == ' ') i++;
		}
	}
	else if (isLabel(temp) == 1){
		bt[++btindex].name = temp;
		bt[btindex].address = ilindex + 1;
		--ilindex;
	}
	else if (strcmp(temp, "JUMP") == 0){
		il[ilindex].no = ilindex + 1;
		il[ilindex].opcode = 6;
		while (inst[i] == ' ') i++;
		j = 0;
		while (inst[i]!='\n'){
			temp[j++] = inst[i++];
		}
		temp[j] = '\0';
		for (j = 0; j <= btindex; j++){
			if (strcmp(bt[j].name, temp) == 0){
				il[ilindex].params[++il[ilindex].paraIndex] = bt[j].address;
				break;
			}
		}
	}
	else if (strcmp(temp, "IF") == 0){
		int op1, op, op2, flag = 0;
		il[ilindex].no = ilindex + 1;
		il[ilindex].opcode = 7;
		while (inst[i] != '\n'){
			while (inst[i] == ' ') i++;
			if (flag == 0){
				op1 = register_search(inst[i]);
				flag = 1;
				i += 2;
			}
			else if (flag == 1){
				j = 0;
				while (inst[i] != ' '){
					temp[j++] = inst[i++];
				}
				temp[j] = '\0';
				for (k = 0; k < 14; k++){
					if (strcmp(temp, instructions[k]) == 0){
						op = k + 1;
						break;
					}
				}
				flag = 2;
			}
			else if (flag == 2){
				op2 = register_search(inst[i]);
				flag = 3;
				i += 2;
			}
			else{
				i++;
			}
		}
		il[ilindex].params[++il[ilindex].paraIndex] = op1;
		il[ilindex].params[++il[ilindex].paraIndex] = op2;
		il[ilindex].params[++il[ilindex].paraIndex] = op;
		il[ilindex].params[++il[ilindex].paraIndex] = -1;

		stack[++top] = ilindex + 1;
	}
	else if (strcmp(temp, "ENDIF") == 0)
	{
		if (il[stack[top] - 1].params[3] == -1)
			il[stack[top] - 1].params[3] = ilindex + 1;
		else
		{
			il[stack[top] - 1].params[0] = ilindex + 1;
			il[stack[top] - 1].paraIndex++;
		}
		top--;
		ilindex--;
	}
	else if (strcmp(temp, "ELSE") == 0)
	{
		il[ilindex].no = ilindex + 1;
		il[ilindex].opcode = 6;
		il[stack[top] - 1].params[3] = ilindex + 2;
		top--;
		stack[++top] = ilindex + 1;
	}
	else if (strcmp(temp, "END") == 0){
		--ilindex;
	}
	return;
}

int main()
{
	FILE *fp = fopen("input.txt","r");
	char *temp = (char*)malloc(100 * sizeof(char));
	int flag = 0;
	while (fgets(temp, 100, fp) != NULL){
		if (strcmp(temp, "START:\n")==0){
			flag = 1;
			continue;
		}
		if (flag == 0){
			variableTokenizer(temp);
		}
		else{
			instructionTokenizer(temp);
		}
	}
	printf("\n\tSymbol Table\n");
	for (int i = 0; i <= stindex; i++){
		printf("%s %d %d\n", st[i].name, st[i].address, st[i].size);
	}
	printf("\n\tIntermediate Language\n");
	for (int i = 0; i <= ilindex; i++){
		printf("%d : %d : ", il[i].no, il[i].opcode);
		for (int j = 0; j <= il[i].paraIndex; j++){
			printf("%d ", il[i].params[j]);
		}
		printf("\n");
	}
	printf("\n\tLabel Table\n");
	for (int i = 0; i <= btindex; i++){
		printf("%s | %d\n", bt[btindex].name, bt[btindex].address);
	}

	_getch();
	return 0;
}