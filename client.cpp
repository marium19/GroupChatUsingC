#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>         
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
using namespace std;

int main(){
	int sockfd,fdmax,i;
	struct sockaddr_in serv;
	fd_set master,copy;

	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0){
		perror("socket failed");
		exit(1);
	}

	serv.sin_family=AF_INET;
	serv.sin_port=htons(4480);
	serv.sin_addr.s_addr=inet_addr("127.0.0.1");

	if((connect(sockfd,(struct sockaddr*)&serv,sizeof(struct sockaddr_in)))<0){
		perror("connect failed");
		exit(1);
	}

	FD_ZERO(&master);
	FD_ZERO(&copy);
	FD_SET(0,&master);
	FD_SET(sockfd,&master);
	fdmax=sockfd;

	while(1){
		copy=master;
		if((select(fdmax+1,&copy,nullptr,nullptr,nullptr))<0){
			perror("select");
			exit(1);
		}

		for(i=0;i<=fdmax;i++){
			if(FD_ISSET(i,&copy)){
				char sendbuf[4096],recvbuf[4096];
				int byteRecv;

				if(i==0){
					fgets(sendbuf,4096,stdin);
					send(sockfd,sendbuf,strlen(sendbuf),0);
				}
				else{
					byteRecv=recv(sockfd,recvbuf,4096,0);
					recvbuf[byteRecv]='\0';
					printf("%s\n",recvbuf);
					fflush(stdout);
				}
			}
		}
	}
	close(sockfd);
}


