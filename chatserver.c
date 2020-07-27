#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/times.h>
#include <sys/select.h>

int main(){
	fd_set master,copy;
	int sockfd,fdmax,i,yes=1;
	struct sockaddr_in serv,client;

	FD_ZERO(&master);
	FD_ZERO(&copy);

	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0){
		perror("socket failed");
		exit(1);
	}

	serv.sin_family=AF_INET;
	serv.sin_port=htons(4480);
	serv.sin_addr.s_addr=INADDR_ANY;

	if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1){
		perror("setsockopt");
		exit(1);
	}

	//bind
	
	if((bind(sockfd,(struct sockaddr*)&serv,sizeof(struct sockaddr)))<0){
		perror("bind");
		exit(1);
	}

	//listen
	if((listen(sockfd,10))<0){
		perror("listen");
		exit(1);
	}

	printf("server waiting on port 4950");

	FD_SET(sockfd,&master);

	fdmax=sockfd;

	while(1){
		copy=master;
		if(select(fdmax+1,&copy,NULL,NULL,NULL)<0){
			perror("select");
			exit(1);
		}

		for(i=0;i<=fdmax;i++){
			if(FD_ISSET(i,&copy)){

				if(i==sockfd){
					socklen_t s;
					int newsd;
					s=sizeof(struct sockaddr_in);

					if((newsd=accept(sockfd,(struct sockaddr*)&client,&s))<0){
						perror("accept");
						exit(1);
					}
					else{
						FD_SET(newsd,&master);
						if(newsd>fdmax)
							fdmax=newsd;
						printf("new connection connected");
					}
				}
				else{
					int bytesRecv,j;
					char buff[1024];

					if((bytesRecv=recv(i,buff,1024,0))<=0){
						if(bytesRecv==0)
							printf("socket hung up");
						else
							perror("recv");
						close(i);
						FD_CLR(i,&master);
					}
					else{
						printf("%s\n",buff);
						for(j=0;j<=fdmax;j++){
							if(FD_ISSET(j,&master)){
								if(j!=sockfd && j!=i){
									if(send(j,buff,bytesRecv,0)<0)
										perror("send");
								}
							}
						}
					}
				}
			}
		}
	}
}







				



			




