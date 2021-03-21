#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <WinSock2.h>
#include <math.h>
#include <stdbool.h>

/*
void die(char* s)
{
    perror(s);
    exit(1);
}

int main(void)
{
    struct sockaddr_in si_me, si_other;

    int s, i, j, slen = sizeof(si_other), recv_len;
    char buf[BUFLEN];

    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    memset((char*)&si_me, '1', sizeof(si_me));
    //printf("%d",si_me);

    si_me.sin_family = AF_INET;
    si_me.sin_port = PORT;
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(s, (struct sockaddr*)&si_me, sizeof(si_me)) == -1)
    {
        die("bind");
    }
    //memset(buf,0,503);
    char fname[20];
    FILE* fp;
    recv_len = recvfrom(s, buf, 20, 0, (struct sockaddr*)&si_other, &slen);

    char fna[100];
    memset(buf, 0, 503);

    recv_len = recvfrom(s, buf, 20, 0, (struct sockaddr*)&si_other, &slen);

    strcpy(fna, buf);
    //printf("%c\n",fna);
    int len = strlen(fna);
    printf("%d", len);

    unsigned long mm = atoi(buf);
    //printf("mm value: %ld\n",mm);

    fp = fopen(fna, "wb");
    int itr = 1;
    memset(buf, 0, 503);
    while (itr * 503 < mm)
    {
        if ((recv_len = recvfrom(s, buf, 503, 0, (struct sockaddr*)&si_other, &slen)) == -1)
        {
            die("recvfrom()");
        }
        fwrite(buf, 503, 1, fp);
        memset(buf, 0, 503);
        //printf("Loop no: %d",i)
        //for(i=0;i<=itr;i++);

        itr++;
    }

    //printf("Loop no: %d\n",i);
    printf("%d", (mm % 503));
    recv_len = recvfrom(s, buf, (mm % 503), 0, (struct sockaddr*)&si_other, &slen);

    fwrite(buf, (mm % 503), 1, fp);
    memset(buf, 0, 503);
    fclose(fp);
    close(s);

    return 0;
}
*/


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
            data[i * 8 + j + skip] = uncoded[i] / pow(2, 7 - j);
            data[i * 8 + j + skip] = data[i * 8 + j + skip] % 2;
        }
    }
    
    for (i = 0; i < 8; i++) {
        data[i*15] = data[i*15 +2] ^ data[i*15 +4] ^ data[i*15 +6] ^ data[i*15 + 8] ^ data[i * 15 + 10] ^ data[i * 15 + 12] ^ data[i * 15 + 14];
        data[(i*15) + 1] = data[i * 15 + 2] ^ data[i * 15 + 5] ^ data[i * 15 + 6] ^ data[i * 15 + 9] ^ data[i * 15 + 10] ^ data[i * 15 + 13] ^ data[i * 15 + 14];
        data[(i * 15) + 3] = data[i * 15 + 4] ^ data[i * 15 + 5] ^ data[i * 15 + 6] ^ data[i * 15 + 11] ^ data[i * 15 + 12] ^ data[i * 15 + 13] ^ data[i * 15 + 14];
        data[(i * 15) + 7] = data[i * 15 + 8] ^ data[i * 15 + 9] ^ data[i * 15 + 10] ^ data[i * 15 + 11] ^ data[i * 15 + 12] ^ data[i * 15 + 13] ^ data[i * 15 + 14];
    }
    
    
    for (i = 0; i < 15; i++) {//coded bits into 15 char string
        for (j = 0; j < 8; j++) {

            coded[i] |= (data[j] == '1') << (7 - j);

        }
    }
}

void Decoder(char* uncoded, char* coded) {
    int filter = 128, i , j, k, k1, k2, k3, k4;
    char bits[120], data_bits[88];

    for (int i = 0; i < 8; i++) {//extracting 15bits per tour (bit number i of each byte)
        for (int j = 0; j < 15; j++) bits[i * 15 + j] = ((coded[j] & filter) == 0) ? 0 : 1;
        filter /= 2;
    }
    for(int i=0;i<8;i++){// checking mistake
        do{
            k1=bits[i*15]^bits[i*15+2]^bits[i*15+4]^bits[i*15+6]^bits[i*15+8]^bits[i*15+10]^bits[i*15+12]^bits[i*15+14];
            k2=bits[i*15+1]^bits[i*15+2]^bits[i*15+5]^bits[i*15+6]^bits[i*15+9]^bits[i*15+10]^bits[i*15+13]^bits[i*15+14];
            k3=bits[i*15+3]^bits[i*15+4]^bits[i*15+5]^bits[i*15+6]^bits[i*15+11]^bits[i*15+12]^bits[i*15+13]^bits[i*15+14];
            k4=bits[i*15+7]^bits[i*15+8]^bits[i*15+9]^bits[i*15+10]^bits[i*15+11]^bits[i*15+12]^bits[i*15+13]^bits[i*15+14];
            k=k1+2*k2+4*k3+8*k4;
            
            if(k!=0) bits[i*15+k-1]=bits[i*15+k-1]^1;
            
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
            uncoded[i] |= data_bits[j * 11 + i] * filter;
            filter /= 2;
        }
        filter = 128;
    }

}
    




int main(void)
{
    char eight_uncoded_words[12] = "8ka/5a6mbda", eight_coded_words[16];
    Encoder (eight_uncoded_words, eight_coded_words);
    //for (int i = 0; i < 15; i++)
    //    printf("%d\n", (int)eight_coded_words[i]);
    //printf("%s\n", eight_coded_words);
    Decoder(eight_uncoded_words, eight_coded_words);
    //printf("%s\n", eight_uncoded_words);
    return 0;
}

