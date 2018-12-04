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
#include<fcntl.h>
#include<sys/sendfile.h>
#define Max 10

	
struct client{
	int id;		//file des
	int num;	//矩陣中第幾個
	char name[2048];	//名子
	int flag;	//紀錄是否秘語
};

struct client fd[Max];
int cli_number=0,num_sendfile=-1;
void *server_thread(void *);
unsigned long fsize(char*);  //看檔案的大小

int main(int argc,char **argv){
	int sockfd,connectfd,i;
	struct sockaddr_in serv_addr,cli_addr;
	socklen_t cli_len;
	
	for(i=0;i<Max;i++){
		fd[i].id=0;
		memset(fd[i].name,0,2048);
	}
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
				fd[i].num=i;
				fd[i].flag=-1;
				recv(connectfd,fd[i].name,2048,0);
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
	int num_bytes,i;
	char buf[2048];

//	if(fd[num_sendfile].id!=cli_fd.id){	
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

		if(strncmp(buf,"show",4)==0){
			int j;	
			for(i=0;i<Max;i++){
				for(j=0;j<Max;j++){
					if(fd[j].id!=0)
						send(fd[i].id,fd[j].name,sizeof(fd[j].name),0);
				}
			}
		}
		else if(strncmp(buf,"file",4)==0){
			for(i=0;i<Max;i++){
                                if(strncmp(&buf[5],fd[i].name,sizeof(fd[i].name))==0){
					num_sendfile=i;
                                        break;
				}
                	}
		}
		else if((strncmp(buf,"yy",2)==0||strncmp(buf,"nn",2)==0)&&num_sendfile>=0&&strncmp(&buf[3],cli_fd.name,sizeof(cli_fd.name)-1)==0){
                                        int fptr;
                                        //char notice[]="accept file(y or n)\0";

                                        fptr=open("test.txt",O_RDONLY);
                                        sendfile(fd[num_sendfile].id,fptr,NULL,fsize("test.txt"));
				num_sendfile=-1;
		}
		else if(strncmp(buf,"secret",6)==0){
			for(i=0;i<Max;i++){
				if(strncmp(&buf[7],fd[i].name,sizeof(fd[i].name))==0){

					cli_fd.flag=i;
					break;
				}
			}
		}
		else if(strncmp(buf,"exit",4)==0)
			cli_fd.flag=-1;
		else{
			if(cli_fd.flag<0){
				for(i=0;i<Max;i++){
					if(fd[i].id!=0){
						send(fd[i].id,fd[cli_fd.num].name,sizeof(fd[cli_fd.num].name),0);
						send(fd[i].id,": ",2,0);
						send(fd[i].id,buf,num_bytes,0);
					}
				}
			}
			else{
				send(fd[cli_fd.flag].id,fd[cli_fd.num].name,sizeof(fd[cli_fd.num].name),0);
				send(fd[cli_fd.flag].id,": ",2,0);
                                send(fd[cli_fd.flag].id,buf,num_bytes,0);
				
			}
		}
	}
//	}
//	else{
	//	printf("sssssssss");
	//	char recvbuf[2048];
		//memset(recvbuf,0,2048);
		//if((num_bytes=recv(cli_fd.id,recvbuf,2048,0))<=0){
                //        for(i=0;i<Max;i++){
                //                if(fd[i].id==cli_fd.id){
                    //                    fd[i].id=0;
                   //                     break;
                  //              }
                 //       }
		//}

		
//			int fptr;
 //                      	fptr=open("test.txt",O_RDONLY);	
//               		sendfile(fd[num_sendfile].id,fptr,NULL,fsize("test.txt"));
		
	//	num_sendfile=-1;
	//}
}
unsigned long fsize(char * file){  //看檔案的大小
 	FILE *f=fopen(file,"r");
 	fseek(f,0,SEEK_END);
 	unsigned long len=(unsigned long)ftell(f);
 	fclose(f);
 	return len;
}




