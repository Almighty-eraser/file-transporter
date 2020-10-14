#pragma comment(lib,"ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <WinSock2.h>
void ErrorHandling(char* message);

int main()
{
	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN servAddr;

	int strLen = 0;
	char ip[20];
	char port[10];
	memset(ip, 0, sizeof(ip));
	memset(port, 0, sizeof(port));
	printf("ip : ");
	scanf_s("%s", ip, sizeof(ip));
	printf("port : ");
	scanf_s("%s", port, sizeof(port));

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error");

	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(ip);
	servAddr.sin_port = htons(atoi(port));

	if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("connect() error");
	else
		printf("\nConnected\n");

	char filename[20];
	long size_of_file = 0;
	long len = 0;

	while (len < sizeof(filename))
		len += recv(hSocket, filename, sizeof(filename), 0);

	puts("\n\nReceived filename..\n\n");

	Sleep(10);
	FILE* fp = NULL;
	fopen_s(&fp, filename, "wb");
	if (fp == NULL)
		ErrorHandling("failed to open file");

	len = 0;
	while (len < sizeof(size_of_file))
		len += recv(hSocket, &size_of_file, sizeof(size_of_file), 0);

	puts("\n\nReceived size of file..\n\n");

	puts("\n\nWaiting..\n\n");

	len = 0;
	int i = 0;
	char* content = malloc(size_of_file);
	for (long j = 0; j < size_of_file; j++)
		content[j] = 0;

	puts("\n\nDownloading File from Server...\n\n");

	while (len < size_of_file)
	{
		len += recv(hSocket, content, size_of_file, 0);
		if (len == SOCKET_ERROR && i >= 100)
		{
			puts("\n\nrecv()[content] error\n\nCANNOT RECEIVE FILE");
		}
		if (len == SOCKET_ERROR)
		{
			len = 0;
			puts("\n\nrecv()[content] error\n\nWaiting for server...\n\n");
			i++;
			Sleep(2000);
			continue;
		}
		printf("\n%d%%", len * 100 / size_of_file);
	}

	puts("\n\nDownloaded File from Server successfully\n\nCreating File...");

	fseek(fp, 0, SEEK_SET);
	len = 0;
	while (len < size_of_file)
	{
		len += fwrite(content, sizeof(char), size_of_file, fp);
		printf("\n%d%%", len * 100 / size_of_file);
	}

	puts("\n\nFile Created successfully\n\n");


	closesocket(hSocket);
	WSACleanup();
	fclose(fp);
	free(content);
	printf("\nPress any key to exit\n");
	i = _getch();

	return 0;
}//

void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	printf("\nPress any key to exit\n");
	int a = _getch();
	exit(1);
}