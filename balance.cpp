#include <iostream>
#include<queue>
#include<map>
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
#include<sys/epoll.h>
#include<fcntl.h>


const int MAXFD = 10;


class Balance
{
public:
	Balance(char* host, unsigned short port);// 监视客户连接
	void *Wakeup();
	void ConSer(char* host, unsigned short port);//连接服务器
	
	static void* Readcli(void *arg);
	void Readser();
	void Writeser();
	
	void wait();
	void add_fd(int fd);
	void del_fd(int fd);
	void setnonblock(int fd);
	
private:
	struct mArgu{ Balance* mb; int fd;};
	queue<int> ser_fd;
	map<int,int> mcli;
	int epfd;
	int clifd;
	struct epoll_event fds[MAXFD];
};

Balance::Balance(char* host, unsigned short port)
{
	epfd = epoll_create(MAXFD);
	
	clifd = socket(AF_INET,SOCK_STREAM,0);
    assert( clifd != -1);
    
	struct sockaddr_in saddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr("192.168.1.108");

    int res = bind(clifd,(struct sockaddr*)&saddr,sizeof(saddr));
    assert( res != -1);

    if (listen(clifd, 5) == -1)
	{
		cout << "listen error" << endl;
		exit(0);
	}
	add_fd(clifd);
}

void *Balance::Wakeup()
{
	struct sockaddr_in caddr;
	socklen_t len = sizeof(caddr);
	
	int cli_c = accept(clifd,(struct sockaddr*)&caddr,&len);
	if(cli_c < 0)
	{
		cout<<"accept error";
		return NULL;
	}
	add_fd(cli_c);	
	//cout<<;
}


void Balance::wait()
{
	while(true)
	{
		int n = epoll_wait(epfd, fds, MAXFD, 5000);
		if (n == -1)
		{
			cout << "epoll wait error" << endl;
			continue;
		}
		else if (n == 0)
		{
			continue;
		}
		else
		{
			for(int i = 0 ;i<n;++i)
			{
				if(fds[i].events & EPOLLIN)
				{
					if(fds[i].data.fd == clifd)
					{
						//pthread_t tid;
						//pthread_create(&tid,NULL,Wakeup, (void*)this);
						Wakeup();
					}
					else 
					{
						struct mArgu arg;
						arg.mb = this;
						arg.fd = fds[i].data.fd;
						//Readcli(fds[i].data.fd);
						pthread_t tid;
						pthread_create(&tid,NULL,Readcli, (void*)&arg);
					}
				}
			}//for
		}
	}
}

void Balance::add_fd(int fd)
{
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
    ev.data.fd  = fd;
    if(epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev) == -1)
    {
        perror("epoll ctl error");
    }
    setnonblock(fd);
}

void Balance::del_fd(int fd)
{
	if(epoll_ctl(epfd,EPOLL_CTL_DEL,fd,NULL) == -1)
    {
        perror("epoll ctl error" );
    }
}
void Balance::setnonblock(int fd)
{
	int oldflg = fcntl(fd,F_GETFL);
    int newflg = oldflg | O_NONBLOCK;
    if( fcntl(fd,F_SETFL,newflg) == -1)
    {
        perror("fcntl error");
    }
}



void Balance::ConSer(char* host, unsigned short port)
{
	int serfd = socket(AF_INET,SOCK_STREAM,0);
	assert(serfd != -1);

	struct sockaddr_in saddr;
	memset(&saddr,0,sizeof(saddr));

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr("192.168.1.108");

	int res = connect(serfd,(struct sockaddr*)&saddr,sizeof(saddr));
	assert(res != -1);

	ser_fd.push(serfd);	
	cout<<"server ip:"<<inet_ntoa(saddr.sin_addr)<<" port:"<<
    ntohs(saddr.sin_port)<<endl;

}


void* Balance::Readcli(void *arg)
{//创建线程
	struct mArgu* tmp = (struct mArgu*)arg;
	int fd = tmp->fd;
	char buff[1024] = {0};
	int c = 0;
	map<int,int>::iterator it = tmp->mb->mcli.find(fd);
	if(it == tmp->mb->mcli.end())
	{
		 c = tmp->mb->ser_fd.front();
		 tmp->mb->ser_fd.pop();
		tmp->mb->mcli.insert(make_pair(fd,c));
		tmp->mb->ser_fd.push(c);
	}
	else
	{c = it->second;}
	while(true)
	{
		int n = recv(fd,buff,1023,0);
		if(n == -1)break;
		if(n == 0)
		{
			tmp->mb->del_fd(fd);
			close(fd);
			break;
		}
		cout<<"recv: "<<buff<<endl;
		
		
		int m = send(c,buff,n,0);// 向ser发送数据
		cout<<"send: "<<c<<endl;
		memset(buff,0,1024);
		if(recv(c,buff,1023,0) == 2)
		{
			cout<<"ser:"<<buff<<endl;
			send(fd,"ok",2,0);
		}
			
		memset(buff,0,1024);
	}
	pthread_exit(NULL);
}


/*
void Balance::Writeser()
{
	send(serfd,buff,strlen(buff),0);
	memset(buff,0,1024);
	
	recv(serfd,buff,1024,0);
	memset(buff,0,1024);
}
*/



int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		cout << "command is invalid! ./a.out ip port!" << endl;
		exit(0);
	}
	unsigned short port = atoi(argv[2]);
	
	Balance B(argv[1],port);
	unsigned short p = 0;
	while(cin>>p,p)
	{
		B.ConSer(argv[1],p);//连接一个服务器
		//B.ConSer(argv[1],6700);
	}

	B.wait();
	return 0;
	
}