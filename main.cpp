#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include"FileUtil.h"
using namespace std;


void menu()
{
	printf("\n---------- Menu ----------");
	printf("\n1.Register");
	printf("\n2.login");
	printf("\n3.Send Message");
	printf("\n4.Read Message");
	printf("\n5.Delete Message");
	printf("\n6.Upload File");
	printf("\n7.Download File");
	printf("\n8.View ALL Files");
	printf("\n9.Delete File");
	printf("\n10.Sync To My File System");
	printf("\n11.Exit");
	printf("\n\nEnter choice: ");
}



int main()
{
	fp = fopen("filesystem.disk", "rb+");
	if (fp == NULL){
		system("fsutil file createnew filesystem.disk 104857600");
		fp = fopen("filesystem.disk", "rb+");
	}
	globalmetadata gb = loadGlobalMetaData();
	gmd = gb;
	char *username = NULL;
	char *password = NULL;
	pthread_t t1;
	pthread_create(&t1, NULL, sync_call, NULL);
	int choice, i;
	do{
		menu();
		scanf("%d", &choice);
		fflush(stdin);
		switch (choice)
		{
		case 1:
			username = (char*)malloc(sizeof(char) * 16);
			printf("Enter username: ");
			gets(username);
			password = (char*)malloc(sizeof(char) * 16);
			printf("Enter password: ");
			gets(password);
			createUser(username, password);
			break;
		case 2:
			username = (char*)malloc(sizeof(char) * 16);
			printf("Enter username: ");
			gets(username);
			password = (char*)malloc(sizeof(char) * 16);
			printf("Enter password: ");
			gets(password);
			login(username, password);
			break;
		case 3:
			if (username == NULL)
			{
				printf("\n\nPlease Login To Msg!\n");
			}
			else{
				char *filename = (char*)malloc(sizeof(char) * 16);
				printf("Enter filename: ");
				gets(filename);
				char *message = (char*)malloc(sizeof(char) * 128);
				printf("Enter message: ");
				gets(message);
				writeMessage(username, filename, message);
			}
			break;
		case 4:
			if (username == NULL)
			{
				printf("\n\nPlease Login! \n");
				break;
			}
			else{
				char *filename = (char*)malloc(sizeof(char) * 16);
				printf("Enter filename: ");
				gets(filename);
				readMessage(filename);
			}
			break;
		case 5:
			if (username == NULL)
			{
				printf("\n\nPlease Login! \n");
				break;
			}
			else{
				char *filename = (char*)malloc(sizeof(char) * 16);
				printf("Enter filename: ");
				gets(filename);
				deleteMessage(filename, username);
			}
			break;
		case 6:
			if (username == NULL)
			{
				printf("\n\nPlease Login! \n");
				break;
			}
			else{
				char *filename = (char*)malloc(sizeof(char) * 16);
				printf("Enter filename: ");
				gets(filename);
				writeBlobToFile(filename, username);
			}
			break;
		case 7:
			if (username == NULL)
			{
				printf("\n\nPlease Login! \n");
				break;
			}
			else{
				char *filename = (char*)malloc(sizeof(char) * 16);
				printf("Enter filename: ");
				gets(filename);
				downloadblob(filename);
			}
			break;
		case 8:
			if (username == NULL)
			{
				printf("\n\nPlease Login! \n");
			}
			else{
				enumFiles();
			}
			break;
		case 9:
			if (username == NULL)
			{
				printf("\n\nPlease Login! \n");
			}
			else{
				char *filename = (char*)malloc(sizeof(char) * 16);
				printf("Enter filename: ");
				gets(filename);
				deleteBlob(filename);
			}
			break;
		case 10:
			i = connect_to_service();
			if (i == 0){
				printf("\n\nStarting Sync.. Pls be patient\n");
				client_initial_sync();
			}
			break;
		case 11:
			break;
		}
		fflush(fp);
	} while (choice != 11);
	fclose(fp);
	system("pause");
	return 0;
}