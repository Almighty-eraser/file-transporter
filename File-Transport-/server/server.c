#pragma comment(lib,"ws2_32.lib")
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <WinSock2.h>
#define BUF_SIZE 1024
void ErrorHandling(char* message);

int main()
{
	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAddr, clntAddr;
	char port[10];

	printf("port : ");
	scanf_s("%s", port, sizeof(port));


	int szClntAddr;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	if (hServSock == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	port[sizeof(port) - 1] = '\0';
	servAddr.sin_port = htons(atoi(port));

	if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("bind() error");

	if (listen(hServSock, 5) == SOCKET_ERROR)
		ErrorHandling("listen() error");

	szClntAddr = sizeof(clntAddr);
	hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);
	if (hClntSock == INVALID_SOCKET)
		ErrorHandling("accept() error");
	else
		printf("\nConnected\n");

	char filename[20];

	memset(filename, 0, sizeof(filename));
	puts("\n\nname of file : ");
	scanf_s("%s", filename, sizeof(filename));

	FILE* fp = NULL;
	fopen_s(&fp, filename, "rb");
	if (fp == NULL)
		ErrorHandling("failed to open file");

	send(hClntSock, filename, sizeof(filename), 0);

	fseek(fp, 0, SEEK_END);
	long size_of_file = ftell(fp);

	send(hClntSock, &size_of_file, sizeof(size_of_file), 0);

	char* content = malloc(size_of_file);
	char buffer[BUF_SIZE];
	long count = 0;
	long i = 0;

	for (long j = 0; j < size_of_file; j++)
		content[j] = 0;
	puts("\n\nReading file\n\n");
	puts("\n\nSending file to Client...\n\n");
	fseek(fp, 0, SEEK_SET);
	while (i < size_of_file)
	{
		memset(buffer, 0, sizeof(buffer));
		count = fread(buffer, sizeof(char), sizeof(buffer), fp);
		if (send(hClntSock, buffer, count, 0) == SOCKET_ERROR)
			ErrorHandling("send()[buffer] error");
		i += count;
		printf("\n%d%% %dbytes [%dbytes]", i * 100 / size_of_file, i, size_of_file);
	}



	puts("\n\nFile sent successfully\n\n");

	closesocket(hClntSock);
	closesocket(hServSock);
	WSACleanup();
	fclose(fp);
	free(content);

	printf("\nPress any key to exit\n");
	i = _getch();

	return 0;
}

void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	printf("\nPress any key to exit\n");
	int a = _getch();
	exit(1);
}