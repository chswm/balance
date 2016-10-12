#ifndef _SER_H
#define _SER_H



#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<assert.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<semaphore.h>
void * fun(void *arg)
{
        int c =(int ) arg;
        char buff[128] = {0};
        while(1)
        {
            int n =    recv(c,buff,127,0);
            if ( n == 0)
            {
                break;
            }
            printf("recv %s\n",buff);
            send(c,"ok",2,0);
            memset(buff,0,128);
        }
        close(c);
}



 



#endif




