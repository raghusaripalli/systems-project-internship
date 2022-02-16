#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<iostream>
#include<vector>
#include<iterator>
#include<map>
#define PAGE_SIZE 64
using namespace std;
typedef struct
{
	int id;
	int courseid;
	char name[19];
	__int8 marks;

}student;

typedef struct
{
	int data[7];
	int children[8];

}nonleafpage;

typedef struct
{
	char pgtype;
	char tableid;
	short level;
	char data[0];

}page;

typedef struct 
{
	int id;
	char name[16];
}course;

typedef struct 
{
	course courses[3];
}course_leaf_page;

typedef struct 
{
	int table_id;
	int root_page;
	char table_name[8];
}table_info;

typedef struct 
{
	int pad[3];
	table_info tables[3];
}header;

typedef struct
{
	int pad;
	student entries[2];
}leafpage;

typedef struct
{
	bool free;
	int pageid;
	int time;
	void *ptr;
}pagecache;


FILE *fp, *fp2;
page *pg;
leafpage *lf;
pagecache pc[4];
nonleafpage *nlp;
int c = 0, cnt = 1;
int no_of_records;
int page_count = 0;
course_leaf_page *clf;
int hashtable[14];
map<string, vector<string>> mp;

int lru()
{
	int i;
	for (i = 0; i < 4; i++)
		if (pc[i].free == false)
			return i;
	int min = 0;
	for (i = 0; i < 4; i++)
	{
		if (pc[min].time > pc[i].time)
			min = i;
	}
	return i;
}

void* allocpage(){
	return malloc(PAGE_SIZE);
}

void swap(int *a, int *b)
{
	*a = *a ^ *b;
	*b = *a ^ *b;
	*a = *a ^ *b;
}

void writepage(FILE *fp, int offset, void *memory)
{
	fseek(fp, offset, SEEK_SET);
	fwrite(memory, PAGE_SIZE, 1, fp);
}


void readpage(FILE *fp, int offset)
{
fseek(fp, offset, SEEK_SET);
fread(p, PAGE_SIZE, 1, fp);
}

void constructLeafNode_student(char *line)
{
	if (c == 0){
		pg = (page*)allocpage();
		lf = (leafpage *)pg->data;
	}
	pg->pgtype = '1';
	pg->level = 0;
	pg->tableid = '0';

	int i, j, num = 0;
	for (i = 0; line[i] != ' '; i++){
		num = (num * 10) + (line[i] - '0');
	}
	lf->entries[c].id = num;
	++i; j = 0;
	student s;
	s.id = num;
	for (; line[i] != ' '; i++){
		s.name[j++] = line[i];
	}
	++i;
	num = 0;
	for (; line[i] != ' ' && line[i]!='\0'; i++){
		num = (num * 10) + (line[i] - '0');
	}
	++i;
	s.courseid = num;
	s.name[j] = '\0';
	__int8 n1 = 0;
	for (; line[i] != '\n' && line[i] != '\0'; i++){
		n1 = (n1 * 10) + (line[i] - '0');
	}
	s.marks = n1;
	lf->entries[c] = s;
	c++;
	if (c == 2){
		fwrite(pg, PAGE_SIZE, 1, fp);
		c = 0;
	}
}

