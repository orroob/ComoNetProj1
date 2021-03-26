#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <WinSock2.h>
#include <math.h>
#include <stdbool.h>



void Encoder(char* uncoded, char* coded) {
    char data[120] = { 0 };
    int i, j, skip = 0;

    for (i = 0 ; i < 120 ; i++) {//inserting -1 for all the places that are used for parity bits
        if (i % 15 == 0 || i % 15 == 1 || i % 15 == 3 || i % 15 == 7) {
            data[i] = -1;
        }
    }
    
    for (i = 0; i < 11; i++) {//coding the word into bits format
        for (j = 0; j < 8; j++) {
            if (data[i * 8 + j + skip] == -1) skip++;            
            if (data[i * 8 + j + skip] == -1) skip++;
            data[i * 8 + j + skip] = abs((char)((int)(uncoded_word[i]>>(7-j)))%2);
           
        }
    }
    
    for (i = 0; i < 8; i++) {//
        data[i*15] = data[i*15 +2] ^ data[i*15 +4] ^ data[i*15 +6] ^ data[i*15 + 8] ^ data[i * 15 + 10] ^ data[i * 15 + 12] ^ data[i * 15 + 14];
        data[(i*15) + 1] = data[i * 15 + 2] ^ data[i * 15 + 5] ^ data[i * 15 + 6] ^ data[i * 15 + 9] ^ data[i * 15 + 10] ^ data[i * 15 + 13] ^ data[i * 15 + 14];
        data[(i * 15) + 3] = data[i * 15 + 4] ^ data[i * 15 + 5] ^ data[i * 15 + 6] ^ data[i * 15 + 11] ^ data[i * 15 + 12] ^ data[i * 15 + 13] ^ data[i * 15 + 14];
        data[(i * 15) + 7] = data[i * 15 + 8] ^ data[i * 15 + 9] ^ data[i * 15 + 10] ^ data[i * 15 + 11] ^ data[i * 15 + 12] ^ data[i * 15 + 13] ^ data[i * 15 + 14];
    }
    
    for (i = 0; i < 15; i++) {//turn coded bits into 15 char string
        coded[i] = 0;
        for (j = 0; j < 8; j++) {
            coded[i] = coded[i] * 2;
            coded[i] += data[i * 8 + j];
        }

    }

    
}



int Decoder(char* uncoded, char* coded) {
    int filter = 128, i , j, k, k1, k2, k3, k4, err_count = 0;
    char bits[120], data_bits[88];

    for (int i = 0; i < 15; i++) {//extracting 15bits per tour (bit number i of each byte)
	if (coded[i] < 0) coded[i] = (int)coded[i] + 256;
        filter = 128;
        for (int j = 0; j < 8; j++) {
            bits[i * 8 + j] = ((coded[i] & filter) == 0) ? 0 : 1;
            filter /= 2;
        }
    }

    for(int i=0;i<8;i++){// checking mistakes
        do{
            k1=bits[i*15]^bits[i*15+2]^bits[i*15+4]^bits[i*15+6]^bits[i*15+8]^bits[i*15+10]^bits[i*15+12]^bits[i*15+14];
            k2=bits[i*15+1]^bits[i*15+2]^bits[i*15+5]^bits[i*15+6]^bits[i*15+9]^bits[i*15+10]^bits[i*15+13]^bits[i*15+14];
            k3=bits[i*15+3]^bits[i*15+4]^bits[i*15+5]^bits[i*15+6]^bits[i*15+11]^bits[i*15+12]^bits[i*15+13]^bits[i*15+14];
            k4=bits[i*15+7]^bits[i*15+8]^bits[i*15+9]^bits[i*15+10]^bits[i*15+11]^bits[i*15+12]^bits[i*15+13]^bits[i*15+14];
            k=k1+2*k2+4*k3+8*k4;
            
			if (k != 0)
			{
				bits[i * 15 + k - 1] = bits[i * 15 + k - 1] ^ 1;
				err_count++;
			}
            
        }while(k!=0);
    }
    
    filter = 0;
    for (int i = 0; i < 8; i++) {//extracting only data_bits
        for (int j = 0; j < 15; j++) {
            if (j > 1 && j != 3 && j != 7) {
                data_bits[filter] = bits[i * 15 + j];
                filter++;
            }
        }
    }

    for (int i = 0; i < 11; i++) uncoded[i] = 0;

    filter = 128;

    for (int i = 0; i < 11; i++) {// writting dataout
        for (int j = 0; j < 8; j++) {
            uncoded[i] |= abs(data_bits[i * 8 + j] * filter);
            filter >> 1;
        }
        filter = 128;
    }
	return err_count;
}
    
