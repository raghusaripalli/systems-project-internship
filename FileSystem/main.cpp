#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<string>
FILE *fp;
typedef struct masterboot
{
	int no_of_users;
	int no_of_msgs;
};

typedef struct msgmetadata
{
	int userid;
	char user[20];
};

typedef struct msgdata
{
	int userid;
	char msg[128];
	bool isEmpty=false;
};

masterboot mbr;
msgmetadata mmd[23832];
msgdata md[23832];
char username[20];

/*void print()
{
	printf("\nNo_of_users: %d\n", mbr.no_of_users);
	printf("\nNo_of_msgs: %d\n", mbr.no_of_msgs);
	printf("\nMsg meta data:\n");
	for (int i = 0; i < mbr.no_of_users; i++){
		printf("%d %s\n", mmd[i].userid, mmd[i].user);
	}
	printf("\nMsg data:\n");
	for (int i = 0; i < mbr.no_of_msgs; i++){
		printf("%d %s %d\n", md[i].userid, md[i].msg, md[i].isEmpty);
	}
	printf("\n");
}*/

void loadInput()
{
	fp = fopen("filesystem.txt", "rb+");
	fread(&mbr, sizeof(masterboot), 1, fp);
	fseek(fp, 128, SEEK_SET);
	msgmetadata temp;
	for (int i = 0; i < mbr.no_of_users; i++){
		fread(&temp, sizeof(msgmetadata), 1, fp);
		mmd[i].userid = temp.userid;
		strcpy(mmd[i].user, temp.user);
	}
	fseek(fp, 128, SEEK_SET);
	fseek(fp, 3145728, SEEK_CUR);
	msgdata t;
	for (int i = 0; i < mbr.no_of_msgs; i++){
		fread(&t, sizeof(msgdata), 1, fp);
		md[i].userid = t.userid;
		strcpy(md[i].msg, t.msg);
		md[i].isEmpty = t.isEmpty;
	}
}

void write_back()
{
	fseek(fp, 0, SEEK_SET);
	fwrite(&mbr, sizeof(masterboot),1, fp);
	fseek(fp, 128, SEEK_SET);
	for (int i = 0; i < mbr.no_of_users; i++){
		fwrite(&mmd[i], sizeof(msgmetadata), 1, fp);
	}
	fseek(fp, 128, SEEK_SET);
	fseek(fp, 3145728, SEEK_CUR);
	for (int i = 0; i < mbr.no_of_msgs; i++){
		fwrite(&md[i], sizeof(msgdata), 1, fp);
	}
}
void create()
{
	char usermsg[128];
	int i, j, id, flag = 0;
	printf("Enter msg: ");
	fflush(stdin);
	gets(usermsg);
	for (i = 0; i < mbr.no_of_users; i++){
		if (strcmp(mmd[i].user, username) == 0){
			flag = 1;
			id = mmd[i].userid;
			break;
		}
	}
	if (flag == 1){
		for (j = 0; j < mbr.no_of_msgs; j++) if(md[j].isEmpty==true) break;
		md[j].userid = id;
		strcpy(md[j].msg, usermsg);
		md[j].isEmpty = false;
		mbr.no_of_msgs++;
	}
	else{
		mmd[i].userid = i;
		strcpy(mmd[i].user, username);
		mbr.no_of_users++;
		for (j = 0; j < mbr.no_of_msgs; j++) if (md[j].isEmpty == true){ mbr.no_of_msgs--; break; }
		md[j].userid = i;
		strcpy(md[j].msg, usermsg);
		md[j].isEmpty = false;
		mbr.no_of_msgs++;
	}
}

void read()
{
	int i, j, cnt = 0, flag = 0;
	for (i = 0; i < mbr.no_of_users; i++){
		if (strcmp(mmd[i].user, username) == 0){
			break;
		}
	}
	for (j = 0; j < mbr.no_of_msgs; j++){
		if (md[j].userid == i && md[j].isEmpty!=1){
			flag = 1;
			printf("%d-> %s\n", ++cnt, md[j].msg);
		}
	}
	if (flag == 0){
		printf("\n\tYou have not commented yet!!\n");
	}
}

void del()
{
	char usermsg[128];
	int i, j, id, flag = 0;
	printf("Enter msg to be deleted: ");
	fflush(stdin);
	gets(usermsg);
	for (i = 0; i < mbr.no_of_users; i++){
		if (strcmp(mmd[i].user, username) == 0){
			break;
		}
	}
	for (j = 0; j < mbr.no_of_msgs; j++){
		if (strcmp(md[j].msg, usermsg) == 0 && md[j].userid==i){
			flag = 1;
			md[j].isEmpty = true;
			break;
		}
	}
	if (flag == 0){
		printf("\n\tNo such msg exists");
	}
}

int main()
{
	int choice;
	loadInput();
	
	printf("Enter username: ");
	scanf("%s", &username);
	do{
		//print();
		printf("1.Create\n");
		printf("2.Read\n");
		printf("3.Delete\n");
		printf("4.exit\n");
		printf("Enter your choice: ");
		scanf("%d", &choice);
		switch (choice){
		case 1: create(); break;
		case 2: read(); break;
		case 3: del(); break;
		case 4: write_back();  
			break;
		default: printf("No such option!\n");
		}
	} while (choice!=4);
	
	_getch();
	return 0;
}
