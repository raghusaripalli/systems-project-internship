#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<conio.h>

typedef struct
{
	char exp[20];
}node;
node n[26][26];

int ss = 0;
int fi = 0;
int strlen(const char *str)
{
	int i;
	for (i = 0; str[i] != '\0' && str[i] != '\n'; i++);
	return i;
}

int isNum(char a)
{
	if (a >= '0' && a <= '9')
		return 1;
	return 0;
}

int isop(char a)
{
	if (a == '+' || a == '-' || a == '*' || a == '^')
		return 1;
	return 0;
}

int modular_exp(int base, int exp)
{
	int result = 1;
	while (exp > 0){
		if (exp & 1)
			result *= base;
		exp = exp >> 1;
		base *= base;
	}
	return result;
}

void _str_copy(char dest[20], char src[20]){
	int i;
	for (i = 0; i<strlen(src); i++){
		dest[i] = src[i];
	}
	dest[i] = '\0';
}

void init()
{
	for (int i = 0; i < 26; i++)
		for (int j = 0; j < 26; j++)
		{
			_str_copy(n[i][j].exp, "0");
		}
}

int splitIndex(char *s)
{
	int i;
	for (i = 0; s[i] != '\n' && s[i] != '\0'; i++){
		if (isop(s[i]) == 1) return i;
	}
	return 0;
}
int itr = 0;
int Eval(node *temp)
{
	if (isNum(temp->exp[0]) == 1)
	{
		int res = 0;
		for (int i = 0; temp->exp[i] != '\0'; i++)
			res = (res * 10) + (temp->exp[i] - '0');
		return res;
	}
	else
	if (isLetter(temp->exp[1])==1){
		int ans, i, j, p, q, r, row, col, sum = 0, c, max = -1, min = 9999;
		int row1, col1, row2, col2;
				p = 3;
				if (temp->exp[p + 2] == ')'){
					if (isNum(temp->exp[p + 1]) == 0)
					{
						row = (int)temp->exp[p + 1] - 'a';
						sum = 0; c = 0; max = -1; min = 9999;
						for (p = 0; p < 26; p++){
							ans = Eval(&n[row][p]);
							if (ans != 0) c++;
							sum += ans;
							if (ans > max) max = ans;
							if (ans < min) min = ans;

						}
						if (temp->exp[1] == 'u')
							return sum;
						else if (temp->exp[1] == 'v')
							return sum / c;
						else if (temp->exp[1] == 'a')
							return max;
						else
							return min;
					}
					else{
						col = 0;
						++p;
						while (temp->exp[p] != ')')
							col = (col * 10) + (temp->exp[p++] - '0');
						--col;
						sum = 0; c = 0; max = -1; min = 9999;
						for (p = 0; p < 26; p++){
							ans = Eval(&n[p][col]);
							if (ans != 0) c++;
							sum += ans;
							if (ans > max) max = ans;
							if (ans < min) min = ans;
						}
						if (temp->exp[1] == 'u')
							return sum;
						else if (temp->exp[1] == 'v')
							return sum / c;
						else if (temp->exp[1] == 'a')
							return max;
						else
							return min;
					}
				}
				else{
					++p;
					while (temp->exp[p] == ' ')p++;
					row1 = (int)temp->exp[p++] - 'a';
					col1 = 0;
					while (temp->exp[p] != ':'){
						col1 = (col1 * 10) + (temp->exp[p] - '0');
						p++;
					}
					--col1;
					++p;
					row2 = (int)temp->exp[p++] - 'a';
					col2 = 0;
					while (temp->exp[p] != ')'){
						col2 = (col2 * 10) + (temp->exp[p] - '0');
						p++;
					}
					--col2;
					sum = 0; c = 0; max = -1; min = 9999;
					if (row1 == row2){
						for (p = col1; p < col2; p++){
							ans = Eval(&n[row1][p]);
							if (ans != 0) c++;
							sum += ans;
							if (ans > max) max = ans;
							if (ans < min) min = ans;
						}
					}
					else{
						for (p = row1; p < row2; p++){
							ans = Eval(&n[p][col1]);
							if (ans != 0) c++;
							sum += ans;
							if (ans > max) max = ans;
							if (ans < min) min = ans;
						}
					}
					if (temp->exp[1] == 'u')
						return sum;
					else if (temp->exp[1] == 'v')
						return sum / c;
					else if (temp->exp[1] == 'a')
						return max;
					else
						return min;
				}
			}
	else
	{
		char op;
		int i, j, row1, col1, ex = splitIndex(temp->exp);
		if (ex != 0)
		{
			node n1, n2;
			j = 0;
			for (i = 0; i < ex; i++){
				n1.exp[j++] = temp->exp[i];
			}
			n1.exp[j] = '\0';
			op = temp->exp[i++];
			j = 0;
			for (; temp->exp[i]; i++){
				n2.exp[j++] = temp->exp[i];
			}
			n2.exp[j] = '\0';
			switch (op){
			case '-':
				return Eval(&n1) - Eval(&n2);
				break;
			case '+':
				return Eval(&n1) + Eval(&n2);
				break;
			case '*':
				return Eval(&n1) * Eval(&n2);
				break;
			case '^':
				return modular_exp(Eval(&n1), Eval(&n2));
				break;
			default:
				printf("\nInvalid Operand");
				return 1;
			}
		}
		else
		{
			row1 = (int)temp->exp[0] - 'a';
			col1 = 0;
			for (i = 1; temp->exp[i]; i++){
				col1 = (col1 * 10) + (temp->exp[i] - '0');
			}
			--col1;
			return Eval(&n[row1][col1]);
		}
	}
}

