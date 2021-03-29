#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define BUFF_SIZE 1500

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <WinSock2.h>
#include <math.h>
#include <stdbool.h>

int Noise(char* buffer, int seed, double probability)
{
	/*
		Input: (char*) buffer, (int) seed, (double) probability
		Functionality: "flips" every buffer's bit at the given probabilty.
		Return value: number of flipped bits.
	*/

	int MAX_P = 1 / probability;
	int random, mask = 1, flipped = 0;
	srand(seed);

	for (int i = 0; i < 1500; i++)
	{
		mask = 1;
		for (int j = 0; j < 8; j++)
		{
			mask *= 2;
			random = rand();
			if (pow(2, 16) == MAX_P)
			{
				if (rand() % 2 == 0)
				{
					continue;
				}
			}
			if ((random % MAX_P) == 0)
			{
				buffer[i] = buffer[i] ^ mask;
				flipped++;
			}
		}

	}
	return flipped;
}

void checkArgs(int argc, char* argv[])
{
	if (argc < 5)
	{
		printf("too few arguments\n");
		exit(1);
	}

	if (atoi(argv[1]) == 0 || atoi(argv[3]) == 0)
	{
		printf("Invalid port\n");
		exit(1);
	}
}

int main(int argc, char* argv[])
{
	/*
		Arguments:
		argv[1] - local port
		argv[2] - server's IP
		argv[3] - server's port
		argv[4] - probability
		argv[5] - random seed
	*/

	//Check if the arguments are valid
	checkArgs(argc, argv);

	unsigned char buffer[BUFF_SIZE + 2] = { 0 };
	int buffer_len;
	double probability = atoi(argv[4]) / (pow(2, 16));

	//init winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
		printf("Error at WSAStartup()\n");

	//create sockets
	SOCKET client_s, server_s;

	//init sockets
	if ((server_s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	if ((client_s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	// Filling client information 
	struct sockaddr_in client_addr, server_addr;
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(atoi(argv[1]));
	client_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// Filling server information 
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[3]));
	server_addr.sin_addr.s_addr = inet_addr(argv[2]);

	//Bind
	if (bind(client_s, (struct sockaddr*)&client_addr, sizeof(client_addr)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	// Setup timeval variable
	struct timeval timeout;
	struct fd_set fds;
	int  retval, client_addrss_len = sizeof(client_addr), server_addr_len = sizeof(server_addr), flipped = 0;
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;

	char output[200];
	int size = 0;

	while (1)
	{
		//set descriptors
		FD_ZERO(&fds);
		FD_SET(client_s, &fds);
		FD_SET(server_s, &fds);

		//wait for data to be sent by either the client or the server
		retval = select(max(client_s, server_s) + 1, &fds, NULL, NULL, &timeout);

		if (retval < 0)
		{//error occured

			// check errno/WSAGetLastError()
			printf("error occured");
			exit(1);
		}

		if (retval == 0)
		{//timeout occured
			continue;
		}

		if (retval > 0)
		{

			if (FD_ISSET(client_s, &fds))
			{//Data was recived from the client
			//we will process it (add noise to it) and send it to the server

				//receive data from client
				buffer_len = recvfrom(client_s, buffer, BUFF_SIZE + 2, 0, (struct sockaddr*)&client_addr, &client_addrss_len);
				size += buffer_len;

				for (int i = buffer_len; i < BUFF_SIZE; i++)
				{
					buffer[i] = '\0';
				}
				//add noise to the data before sending it to the server

				flipped += Noise(buffer, argv[5], probability);

				//Sending the processed data to the server
				if (sendto(server_s, buffer, buffer_len, 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
				{
					printf("sendto() failed with error code : %d", WSAGetLastError());
					exit(EXIT_FAILURE);
				}
			}

			if (FD_ISSET(server_s, &fds))
			{//Ack received from the server.
			//we will send Ack to the client, and close the connections.

				//receive data from server
				buffer_len = recvfrom(server_s, buffer, BUFF_SIZE, 0, (struct sockaddr*)&server_addr, &server_addr_len);

				//close connection with the server
				closesocket(server_s);

				//send Ack to the client
				if (sendto(client_s, buffer, buffer_len, 0, (struct sockaddr*)&client_addr, sizeof(client_addr)) == SOCKET_ERROR)
				{
					printf("sendto() failed with error code : %d", WSAGetLastError());
					exit(EXIT_FAILURE);
				}

				//present the data before finishing execution
				sprintf(output, "sender: %s \nreceiver: %s \n%d bytes, flipped %d bits\n", inet_ntoa(client_addr.sin_addr), argv[2], size, flipped);
				write(2, output, strlen(output));

				//close connection with the client
				closesocket(client_s);

				//finish execution
				break;
			}
		}
	}

	return 0;
}
