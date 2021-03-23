#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define BUFF_SIZE 1500
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <WinSock2.h>
#include <math.h>
#include <stdbool.h>
#include "C:\Users\datne\OneDrive\Desktop\CCProrojet\Project1\hamming.c"

/*
void decode(char* buffer, char* dec_buffer){
	int blen = strlen(buffer);
	char decoded[16] = { 0 };

	while (blen > 0) {
		Decoder(decoded, buffer);
		strncat(dec_buffer, decoded, 12);
		blen -= 15;
		buffer += 15;
	}
}
*/

int main(int argc, char* argv[]){
	char Lport[10] , R_ip[20], R_port[20], P[10], RS[10], buffer[1500] = { 0 };
	int buffer_len ;
	strcpy(Lport, argv[1]);
	strcpy(R_ip, argv[2]);
	strcpy(R_port, argv[3]);
	strcpy(P, argv[4]);
	strcpy(RS, argv[5]);
	


	//init winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
		printf("Error at WSAStartup()\n");

	//create socket
	SOCKET client_s, server_s;


	if ((server_s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	printf("Socket created.\n");

	if ((client_s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	printf("Socket created.\n");

	// Filling server information 
	struct sockaddr_in client_addr, server_addr;
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(atoi(Lport));
	client_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//Bind
	if (bind(client_s, (struct sockaddr*)&client_addr, sizeof(client_addr)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done");

	// Setup timeval variable
	struct timeval timeout;
	struct fd_set fds;
	int  retval, client_addrss_len = sizeof(server_addr), i = 0 ;
	char dec_buff[BUFF_SIZE] = { 0 };

	while (1)
	{
		FD_ZERO(&fds);
		FD_SET(client_s, &fds);
		FD_SET(server_s, &fds);
		// add connected TCP clients, if needed...

		timeout.tv_sec = 10;
		timeout.tv_usec = 0;

		retval = select(max(client_s,server_s) + 1, &fds, NULL, NULL, &timeout);
		if (retval > 0)
		{
			if (FD_ISSET(client_s, &fds))
			{//Reciving the message from the sender
				printf("Sender requesting to connect...\n");
				
				// call accept() and do something with the client socket ...
				buffer_len = recvfrom(client_s, buffer, BUFF_SIZE, 0, (struct sockaddr*)&client_addr, &client_addrss_len);
				//decode(buffer, dec_buff);
				printf("%s\n", dec_buff);
				printf("%d\n", i);
				i++;
			}

			/*if (FD_ISSET(server_s, &fds))
			{
				printf("Reciver query incoming...\n");
				// call recv()/read() to read the datagram ...
			}
			*/

			// check connected TCP clients, if needed...
		}
		else if (retval < 0)
		{
			// check errno/WSAGetLastError(), call perror(), etc ...
		}
	}

	




}