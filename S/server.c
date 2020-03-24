#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<errno.h>
#include<strings.h>
#include<unistd.h>

#define MAX 2
#define PORT 9007
char ip[]="127.0.0.1";

int client[MAX][2];
struct sockaddr_in serveraddr,other[MAX];
pthread_t tid[MAX],cmd;
int active_client;
void quit(){
	for(int i=0;i<MAX;i++){
		if(client[i][1]==1){
			pthread_cancel(tid[i]);
			printf("Terminating client\n");
		}
	}
	exit(1);
}
void * subserver(void * arg){
	int x=*(int *)arg;
	// printf("%d\n",x);
	while(1){
		char filename[100];
		int stat=recv(client[x][0],filename,sizeof(filename),0);
		if(stat<=0){
			printf("Client Disconected\n");
			client[x][1]=0;
			active_client--;
			break;
		}
		else{
			printf("%s",filename);
			printf("%s\bl",filename);
		}
	}
	pthread_exit(NULL);
}
void * command(void * arg){
	while(1){
		char buff[100];
		fgets(buff,sizeof(buff),stdin);
		if(strcasecmp(buff,"exit\n")==0){
			printf("Bye\n");
			quit();
		}
		else if(strcasecmp(buff,"ls\n")==0){
			for(int i=0;i<MAX;i++){
				if(client[i][1]==1)
					printf("%d\n",i);
			}
		}
		else if(strcasecmp(buff,"count\n")==0){
			printf("Active Clients are :- %d\n",active_client);
		}
		// else if(strcasecmp(buff,"restart\n")==0){
		// 	if(fork()>0){
		// 		exit(1);
		// 	}
		// 	else{
		// 		printf("Restarting\n");
		// 		main();
		// 	}
		// }
	}
	pthread_exit(NULL);
}
int main(){
	active_client=0;
	for(int i=0;i<MAX;i++){
		client[i][1]=0;
	}
    int server=socket(AF_INET,SOCK_STREAM,0);
    serveraddr.sin_addr.s_addr=inet_addr(ip);
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(PORT);

    int stat=bind(server,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
    if(stat==-1){
        printf("Error in Creating Socket :%d\n",errno);
		exit(1);
    }
	pthread_create(&cmd,NULL,command,NULL);

    printf("Server Started\n");
    listen(server,MAX);
    printf("Listenning ...\n");
	while(1){
		if(active_client<MAX){
			struct sockaddr_in temp_other;
			socklen_t size=sizeof(temp_other);
			int temp_client=accept(server,(struct sockaddr *)&temp_other,&size);
			
			int i,*f=malloc(sizeof(int*));
			for(i=0;i<MAX;i++){
				if(client[i][1]==0){
					break;
				}
			}
			*f=i;
			temp_other=other[i];
			client[i][0]=temp_client;
			client[i][1]=1;
			active_client++;
			pthread_create(&tid[i],NULL,subserver,f);
		}
	}
	return 0;
}