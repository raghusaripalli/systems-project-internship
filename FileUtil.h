#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define lli long long int

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <conio.h>
#include <pthread.h>
#include"md5header.h"


// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 6416

WSADATA wsaData;
SOCKET ListenSocket = INVALID_SOCKET;
SOCKET ClientSocket = INVALID_SOCKET;
SOCKET ServerSocket = INVALID_SOCKET;

struct addrinfo *result1 = NULL;
struct addrinfo *result2 = NULL;
struct addrinfo hints1, hints2;

char ip_address[16] = "127.0.0.1";
char port1[6] = "27016";
char serverport[6] = "98765";
char serverport1[6] = "45678";
char port2[6] = "87654";
int iSendResult;
FILE *fp;

//0000a00
//00002a00
//00102a00

// define structures
typedef struct
{
	char username[16];
	char password[16];
	int pad[24];
}users;

typedef struct
{
	char filename[16];
	lli msgidx;
	lli useridx;
	int pad[24];
}msgmetadata;

typedef struct
{
	bool flag[128];
}bitvector;

typedef struct
{
	char msg[128];
}msgdata;

typedef struct
{
	bool flag[128];
}blobbitvector;

typedef struct
{
	char filename[16];
	lli start;
	lli useridx;
	lli end;
	lli blocksrequired;
	int pad[20];
}blobmetadata;

typedef struct
{
	lli useridx;
	lli bitvectoridx;
	lli msgmetadataidx;
	lli msgidx;
	lli usercnt;
	lli bitvectorcnt;
	lli blobcnt;
	lli blobmetadataidx;
	lli blobbitvectoridx;
	lli blobmsgidx;
	int pad[12];
}globalmetadata;

typedef struct{
	char data[128];
}blobdata;

globalmetadata gmd;

// message code

void writeMetaData()
{
	fseek(fp, 0, SEEK_SET);
	fwrite(&gmd, 128, 1, fp);
}

globalmetadata loadGlobalMetaData()
{
	globalmetadata gb;
	fseek(fp, 0, SEEK_SET);
	fread(&gb, 128, 1, fp);
	globalmetadata gbb;
	if (gb.useridx == 0)
	{
		gbb.useridx = 128;
		gbb.bitvectoridx = 2560;
		gbb.msgmetadataidx = 10752;
		gbb.msgidx = 1059328; //00102A00
		gbb.usercnt = 0;
		gbb.blobmsgidx = 2124416; //00206A80
		gbb.bitvectorcnt = 0;
		gbb.blobbitvectoridx = 2107904; //00202A00	
		gbb.blobcnt = 0;
		gbb.blobmetadataidx = 2116096;	//00204A00
		rewind(fp);
		fwrite(&gbb, 128, 1, fp);
		return gbb;
	}
	rewind(fp);
	globalmetadata g;
	fread(&g, 128, 1, fp);
	return gb;
}

void createUser(char *uname, char *pwd)
{
	fseek(fp, gmd.useridx, SEEK_SET);
	users u;
	for (int i = 0; i < gmd.usercnt; i++)
	{
		fread(&u, 128, 1, fp);
		if (strcmp(u.username, uname) == 0){
			printf("\n\nName is already taken. Try other name :)\n\n");
			return;
		}
	}
	fseek(fp, (gmd.usercnt + 1) * gmd.useridx, SEEK_SET);
	users write;
	strcpy(write.username, uname);
	strcpy(write.password, pwd);
	fwrite(&write, 128, 1, fp);
	gmd.usercnt += 1;
	writeMetaData();
	return;
}

lli findUserOffset(char *uname)
{
	fseek(fp, gmd.useridx, SEEK_SET);
	users u;
	for (int i = 0; i < gmd.usercnt; i++)
	{
		fread(&u, 128, 1, fp);
		if (strcmp(u.username, uname) == 0){
			return ftell(fp);
		}
	}
	return -1;
}

