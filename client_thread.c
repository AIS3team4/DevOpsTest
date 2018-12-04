#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<strings.h>
#include<string.h>

void* recv_thread(void*);	//接收的執行緒

char sendbuf[2048]={0},recvbuf[2048]={0},name[2048]={0};

int main(int argc,char **argv){
	int num_bytes,sockfd;
	struct sockaddr_in cli_addr;
	struct hostent *he;

	if(argc!=2)
		printf("usage:<ipaddress>!\n");
	
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
		printf("socket error!\n");
	
	bzero(&cli_addr,sizeof(cli_addr));
	
	cli_addr.sin_family=AF_INET;	//set family
	cli_addr.sin_port=htons(9877);	//set port
	
	if((he=gethostbyname(argv[1])) == NULL)
		printf("inet_pton error!\n");
	cli_addr.sin_addr=*((struct in_addr*)he->h_addr);	//set addr

	if(connect(sockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) < 0)
		printf("connect error!\n");
	
	printf("請輸入名子:");
	fgets(name,2048,stdin);
        send(sockfd,name,(strlen(name)),0);       //設定名子

	pthread_t tid;
	pthread_create(&tid,NULL,(void*)recv_thread,(void*)&sockfd);//建立執行緒
	
	while(1){
		memset(sendbuf,0,sizeof(sendbuf));
		fgets(sendbuf,2048,stdin);		//使用者輸入
		send(sockfd,sendbuf,(strlen(sendbuf)),0);//發送訊息
		
	}
	close(sockfd);
	return 0;
}
void* recv_thread(void *fd){
	int cli_fd=*(int*)fd;

	while(1){
		if((recv(cli_fd,recvbuf,2048,0))<=0){	//收訊息
			printf("recv error!\n");
			exit(1);
		}
		printf("%s",recvbuf);			//將收到訊息印出
		memset(recvbuf,0,sizeof(recvbuf));
	}
}


