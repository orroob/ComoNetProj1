#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define BUFF_SIZE 1502

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <WinSock2.h>
#include <math.h>
#include <stdbool.h>
#include "C:\Users\user1\source\repos\Noisy Channel\Noisy Channel\hamming.c" 


int main(int argc, char* argv[])
{
/*
	Arguments:
	argv[1] - channel IP
	argv[2] - channel port
	argv[3] - input file name
*/

	FILE* f = NULL;
	f = fopen(argv[3], "rb");
	if (f == NULL)
	{
		printf("File error. Coudn't open file\n");// CHECKING IF OPENED SUCCSESSFULLY
		return -1;
	}
	printf("File opened successfully\n");

	// Init winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
		printf("Error at WSAStartup()\n");

	// Create and init socket
	SOCKET channel_s;
	if ((channel_s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");

	// Filling channel information 
	struct sockaddr_in client_addr;
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(atoi(argv[2]));
	client_addr.sin_addr.s_addr = inet_addr(argv[1]);
	int client_addr_len = sizeof(client_addr);

	// Setup timeval variable
	struct timeval timeout;
	struct fd_set fds;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	char chr[12] = { 0 }, coded_chr[16] = { 0 }, buffer[BUFF_SIZE] = { 0 };
	int  retval, pckt_size = 0, buffer_len;

	while (fread(chr, 1, 11, f))
	{
/*
		// Check if Ack has been sent from the channel. if so, close the connection, the file and finish execution:
		//set descriptors
		FD_ZERO(&fds);
		FD_SET(channel_s, &fds);

		//wait for data to be sent by the channel
		retval = select(channel_s + 1, &fds, NULL, NULL, &timeout);
		if (retval < 0)
		{//error occured

			// check errno/WSAGetLastError(), call perror(), etc ...
		}

		if (retval == 0)
		{//timeout occured
			continue;
		}

		if (retval > 0)
		{
			if (FD_ISSET(channel_s, &fds))
			{//data was received from the channel
			//we will process it (decode) and write it to the output file

				printf("Sender requesting to connect...\n");

				//Receive data from the channel
				buffer_len = recvfrom(channel_s, buffer, BUFF_SIZE, 0, (struct sockaddr*)&client_addr, &client_addr_len);
				//======(we receive the message from the address to which we sent our data to - check for errors)=======================

				//===present the data from Ack before finishing execution =======

				//close connection with the channel
				closesocket(channel_s);

				//close intput file
				fclose(f);
			}
		}
*/
		//encode message and insert it to the packet
		Encoder(chr, coded_chr);
		strncpy(buffer + pckt_size, coded_chr, 15);
		pckt_size += 15;

		if (pckt_size == BUFF_SIZE-2)
		{//packet is ready to be sent


			//send data to channel
			if (sendto(channel_s, buffer, BUFF_SIZE, 0, (struct sockaddr *) &client_addr, sizeof(client_addr)) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d", WSAGetLastError());
				exit(EXIT_FAILURE);
			}

			//clean buffer
			for (int i = 0; i < BUFF_SIZE; i++)
			{
				buffer[i] = 0;
			}
			pckt_size = 0;
		}
	}

	if (pckt_size != BUFF_SIZE-1)
	{
		while (pckt_size!= BUFF_SIZE - 1)
		{
			buffer[pckt_size] = '\0';
			pckt_size++;
		}
		//send data
		if (sendto(channel_s, buffer, strlen(buffer), 0, (struct sockaddr *) &client_addr, sizeof(client_addr)) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}