lli getBit()
{
	fseek(fp, gmd.bitvectoridx, SEEK_SET);
	bitvector bv;
	int cnt = 0;
	while (1)
	{
		fread(&bv, 128, 1, fp);
		for (int i = 0; i < 128; i++){
			if (bv.flag[i] == false){
				bv.flag[i] = true;
				int pos = ftell(fp) - 128;
				fseek(fp, pos, SEEK_SET);
				bitvector bv1 = bv;
				fwrite(&bv1, 128, 1, fp);
				fseek(fp, 0, SEEK_SET);
				gmd.bitvectorcnt = cnt;
				return cnt;
			}
			++cnt;
		}
	}
}

void writeMessage(char *uname, char* fname, char *msg)
{
	lli offset = findUserOffset(uname);
	if (offset == -1){
		printf("\n\nUser not found in records..\n");
		return;
	}
	msgmetadata mmd;
	msgdata md;
	int bitoffset = getBit();
	_fseeki64(fp, (bitoffset * 128) + gmd.msgidx, SEEK_SET);
	strcpy(md.msg, msg);
	fwrite(&md, 128, 1, fp);
	_fseeki64(fp, (bitoffset * 128) + gmd.msgmetadataidx, SEEK_SET);
	mmd.useridx = offset;
	strcpy(mmd.filename, fname);
	mmd.msgidx = (bitoffset * 128) + gmd.msgidx;
	fwrite(&mmd, 128, 1, fp);
	gmd.bitvectorcnt++;
	writeMetaData();
}

void login(char *uname, char *pwd)
{
	fseek(fp, gmd.useridx, SEEK_SET);
	users u;
	for (int i = 0; i < gmd.usercnt; i++)
	{
		fread(&u, 128, 1, fp);
		if (strcmp(u.username, uname) == 0 && strcmp(u.password, pwd) == 0){
			printf("\n\nLogin Successful\n");
			return;
		}
	}
	printf("\n\nCredentials Invalid!\n");
	uname = NULL; pwd = NULL;
	return;
}

void readMessage(char *filename)
{
	bitvector bv;
	msgmetadata mmd;
	int flag = 0;
	int bvcount = gmd.bitvectorcnt; // no of msgs
	int i = 0;
	while (1)
	{
		fseek(fp, (i * 128) + gmd.bitvectoridx, SEEK_SET); //start address
		fread(&bv, 128, 1, fp);
		for (int j = 0; j < 128 && j < bvcount; j++)
		{
			int count = 0;
			if (bv.flag[j] == true)
			{
				fseek(fp, ((gmd.bitvectorcnt - bvcount + j) * 128) + gmd.msgmetadataidx, SEEK_SET);
				fread(&mmd, 128, 1, fp);
				if (strcmp(mmd.filename, filename) == 0)
				{
					flag = 1;
					msgdata md;
					fseek(fp, mmd.msgidx, SEEK_SET);
					fread(&md, 128, 1, fp);
					printf("%d ", (gmd.bitvectorcnt - bvcount + j) + count);
					printf("%s\n", md.msg);
				}
			}
			count++;
		}
		bvcount -= 128;
		if (bvcount <= 0)
		{
			if (flag == 0)
				printf("No Such File Exists");
			return;
		}
		i++;
	}
}

void deleteMessage(char *filename, char *username)
{
	int msg_id;
	bitvector bv;
	readMessage(filename);
	printf("Enter Message ID:");
	scanf("%d", &msg_id);
	int block_no = msg_id / 128;
	int seq = msg_id % 128;
	fseek(fp, gmd.bitvectoridx, SEEK_SET);
	while (block_no > 0)
	{
		fseek(fp, 128, SEEK_CUR);
	}
	fread(&bv, 128, 1, fp);
	bv.flag[seq] = false;
	fseek(fp, -128, SEEK_CUR);
	fwrite(&bv, 128, 1, fp);
}


