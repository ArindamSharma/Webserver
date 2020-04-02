#include<stdio.h>
#include<sys/socket.h>//for socket(),bind,AF_INET,SOCKSTREAM,linsten,accept
#include<arpa/inet.h>//for sockaddr_in,inet_addr,htons,inet_ntoa
#include<netinet/in.h>//for htons,ntohs
#include<stdlib.h>//for exit(),malloc,system
#include<unistd.h>//for read(),write()
#include<pthread.h>//for pthread_create,pthread_t 
#include<errno.h>//for errno
#include<string.h>//for strlen()

#define MAX 2
#define PORT 9007

char ip[16];
int client[MAX][2];
int active_client,server;
struct sockaddr_in other[MAX];
pthread_t tid[MAX],tid_thread_creator,tid_cmd;

void *subserver(void *arg);
void *thread_creator(void *arg);
void *cmd(void *arg);
void terminate();
void quit();

void terminate(){
	for(int i=0;i<MAX;i++){
		if(client[i][1]==1){
			close(client[i][0]);
			pthread_cancel(tid[i]);
			printf("Terminating Client [%s:%d]\n",inet_ntoa(other[i].sin_addr),ntohs(other[i].sin_port));
			client[i][1]=0;
			// number_of_client--;
		}
	}
}

void quit(){
	terminate();
	printf("Server Closed\n");
	pthread_cancel(tid_thread_creator);
	close(server);
	exit(1);
}

void *subserver(void *arg){
    int x=*(int *)arg;
	// printf("%d\n",x);
    printf("\nClient Connected [%s:%d] (%d)\n",inet_ntoa(other[x].sin_addr),ntohs(other[x].sin_port),x+1);
	while(1){
		char filename[123];
		int stat=recv(client[x][0],filename,sizeof(filename),0);
		if(stat<=0){
			printf("\nClient Disconected [%s:%d] (%d)\n",inet_ntoa(other[x].sin_addr),ntohs(other[x].sin_port),x+1);
			client[x][1]=0;
			active_client--;
			break;
		}
		else{
			printf("%s",filename);
		}
	}
	pthread_exit(NULL);
}
void *thread_creator(void *arg){
    while(1){
        if(active_client<MAX){
            
            int i,stop=0,*f=malloc(sizeof(int*));
            for(i=0;i<MAX;i++){
                if(client[i][1]==0){
                    break;
                }
                else{
                    stop++;
                }
            }
            if(stop!=MAX){
                *f=i;
                socklen_t size=sizeof(other[i]);
                client[i][0]=accept(server,(struct sockaddr *)&other[i],&size);
                client[i][1]=1;
                active_client++;
                pthread_create(&tid[i],NULL,subserver,f);
            }
        }
    }
    pthread_exit(NULL);
}

void *cmd(void *arg){
    while(1){
        char buff[101];
        fgets(buff,sizeof(buff),stdin);
        // printf("%s\n",buff);
        if(strcmp("exit\n",buff)==0){
            printf("Bye\n");
            quit();
        }
        else if(strcmp("ls\n",buff)==0){
            int sum=0,counter=1;
            printf("-------------------------\n");
            for(int i=0;i<MAX;i++){
                if(client[i][1]==1){
                    printf("%d. [%s:%d]\n",counter++,inet_ntoa(other[i].sin_addr),ntohs(other[i].sin_port));
                }
                else{
                    sum++;
                }
            }
            if(sum==MAX){
                printf("No Active Client\n");
            }
            printf("-------------------------\n");
        }
    }
    pthread_exit(NULL);
}

int main(){
    active_client=0;
    for(int i=0;i<MAX;i++){
        client[i][1]=0;
    }

    system("hostname -I > ip.txt");
    FILE *A;
	A=fopen("ip.txt","r");
    fgets(ip,sizeof(ip),A);
	fclose(A);
	system("rm ip.txt");
	ip[strlen(ip)-1]='\0';
    
    struct sockaddr_in serveraddr;
    serveraddr.sin_addr.s_addr=inet_addr(ip);
    // serveraddr.sin_addr.s_addr=INADDR_ANY;
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(PORT);

    server=socket(AF_INET,SOCK_STREAM,0);
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
    printf("Server Started \nLINK :- http://%s\b:%d\n",ip,PORT);

    pthread_create(&tid_cmd,NULL,cmd,NULL);
    
    listen(server,MAX);
    printf("Listening...\n");

    pthread_create(&tid_thread_creator,NULL,thread_creator,NULL);
    pthread_join(tid_thread_creator,NULL);

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