int strcmp(const char* s1, const char* s2)
{
	while (*s1 && (*s1 == *s2))
		s1++, s2++;
	return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int isLetter(char s)
{
	if (s >= 'a' && s <= 'z') return 1;
	return 0;
}

void printSheet()
{
	int ans, i, j, p, q, r, row, col, sum = 0, c,max = -1, min=9999;
	int row1, col1, row2, col2;
	for (i = 0; i < 26; i++){
		for (j = 0; j < 26; j++){
				ans = Eval(&n[i][j]);
				if (ans ==  0) printf(" . ");
				else printf("%2d ", ans);
		}
		printf("\n");
	}
	printf("\n\n");
}

void dolower(char *s)
{
	int i;
	for (i = 0; i < strlen(s); i++){
		if (s[i] >= 'A' && s[i] <= 'Z') s[i] = s[i] - 'A' + 'a';
	}
}

void exec(char *inst)
{
	char *temp = (char*)malloc(20 * sizeof(temp));
	int i, j, col, row, res, num;
	i = 3; j = 0; col; row;
	if (inst[i] == 'F') i++;
	while (inst[i] == ' ') i++;
	row = (int)inst[i++] - 'A';
	col = 0;
	for (; inst[i] != ' '; i++){
		col = (col * 10) + (inst[i] - '0');
	}
	--col;
	while (inst[i] == ' ') i++;
	for (; inst[i] != '\n'; i++){
		n[row][col].exp[j++] = inst[i];
	}
	n[row][col].exp[j] = '\0';
	dolower(n[row][col].exp);
}

void Parse(char *inst)
{
	char *temp = (char*)malloc(20 * sizeof(temp));
	int i, j, col, row, res, num;

	if (inst[0] == 'I'){
		fi = 1;
		i = 2;
		while (inst[i] == ' ') i++;
		row = (int)inst[i++] - 'A';
		col = 0;
		for (; inst[i] != ' '; i++){
			col = (col * 10) + (inst[i] - '0');
		}
		--col;
		res = Eval(&n[row][col]);
		j = 0;
		while (inst[i] == ' ') i++;
		for (; inst[i] != ' '; i++){
			temp[j++] = inst[i];
		}
		temp[j] = '\0';
		num = 0;
		while (inst[i] == ' ') i++;
		for (; inst[i] != '\n'; i++){
			num = (num * 10) + (inst[i] - '0');
		}
		if (strcmp(temp, "EQ") == 0){
			if (res == num){
				ss = 1;
			}
		}
	}
	else if (strcmp(inst, "ELSE\n")==0){
		if (ss == 1) ss = 0;
		else fi = 0;
	}
	else if (strcmp(inst, "ENDIF\n") == 0){
		ss = 0;
		fi = 0;
		return;
	}
	else if (fi == 1){
		if (ss == 1) exec(inst);
	}
	else{
		exec(inst);
	}
}

int main()
{
	char *temp = (char*)malloc(100 * sizeof(temp));
	FILE *fp = fopen("input.txt", "r");
	init();
	while (fgets(temp, 100, fp)){
		Parse(temp);
		printSheet();
	}
	_getch();
	return 0;
}