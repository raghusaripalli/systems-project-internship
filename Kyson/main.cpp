#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<string>
#include<vector>
#include<map>
#include<iostream>
#include<unordered_map>
using namespace std;
int idx = 0;
int tin = 1;

class kyobject{
public:
	int type;
	int count = 0;
	kyobject(int x = 0){
		type = x;
	}

	bool operator <(const kyobject& rhs) const
	{
		return type < rhs.type;
	}

	void toString();
	void serialize();
};
kyobject *res;
int getIndex(kyobject *k);
class kyint : public kyobject{
public:
	int data;
	kyint(int data): kyobject(1){
		kyint::data = data;
	}

	void toString()
	{
		printf("%d ", data);
	}

	void serialize(){
		printf("I%d\n", data);
	}
};
class kystring : public kyobject{
public:
	string s;
	kystring(string s): kyobject(2){
		kystring::s = s;
	}

	void toString()
	{
		cout << "\'" << s << "\'" << " ";
	}

	void serialize(){
		printf("S");
		cout << "\'" << s << "\'" << "\n";
		int idx = getIndex(this);
		printf("p%d\n", idx);
	}
};
class kylist : public kyobject{
public:
	vector < kyobject* > v;
	kylist() : kyobject(3){}
	void ins(kyobject *k){
		v.push_back(k);
	}

	void toString()
	{
		printf("[");
		for (int i = 0; i < v.size(); i++){
			v[i]->toString();
		}
		printf("]");
	}

	void serialize(){
		int idx = getIndex(this);
		count++;
		if (count < 2)
		{
			printf("(lp%d\n", idx);
			for (int i = 0; i < v.size(); i++){
				v[i]->serialize();
				printf("a");
			}
		}
		else
			printf("g%d\n", idx);
	}

};
class kydict : public kyobject{
public:
	kydict() :kyobject(4){}
	map<kyobject*, kyobject*> m;
	void ins(kyobject *first, kyobject *second){
			kydict::m[first] = second;
	}

	void toString()
	{
		printf("{ ");
		for (auto it = m.begin(); it != m.end(); ++it)
		{
			it->first->toString();
			printf(" : ");
			it->second->toString();
		}
		printf(" }");
	}

	void serialize(){
		int idx = getIndex(this);
		
		count++;
		if (count < 2){
			printf("(dp%d\n", idx);
			for (auto it = m.begin(); it != m.end(); ++it)
			{
				it->first->serialize();
				it->second->serialize();
				printf("s");
			}
		}
		else{
			printf("g%d\n", idx);
		}
	}
};
class kychar : public kyobject{
public:
	char ch;
	kychar(char ch) : kyobject(5){
		kychar::ch = ch;
	}
};


void kyobject::toString(){
	if (type == 1){
		kyint *ki = (kyint*)this;
		ki->toString();
	}
	else if (type == 2){
		kystring *ks = (kystring*)this;
		ks->toString();
	}
	else if (type == 3){
		kylist *klist = (kylist*)this;
		klist->toString();
	}
	else if (type == 4){
		kydict *kdict = (kydict*)this;
		kdict->toString();
	}
}

void kyobject::serialize(){
	if (type == 1){
		kyint *ki = (kyint*)this;
		ki->serialize();
	}
	else if (type == 2){
		kystring *ks = (kystring*)this;
		ks->serialize();
	}
	else if (type == 3){
		kylist *klist = (kylist*)this;
		klist->serialize();
	}
	else if (type == 4){
		kydict *kdict = (kydict*)this;
		kdict->serialize();
	}
}

kyobject *stack[100];
int top = -1;

unordered_map <int, kyobject*> address;


int getIndex(kyobject *k)
{
	for (auto it = address.begin(); it != address.end(); ++it)
	{
		if (it->second == k){
			return it->first;
		}
	}
}

void Parse(char *temp)
{
	int i = 0, len = strlen(temp);
	//printf("%s\n", temp);
	while (i < len){
		if (temp[i] == '('){
			kychar *kc = new kychar('(');
			stack[++top] = kc;
			++i;
		}
		else if (temp[i] == 'd'){
			kydict *kd = new kydict();
			stack[++top] = kd;
			//printf("%d", stack[top]);
			++i;
		}
		else if (temp[i] == 'p'){
			i++;
			int key = 0;
			while (temp[i] != '\n'){
				key = (key * 10) + (temp[i] - '0');
				i++;
			}
			address[key]= stack[top];
		}
		else if (temp[i] == 'S'){
			i += 2;
			string ss;
			while(temp[i]!='\'')
				ss += temp[i++];
			kystring *s = new kystring(ss);
			stack[++top] = s;
		}
		else if (temp[i] == 'l'){
			kylist *kl = new kylist();
			stack[++top] = kl;
			++i;
		}
		else if (temp[i] == 'I'){
			++i;
			int num = 0;
			while (temp[i] != '\n'){
				num = (num * 10) + (temp[i] - '0');
				i++;
			}
			kyint *ki = new kyint(num);
			stack[++top] = ki;
		}
		else if (temp[i] == 'a'){
			++i;
			kyobject *ky = stack[top--];
			kylist * kl = (kylist*)stack[top];
			kl->ins(ky);
		}
		else if (temp[i] == 's'){
			kyobject *kval = stack[top--];
			if (stack[top]->type == 5) top--;
			kyobject *kkey = stack[top--];
			kydict *kdict = (kydict *)stack[top];
			kdict->ins(kkey, kval);
			++i;
		}
		else if (temp[i] == 'g'){
			int num = 0;
			++i;
			while (temp[i] != '\n'){
				num = (num * 10) + (temp[i] - '0');
				i++;
			}
			stack[++top] = address[num];
			return;
		}
		else if (temp[i] == '.'){
			while (top > 0){
				if (stack[top]->type == 5) top--;
				else res = stack[top--];
			}
			i++;
		}
		else{
			i++;
		}
	}
}

int main()
{
	char *temp = (char*)malloc(50 * sizeof(char));
	FILE *fp;
	fp = fopen("input.kyser","r");
	while (fgets(temp, 50, fp) != NULL){
		Parse(temp);
	}
	printf("De-serialize:\n\n");
	res->toString();
	printf("\n\n");
	printf("\nSerialize:\n\n");
	res->serialize();
	printf(".");
	_getch();
	return 0;
}