void constructNonleafNode()
{
	// skip 1st block
	pg = (page*)allocpage();
	nlp = (nonleafpage *)pg->data;
	leafpage *lp;
	
	int no_of_nodes = no_of_records / 2;
	page_count = no_of_nodes;
	int i = 0, j;
	int lvl = 1;
	printf("\nno_of_pages: %d\n", no_of_nodes);
	if (lvl == 1){
		for (i = 0; i < no_of_nodes / 8; i++){
			nlp->children[0] = cnt++;
			fseek(fp, (i * 8 * PAGE_SIZE) + (PAGE_SIZE*2), SEEK_SET);
			for (j = 1; j < no_of_nodes / 2; j++)
			{
				page *pg1 = (page*)malloc(PAGE_SIZE);
				fread(pg1, PAGE_SIZE, 1, fp);
				lp = (leafpage*)pg1->data;
				student *s = (student*)malloc(sizeof(student));
				s->id = lp->entries[0].id;
				nlp->data[j - 1] = s->id;
				nlp->children[j] = cnt++;
			}
			pg->pgtype = '2';
			pg->level = lvl;
			pg->tableid = '0';
			fseek(fp, page_count * PAGE_SIZE, SEEK_SET);
			fseek(fp, 64, SEEK_CUR);
			page_count++;
			fwrite(pg, PAGE_SIZE, 1, fp);
		}
		lvl++;
		no_of_nodes /= 8;
	}
	if (lvl >= 2){
		while (no_of_nodes > 1)
		{
			for (i = 1; i <= no_of_nodes / 2; i++)
			{
				fseek(fp, (page_count - 1) * PAGE_SIZE, SEEK_SET);
				fseek(fp, 64, SEEK_CUR);
				int current_level = 1, k = 0;
				page *pg2 = (page*)malloc(PAGE_SIZE);
				nonleafpage *nlp1 = (nonleafpage *)pg2->data;
				nlp1->children[0] = cnt++;
				do
				{
					page *pg1 = (page*)malloc(PAGE_SIZE);
					fread(pg1, PAGE_SIZE, 1, fp);
					nlp = (nonleafpage*)pg1->data;
					fseek(fp, nlp->children[0] * PAGE_SIZE, SEEK_SET);
					current_level--;
				} while (current_level >= 1);
				page *pg3 = (page*)malloc(PAGE_SIZE);
				fread(pg3, PAGE_SIZE, 1, fp);
				leafpage *lp1 = (leafpage*)pg3->data;
				int number = lp1->entries[0].id;
				nlp1->data[k++] = number;
				for (int x = k; x < 7; x++){
					nlp1->data[x] = 0;
				}
				nlp1->children[k] = cnt++;
				for (int x = k + 1; x < 8; x++){
					nlp1->children[x] = 0;
				}
				pg2->level = lvl;
				lvl++;
				pg2->pgtype = '2';
				pg2->tableid = '0';
				fseek(fp, page_count * PAGE_SIZE, SEEK_SET);
				fseek(fp, 64, SEEK_CUR);
				fwrite(pg2, PAGE_SIZE, 1, fp);
			}
			page_count++;
			no_of_nodes /= 2;
		}

	}
}

void print()
{
	int j = 0;
	fseek(fp, 64, SEEK_SET);
	page *pg = (page*)allocpage();
	for (int i = 0; i < 16; i++)
	{
		fread(pg, PAGE_SIZE, 1, fp);
		if (pg->pgtype == '1'){
			//printf("Entered here");
			leafpage *lf = (leafpage *)pg->data;
			printf("%d %s %d %d\n", lf->entries[0].id, lf->entries[0].name, lf->entries[0].courseid, lf->entries[0].marks);
			printf("%d %s %d %d\n", lf->entries[1].id, lf->entries[1].name, lf->entries[1].courseid, lf->entries[1].marks);
		}
	}
}

void print_nlp(int offset, int no_of_nodes)
{
	int j = 0;
	fseek(fp, offset*PAGE_SIZE, SEEK_SET);
	fseek(fp, 64, SEEK_CUR);
	page *pg = (page*)allocpage();
	for (j = 0; j < no_of_nodes; j++)
	{
		pg = (page*)malloc(PAGE_SIZE);
		fread(pg, PAGE_SIZE, 1, fp);
		//printf("Entered here");
		nonleafpage *nlf = (nonleafpage *)pg->data;
		printf("\nchildren: ");
		for (int i = 0; i < 8; i++)
			printf("%d ", nlf->children[i]);
		printf("\ndata: ");
		for (int i = 0; i < 7; i++)
			printf("%d ", nlf->data[i]);
	}
}

