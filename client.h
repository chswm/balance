#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <event.h>
#include <assert.h>
#include <iostream>
using namespace std;

class cli
{
public:
	cli(char *ip="192.168.1.108",short port= 7000);
	void RealDo();
private:
	int LBfd;
};

void cli::RealDo()
{
	char buff[128] = {0};
	while (true)
	{
		cout<<"input:";
		cin>>buff;
		send(LBfd,buff,strlen(buff),0);
		memset(buff,0,128*sizeof(char));
		recv(LBfd,buff,127,0);
		cout<<"re:"<<buff<<endl;
	}
}

cli::cli(char *ip,short port)
{
	LBfd = socket(AF_INET,SOCK_STREAM,0);
	assert(LBfd != -1);
	struct sockaddr_in saddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(ip);

	int res = connect(LBfd,(struct sockaddr*)&saddr,sizeof(saddr));
	assert(res != -1);
}
