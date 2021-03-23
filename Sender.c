#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define BUFF_SIZE 1501

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <WinSock2.h>
#include <math.h>
#include <stdbool.h>
#include "C:\Users\user1\source\repos\Noisy Channel\Noisy Channel\hamming.c" 


int main(int argc, char* argv[])
{
//read arguments
	char IP[20], port[40], f_name[50];
	strcpy(IP, argv[1]);
	strcpy(port, argv[2]);
	strcpy(f_name, argv[3]);

	FILE* f = NULL;
	f = fopen(f_name, "rb");
	if (f == NULL)
	{
		printf("File error. Coudn't open file\n");// CHECKING IF OPENED SUCCSESSFULLY
		return -1;
	}
	printf("File opened successfully\n");

//init winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
		printf("Error at WSAStartup()\n");

//create socket
	SOCKET client_s;

	if ((client_s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	printf("Socket created.\n");

// Filling server information 
	struct sockaddr_in client_addr;
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(atoi(port));
	client_addr.sin_addr.s_addr = inet_addr(IP);//INADDR_ANY;

//connect to the channel
	//int status = connect(client_s, (SOCKADDR*)&client_addr, sizeof(client_addr));


	char chr[12] = { 0 }, coded_chr[16] = { 0 }, buffer[BUFF_SIZE] = { 0 };
	int pckt_size = 0;
	//buffer = calloc(BUFF_SIZE, sizeof(char*));
	if (buffer == NULL)
	{
		printf("Memory allocation failed\n");
	}

	int i = 0;
	while (fread(chr, 1, 11, f))
	{
		Encoder(chr, coded_chr);
		strncpy(buffer + pckt_size, coded_chr, 15);
		pckt_size += 15;
		//printf("%d\n", strlen(buffer));
		if (pckt_size == BUFF_SIZE-1)
		{
			printf("sending packet num %d\n", i);
			i++;
			//send data
			if (sendto(client_s, buffer, strlen(buffer), 0, (struct sockaddr *) &client_addr, sizeof(client_addr)) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d", WSAGetLastError());
				exit(EXIT_FAILURE);
			}

			for (int i = 0; i < BUFF_SIZE; i++)
			{
				buffer[i] = 0;
			}
			pckt_size = 0;
		}
		//printf("%d %s\n", (char*)coded_chr, buffer);
		
	}
	if (pckt_size != BUFF_SIZE-1)
	{
		buffer[pckt_size] = '\0';
		//send data
		if (sendto(client_s, buffer, strlen(buffer), 0, (struct sockaddr *) &client_addr, sizeof(client_addr)) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		printf("sending packet num %d\n", i);
	}
	closesocket(client_s);
	fclose(f);

	return 0;
}