void place_root()
{
	table_info *ti = (table_info*)malloc(sizeof(table_info));
	ti->root_page = 19;
	ti->table_id = 0;
	strcpy(ti->table_name, "Students");
	page *pg3 = (page*)allocpage();
	header *h = (header*)malloc(sizeof(header));
	h = (header *)pg3->data;
	h->tables[0] = *ti;
	table_info *ti1 = (table_info*)malloc(sizeof(table_info));
	ti1->root_page = 24;
	ti1->table_id = 0;
	strcpy(ti1->table_name, "Courses");
	h->tables[1] = *ti1;
	pg3->pgtype = '2';
	pg3->tableid = '9';
	pg3->level = -1;
	fwrite(pg3, 64, 1, fp);
	page_count++;
}

void constructLeafNode_course(char *str)
{
	if (c == 0){
		pg = (page*)allocpage();
		clf = (course_leaf_page *)pg->data;
	}
	pg->pgtype = '1';
	pg->level = 0;
	pg->tableid = '1';
	int i = 0, number = 0;
	while (str[i] != ','){
		number = number * 10 + str[i++] - '0';
	}
	++i;
	course cc;
	cc.id = number;
	int j = 0;
	while (str[i] != '\0' && str[i] != '\n')
		cc.name[j++] = str[i++];
	cc.name[j] = '\0';
	clf->courses[c] = cc;
	c++;
	if (c == 3){
		fwrite(pg, PAGE_SIZE, 1, fp);
		page_count++;
		c = 0;
	}
}

void constructNonLeafNode_course()
{
	pg = (page*)allocpage();
	page *pg2 = (page*)allocpage();
	nonleafpage *nlp1 = (nonleafpage *)pg2->data;
	course_leaf_page *clp;
	fseek(fp, 20 * 64, SEEK_SET);
	fseek(fp, 64, SEEK_CUR);
	int i;
	for (i = 0; i < 3; i++)
	{
		clp = (course_leaf_page*)pg->data;
		fread(pg, 64, 1, fp);
			nlp1->data[i] = clp->courses[0].id;
	}
	for (; i < 7; i++) nlp1->data[i] = 0;
	int temp = 20;
	fread(pg, 64, 1, fp);
	for (i = 0; i < 4; i++)
	{
		nlp1->children[i] = temp++;
	}
	for (; i < 8; i++) nlp1->children[i] = 0;
	pg2->tableid = '1';
	pg2->pgtype = '2';
	page_count++;
	pg2->level = 2;
	fwrite(pg2, 64, 1, fp);
}

int search_stud(int id, int offset, int no)
{
	fseek(fp, offset*64, SEEK_SET);
	page *pg2 = (page*)allocpage();
	fread(pg2, 64, 1, fp);
	if (pg2->level >= 2){
		nonleafpage *nlp = (nonleafpage*)pg2->data;
		for (int i = 0; i < no; i++){
			if (id < nlp->data[i]){
				if (pg2->level == 2) no *= 8;
				else no *= 2;
				return search_stud(id, nlp->children[i], no);
			}
			else if (id == nlp->data[i]){
				return 1;
			}
			else if (id > nlp->data[i]){
				if (i == no - 1){
					if (pg2->level == 2) no *= 8;
					else no *= 2;
					return search_stud(id, nlp->children[i + 1], no);
				}
			}
		}
	}
	else if (pg2->level ==1){
		nonleafpage *nlp = (nonleafpage*)pg2->data;
		for (int i = 0; i < 7; i++){
			if (id < nlp->data[i]){
				return search_stud(id, nlp->children[i], no);
			}
			else if (id == nlp->data[i]){
				return 1;
			}
			else if (id > nlp->data[i]){
				if (i == 7 - 1){
					return search_stud(id, nlp->children[i + 1], no * 2);
				}
			}
		}
	}
	else{
		leafpage *lp = (leafpage*)pg2->data;
		if (id != lp->entries[0].id && id!=lp->entries[1].id){
			return 0;
		}
		else{
			return 1;
		}
	}
}

