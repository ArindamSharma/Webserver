#include<stdio.h>
#include<sys/socket.h>//for socket(),bind,AF_INET,SOCKSTREAM,linsten,accept
#include<arpa/inet.h>//for sockaddr_in,inet_addr,htons,inet_ntoa
#include<netinet/in.h>//for htons,ntohs
#include<stdlib.h>//for exit(),malloc
#include<unistd.h>//for read(),write()
#include<pthread.h>//for pthread_create,pthread_t 
#include<errno.h>//for errno
// #include<string.h>
#define MAX 2
#define PORT 9007
int client[MAX][2];
int active_client;
struct sockaddr_in other[MAX];
pthread_t tid[MAX];

void *subserver(void *arg){
    int x=*(int *)arg;
	// printf("%d\n",x);
    printf("\nClient Connected [%s:%d] (%d)\n",inet_ntoa(other[x].sin_addr),ntohs(other[x].sin_port),active_client);
	while(1){
		char filename[100];
		int stat=recv(client[x][0],filename,sizeof(filename),0);
		if(stat<=0){
			printf("\nClient Disconected [%s:%d] (%d)\n",inet_ntoa(other[x].sin_addr),ntohs(other[x].sin_port),active_client);
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
int main(){
    active_client=0;
    for(int i=0;i<MAX;i++){
        client[i][1]=0;
    }
    int server=socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in serveraddr;
    // serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    serveraddr.sin_addr.s_addr=INADDR_ANY;
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(PORT);

    int stat=bind(server,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
    if(stat==-1){
        int try=1;
        printf("Error in Creating Connection \n");
        while(stat==-1){
            printf("[Trial no:- %d] Error no :- %d\r",try++,errno);
            usleep(30000);
            stat=bind(server,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
        }
        printf("\n");
        // exit(1);
    }
    printf("Connection Established\n");
    printf("Server Started\n");

    listen(server,MAX);
    printf("Listening...\n");
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

    // {
    //     char fname[100];
    //     int x=recv(client,fname,sizeof(fname),0);
    //     printf("%s\n",fname);
    //     FILE *A;
    //     A=fopen(fname,"rb");
    //     int tmp=fgetc(A),c=0;
    //     send(client,&tmp,sizeof(&tmp),0);
    //     while(tmp!=EOF){
    //         tmp=fgetc(A);
    //         c++;
    //         send(client,&tmp,sizeof(&tmp),0);
    //     }
    //     fclose(A);
    //     printf("\n");
    // }
    return 0;
}