// Blob Code
void enumFiles()
{
	blobbitvector bbv;
	users uv;
	fseek(fp, gmd.blobbitvectoridx, SEEK_SET);
	fread(&bbv, 128, 1, fp);
	int i;
	printf("\n\n");
	printf("%10s %10s %10s\n", "File_Name", "File_Size", "User_Name");
	for (i = 0; i < 128; i++){
		if (bbv.flag[i] == true){
			fseek(fp, (i * 128) + gmd.blobmetadataidx, SEEK_SET);
			blobmetadata bmd;
			fread(&bmd, 128, 1, fp);
			printf("%10s %10d ", bmd.filename, bmd.end - bmd.start);
			fseek(fp, bmd.useridx, SEEK_SET);
			fread(&uv, 128, 1, fp);
			printf("%10s\n", (char*)uv.username);
		}
	}
}

void writeBlobToFile(char *filename, char *username)
{
	FILE *blob_pointer;
	int count = 0;
	blob_pointer = fopen(filename, "rb");
	fseek(blob_pointer, 0, SEEK_END);
	lli file_size = ftell(blob_pointer);
	lli required_blocks = ceil((double)file_size / 128);
	printf("%lld\n", required_blocks);
	blobbitvector bbv;
	fseek(fp, gmd.blobbitvectoridx, SEEK_SET);
	fread(&bbv, 128, 1, fp);
	blobmetadata bmd;
	int start_index;
	for (int i = 0; i < 128; i++)
	{
		if (bbv.flag[i] == false)
		{
			bbv.flag[i] = true;
			break;
		}
	}
	fseek(fp, -128, SEEK_CUR);
	fwrite(&bbv, 128, 1, fp);
	int end_block = 0;
	fseek(fp, gmd.blobmetadataidx, SEEK_SET);
	blobmetadata bmd1;
	for (int i = 0; i < gmd.blobcnt; i++)
	{
		fread(&bmd1, 128, 1, fp);
		end_block += bmd1.blocksrequired;
	}
	bmd.start = ((end_block)* 128) + gmd.blobmsgidx;
	bmd.end = bmd.start + file_size;
	bmd.blocksrequired = required_blocks;
	strcpy(bmd.filename, filename);
	bmd.useridx = findUserOffset(username);
	fseek(fp, (gmd.blobcnt * 128) + gmd.blobmetadataidx, SEEK_SET);
	fwrite(&bmd, 128, 1, fp);
	gmd.blobcnt++;
	writeMetaData();
	fseek(fp, bmd.start, SEEK_SET);
	fseek(blob_pointer, 0, SEEK_SET);
	for (int i = 0; i < bmd.blocksrequired; i++)
	{
		blobdata bd;
		for (int j = 0; j < 128; j++)
		{
			if (!feof(blob_pointer))
				bd.data[j] = fgetc(blob_pointer);
			else
				break;
		}
		fwrite(&bd, 128, 1, fp);
	}
}

void downloadblob(char *filename)
{
	FILE *blob_pointer;
	char *dest_filename = (char*)malloc(sizeof(char) * 20);
	printf("Enter the Destination File Name:");
	fflush(stdin);
	gets(dest_filename);
	blob_pointer = fopen(dest_filename, "wb+");
	blobbitvector *bbv = (blobbitvector*)malloc(sizeof(blobbitvector));
	blobmetadata *bmd = (blobmetadata*)malloc(sizeof(blobmetadata));
	blobdata *bd = (blobdata*)malloc(sizeof(blobdata));
	fseek(fp, gmd.blobbitvectoridx, SEEK_SET);
	fread(bbv, 128, 1, fp);
	for (int i = 0; i < 128; i++)
	{
		if (bbv->flag[i] == true)
		{
			fseek(fp, (i * 128) + gmd.blobmetadataidx, SEEK_SET);
			fread(bmd, 128, 1, fp);
			if (strcmp(bmd->filename, filename) == 0)
			{
				fseek(fp, bmd->start, SEEK_SET);
				int blocks = bmd->blocksrequired;
				int chars_to_read = bmd->end - bmd->start;
				while (blocks > 0)
				{
					//printf("%d\n", chars_to_read);
					fread(bd, 128, 1, fp);
					for (int j = 0; j < 128 && j < chars_to_read; j++){
						fputc(bd->data[j], blob_pointer);
					}
					printf("%.2f %% completed\n", (ftell(blob_pointer) / (float)(bmd->blocksrequired * 128)) * 100);
					blocks--;
					chars_to_read = chars_to_read - 128;
				}
				fclose(blob_pointer);
				system(dest_filename);
				return;
			}
		}
	}
}