int count_course(int offset)
{
	fseek(fp, offset * 64, SEEK_SET);
	page *pg2 = (page*)allocpage();
	fread(pg2, 64, 1, fp);
	if (pg2->level >= 1){
		nonleafpage *nlp = (nonleafpage*)pg2->data;
		return count_course(nlp->children[0]);
	}
	else{
		fseek(fp, -64, SEEK_CUR);
		int max = -1;
		for (int i = 0; i < 16; i++){
			page *pg3 = (page*)allocpage();
			fread(pg3, 64, 1, fp);
			leafpage *lp2 = (leafpage *)pg3->data;
			for (int j = 0; j < 2; j++){
				hashtable[lp2->entries[j].courseid]++;
				if (max < hashtable[lp2->entries[j].courseid])
					max = hashtable[lp2->entries[j].courseid];
			}
		}
		return max;
	}
}

void range_query(int min, int max, int offset, int no)
{
	fseek(fp, offset * 64, SEEK_SET);
	page *pg2 = (page*)allocpage();
	fread(pg2, 64, 1, fp);
	if (pg2->level >= 2){
		nonleafpage *nlp = (nonleafpage*)pg2->data;
		for (int i = 0; i < no; i++){
			if (min < nlp->data[i]){
				if (pg2->level == 2) no = no * 8;
				else no *= 2;
				return range_query(min, max, nlp->children[i], no);
			}
			else if (min == nlp->data[i]){
				return range_query(min, max, nlp->children[i], no);
			}
			else if (min > nlp->data[i]){
				if (i == no - 1){
					if (pg2->level == 2) no = no * 8;
					else no *= 2;
					return range_query(min, max, nlp->children[i + 1], no);
				}
			}
		}
	}
	else if (pg2->level == 1){
		nonleafpage *nlp = (nonleafpage*)pg2->data;
		for (int i = 0; i < 7; i++){
			if (min < nlp->data[i]){
				return range_query(min,max, nlp->children[i], no);
			}
			else if (min == nlp->data[i]){
				return range_query(min, max, nlp->children[i], no);
			}
			else if (min > nlp->data[i]){
				if (i == 7 - 1){
					return range_query(min, max, nlp->children[i + 1], no);
				}
			}
		}
	}
	else{
		//fseek(fp, -64 , SEEK_CUR);
		int sum = 0, idx = 0;
		for (int i = offset; i < 16; i++){
			page *pg3 = (page*)allocpage();
			fread(pg3, 64, 1, fp);
			leafpage *lp2 = (leafpage *)pg3->data;
			//if (lp2->entries[0].id > max) return;
			for (int j = 0; j < 2; j++){
				if (lp2->entries[j].id >= min && lp2->entries[j].id <= max){
					printf("%d %s %d %d\n ", lp2->entries[j].id, lp2->entries[j].name, lp2->entries[j].courseid, lp2->entries[j].marks);
					sum += lp2->entries[j].marks;
					idx++;
				}
			}
		}
		printf("\nNo of records in range = %d\n", idx);
		if (idx == 0) idx = 1;
		printf("\nAVG over query = %d", sum/idx);
		return;
	}
}

void getCourse(int id, int offset, string s)
{
	fseek(fp, offset * 64, SEEK_SET);
	page *pg2 = (page*)allocpage();
	fread(pg2, 64, 1, fp);
	if (pg2->level >= 1){
		nonleafpage *nlpa = (nonleafpage*)pg2->data;
		getCourse(id, nlpa->children[0], s);
	}
	else{
		fseek(fp, -64, SEEK_CUR);
		//printf("<< %d >> \n", ftell(fp));
		for (int i = 0; i < 4; i++){
			page *pg3 = (page*)allocpage();
			fread(pg3, 64, 1, fp);
			course_leaf_page *clp2 = (course_leaf_page *)pg3->data;
			for (int j = 0; j < 3; j++){
				if (id == clp2->courses[j].id){
					printf("%s\n", clp2->courses[j].name);
					mp[clp2->courses[j].name].push_back(s);
				}
			}
		}
		return;
	}
}

