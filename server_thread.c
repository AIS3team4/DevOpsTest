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
#define Max 10

	
struct client{
	int id;
};
struct client fd[Max];
int cli_number=0;
void *server_thread(void *);
 
int main(int argc,char **argv){
	int sockfd,connectfd,i;
	struct sockaddr_in serv_addr,cli_addr;
	socklen_t cli_len;
	
	for(i=0;i<Max;i++)
		fd[i].id=0;
	
	bzero(&serv_addr,sizeof(serv_addr));	//initialized
	
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
		printf("socket error!\n");

	serv_addr.sin_family=AF_INET;		//set family
	serv_addr.sin_port=htons(9877);		//set port 
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);	//set 每個都可接收

	bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	
	if((listen(sockfd,Max))<0)
		printf("listen error!\n");
	
	cli_len=sizeof(cli_addr);

	while(1){
		if((connectfd=accept(sockfd,(struct sockaddr*)&cli_addr,&cli_len))<0)
			printf("accept error!\n");
		if(cli_number>=Max){
			printf("no more client!\n");
			close(connectfd);
		}
		for(i=0;i<Max;i++){
			if(fd[i].id==0){
				fd[i].id=connectfd;
				break;
			}
		}
		pthread_t tid;
		pthread_create(&tid,NULL,(void*)server_thread,&fd[i]);

		cli_number++;
	}
	
	close(sockfd);	
	return 0;
}
void *server_thread(void* client){
	struct client cli_fd=*(struct client*)client;
	char buf[2048];
	int num_bytes,i;
	
	while(1){
		memset(buf,0,2048);
		if((num_bytes=recv(cli_fd.id,buf,2048,0))<=0){
			for(i=0;i<Max;i++){
				if(fd[i].id==cli_fd.id){
					fd[i].id=0;
					break;
				}
			}
			printf("%d exit!\n",cli_fd.id);
			cli_number--;
			break;
		}
		for(i=0;i<Max;i++){
			if((fd[i].id!=0)&&(fd[i].id!=cli_fd.id))
				send(fd[i].id,buf,num_bytes,0);
		}
	}
}




