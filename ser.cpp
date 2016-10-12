#include <iostream>
//#include"ser.h"
using namespace std;


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
            int n =recv(c,buff,127,0);
            cout<<"n:"<<n<<endl;
            if(n == -1)break;
            if ( n == 0)
            {
                continue;
            }
            printf("recv %s\n",buff);
            send(c,"ok",2,0);
            memset(buff,0,128);
        }
        close(c);
}


int main()
{
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    assert( sockfd != -1);
    struct sockaddr_in saddr,caddr;
    memset(&saddr,0,sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(6500);
    saddr.sin_addr.s_addr = inet_addr("192.168.1.108");
    int res = bind(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
    assert( res != -1);
    listen(sockfd,5);
    while(1)
    {
        socklen_t len = sizeof(caddr);
        int c = accept(sockfd,(struct sockaddr*)&caddr,&len);
        if( c < 0 )
        {
            perror("accept error\n");
            continue;
        }
        pthread_t id;
        int err = pthread_create(&id,NULL,fun,(void*)c);

     	if(err == -1)
     	{
	     	cout<<"phread error"<<endl;
     	}
    }
}