void join(int offset1, int offset2)
{
	fseek(fp, offset1 * 64, SEEK_SET);
	page *pg2 = (page*)allocpage();
	int pos = 0;
	fread(pg2, 64, 1, fp);
	if (pg2->level >= 1){
		nonleafpage *nlp = (nonleafpage*)pg2->data;
		return join(nlp->children[0], offset2);
	}
	else{
		fseek(fp, -64, SEEK_CUR);

		for (int i = 0; i < 16; i++){
			page *pg3 = (page*)allocpage();
			fread(pg3, 64, 1, fp);
			pos = ftell(fp);
			leafpage *lp2 = (leafpage *)pg3->data;
			for (int j = 0; j < 2; j++){
				printf("%s ", lp2->entries[j].name);
				getCourse(lp2->entries[j].courseid,  offset2, lp2->entries[j].name);
				//mp[a].push_back(lp2->entries[j].name);
			}
			fseek(fp, pos, SEEK_SET);
		}
		return;
	}
}

int main()
{
	FILE *f1 = fopen("input.txt", "r");
	fp = fopen("output.txt", "wb+");
	fp2 = fopen("Courses.csv", "r");
	char *temp = (char*)malloc(20 * sizeof(char));
	int c1 = 0;
	place_root();
	fseek(fp, 64, SEEK_SET);
	while (fgets(temp, 20, f1) != NULL){
		constructLeafNode_student(temp);
		c1++;
	}
	no_of_records = (c1);
	printf("No_of_records: %d\n", no_of_records);
	constructNonleafNode();
	print();
	print_nlp(16, 2);
	print_nlp(18, 1);
	c1 = 0; no_of_records = 0;
	free(pg);
	fseek(fp, page_count * 64, SEEK_SET);
	fseek(fp, 64, SEEK_CUR);
	c = 0;
	while (fgets(temp, 20, fp2) != NULL)
	{
		//("%s", temp);
		constructLeafNode_course(temp);
		c1++;
	}
	fclose(fp2);
	constructNonLeafNode_course();
	printf("->%d", ftell(fp));
	fseek(fp, 1536, SEEK_SET);
	page *pga = (page*)allocpage();
	fread(pga, 64, 1, fp);
	nonleafpage *nlpa = (nonleafpage*)pga->data;
	no_of_records = (c1);
	int id;
	fseek(fp, 0, SEEK_SET);
	page *pg2 = (page*)allocpage();
	fread(pg2, 64, 1, fp);
	header *hd = (header*)pg2->data;
	printf("\n\n\tSEARCH_MODULE: \n");
	printf("\nEnter Student Id: ");
	scanf("%d", &id);
	if (search_stud(id, hd->tables[0].root_page, 1) == 1)
		printf("\nFound");
	else
		printf("\nNot found");
	printf("\n\n\tCOURSE_COUNT MODULE: \n");
	fseek(fp, 0, SEEK_SET);
	page *pg3 = (page*)allocpage();
	fread(pg3,64, 1, fp);
	header *hd1 = (header*)pg3->data;
	for (int d = 0; d < 14; d++) hashtable[d] = 0;
	printf("\nMax Count = %d ", count_course(hd1->tables[0].root_page));
	fseek(fp, 0, SEEK_SET);
	page *pg4 = (page*)allocpage();
	fread(pg4, 64, 1, fp);
	header *hd2 = (header*)pg3->data;
	int min=0, max=0;
	printf("\n\n\tRANGE_QUERIES MODULE: \n");
	printf("\nEnter min, max values to query: ");
	scanf("%d%d", &min, &max);
	if (max < min) swap(&max, &min);
	range_query(min,max,hd1->tables[0].root_page, 1);
	printf("\n\n\tJOIN & GROUPBY QUERY MODULE: \n");
	fseek(fp, 0, SEEK_SET);
	page *pg5 = (page*)allocpage();
	fread(pg5, 64, 1, fp);
	header *hd3 = (header*)pg5->data;
	printf("<<%d %d>>", hd->tables[0].root_page, hd->tables[1].root_page);
	//getCourse(1, hd->tables[1].root_page);
	join(hd->tables[0].root_page, hd->tables[1].root_page);
	printf("\n");
	for (auto it = mp.cbegin(); it != mp.cend(); ++it)
	{
		printf("%s: ", it->first.c_str());
		for (int z = 0; z < it->second.size(); z++){
			printf("%s, ", it->second[z].c_str());
		}
		printf("\n");
	}
	_getch();
	return 0;
}
