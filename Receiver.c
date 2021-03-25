#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define BUFF_SIZE 1501
#define stdin 0

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <WinSock2.h>
#include <math.h>
#include <stdbool.h>
#include <conio.h>
#include "C:\Users\user1\source\repos\Noisy Channel\Noisy Channel\hamming.c" 

int decode(char* buffer, int blen , FILE* f)
{
	/*
		Receives encoded (char*) buffer and a (FILE*) f, and writes to the file the decoded data from the buffer
	*/
	int errcount = 0;
	char decoded[12] = { 0 };
	int a = strlen(buffer);
	while (blen > 0) {
		errcount += Decoder(decoded, buffer);
		//strncat(dec_buffer, decoded, 12);
		fwrite(decoded, 11, 1, f);
		blen -= 15;
		buffer += 15;
	}
	return errcount;
}

int main(int argc, char* argv[])
{
	/*
		Arguments:
		argv[1] - local port
		argv[2] - file name
	*/

	//Init winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
		printf("Error at WSAStartup()\n");

	//Create and init socket
	SOCKET channel_s;
	if ((channel_s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");

	//Filling channel information
	struct sockaddr_in channel_addr;
	channel_addr.sin_family = AF_INET;
	channel_addr.sin_port = htons(atoi(argv[1]));
	channel_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int channel_addrss_len = sizeof(channel_addr);

	//Bind
	if (bind(channel_s, (struct sockaddr*)&channel_addr, sizeof(channel_addr)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done");

	// Setup timeval variable
	struct timeval timeout;
	struct fd_set fds;

	//open output file 
	FILE* f;
	f = fopen(argv[2], "wb");
	if (f == NULL)
	{
		printf("File error. Coudn't open file\n");// CHECKING IF OPENED SUCCSESSFULLY
		return -1;
	}
	printf("File opened successfully\n");

	int  retval, buffer_len;
	unsigned char dec_buff[1100] = { 0 }, buffer[BUFF_SIZE] = { 0 };
	char end_str[] = "end\n", str[10] = { 0 };

	while (1)
	{

		timeout.tv_sec = 3;
		timeout.tv_usec = 0;

		//set descriptors
		FD_ZERO(&fds);
		FD_SET(channel_s, &fds);
		
		//wait for data to be sent by the channel
		retval = select(max(channel_s, stdin) + 1, &fds, NULL, NULL, &timeout);

		if (retval < 0)
		{//error occured

			// check errno/WSAGetLastError(), call perror(), etc ...
		}

		if (retval > 0)
		{
			if (FD_ISSET(channel_s, &fds))
			{//data was received from the channel
			//we will process it (decode) and write it to the output file

				printf("Sender requesting to connect...\n");

				//Receive data from the channel
				buffer_len = recvfrom(channel_s, buffer, BUFF_SIZE, 0, (struct sockaddr*)&channel_addr, &channel_addrss_len);

				for (int i = buffer_len; i < BUFF_SIZE-1; i++)
				{
					buffer[i] = '\0';
				}
				//decode message and write it to the output file
				printf("%d\n" ,decode(buffer, buffer_len, f));
			}
		}

		if (_kbhit())
		{//check if 'end' was pressed to stdin. if so, send Ack to the channel, close the connection, and close the output file.

			/*strcat(str, gets());
			if (strcmp(str, end_str))
			{
				printf("end\n");
				break;
			}*/
			
			//send Ack to the channel
			strcpy(buffer, "ack");
			if (sendto(channel_s, buffer, BUFF_SIZE, 0, (struct sockaddr *) &channel_addr, sizeof(channel_addr)) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d", WSAGetLastError());
				exit(EXIT_FAILURE);
			}

			//=== present the data before finishing execution =======


			//close connection with the channel
			closesocket(channel_s);
			//close the output file
			fclose(f);

			//finish execution
			break;
		}

		if (retval == 0)
		{//timeout occured
			continue;
		}
	}
	return 0;
}