void deleteBlob(char *filename)
{
	blobbitvector bbv;
	int flag = 0;
	fseek(fp, gmd.blobbitvectoridx, SEEK_SET);
	fread(&bbv, 128, 1, fp);
	int i;
	for (i = 0; i < 128; i++){
		if (bbv.flag[i] == true){
			fseek(fp, (i * 128) + gmd.blobmetadataidx, SEEK_SET);
			blobmetadata bmd;
			fread(&bmd, 128, 1, fp);
			if (strcmp(bmd.filename, filename) == 0){
				// make flag 0 and write back to file
				bbv.flag[i] = false; break;
				flag = 1;
			}
		}
	}
	if (flag == 0){
		printf("No such File Found");
	}
}

int init_hints(struct addrinfo *hints, struct addrinfo **result, char* host, char port[])
{
	ZeroMemory(hints, sizeof(*hints));
	hints->ai_family = AF_INET;
	hints->ai_socktype = SOCK_STREAM;
	hints->ai_protocol = IPPROTO_TCP;
	hints->ai_flags = AI_PASSIVE;
	int iResult;
	// Resolve the server address and port
	iResult = getaddrinfo(host, port, hints, result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	return 0;
}

void server_initial_sync()
{
	rewind(fp);
	int i = 0;
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char *recvbuf = (char*)malloc(sizeof(char) * DEFAULT_BUFLEN);
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, serverport1, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}

	// No longer need server socket
	closesocket(ListenSocket);
	char *to_send = (char*)malloc(sizeof(char) * 1048576);
	rewind(fp);
	printf("\nStarted\n");
	printf("%d\n", ftell(fp));
	do {
		if (fread(to_send, 1048576, 1, fp))
		{
			iSendResult = send(ClientSocket, to_send, 1048576, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
			}
		}
		else{
			iSendResult = send(ClientSocket, to_send, 0, 0);
			break;
		}
	} while (1);
	printf("Success!\n");
	printf("%d\n", ftell(fp));
	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();
	return;
}

void client_initial_sync()
{
	rewind(fp);
	int i = 0;
	int iResult;
	init_hints(&hints1, &result1, NULL, port1);
	char *recvbuf = (char*)malloc(sizeof(char)*DEFAULT_BUFLEN);
	int recvbuflen = DEFAULT_BUFLEN;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return;
	}
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(ip_address, port2, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return;
	}

	char *sendbuf = (char*)malloc(sizeof(char) * 1048576);
	rewind(fp);
	printf("%d\n", ftell(fp));
	printf("\nBegin!\n");
	do {
		iResult = recv(ConnectSocket, sendbuf, 1048576, 0);
		if (iResult > 0){
			fwrite(sendbuf, 1048576, 1, fp);
			//printf("Bytes received: %d\n", iResult);
		}
		else if (iResult == 0)
			printf("Connection closed\n");
	} while (iResult > 0);
	printf("\nEnd!\n");
	printf("%d\n", ftell(fp));
	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();
	return;
}



void* sync_call(void *ptr)
{
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		_getch();
		return NULL;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, port1, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		_getch();
		return NULL;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		_getch();
		return NULL;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		_getch();
		return NULL;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		_getch();
		return NULL;
	}

	// Accept a client socket
	printf("\nWaiting for connection...\n");
	ClientSocket = accept(ListenSocket, NULL, NULL);
	printf("\nDone..\n");
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		_getch();
		return NULL;
	}
	printf("Accepted Invitation!\n");
	// No longer need server socket
	closesocket(ListenSocket);

	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		_getch();
		return NULL;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();
	server_initial_sync();
	_getch();
	return NULL;
}

int connect_to_service()
{
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	char *sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(ip_address, serverport, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();
	return 0;
}