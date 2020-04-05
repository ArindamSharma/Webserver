#include<stdio.h>
#include<sys/socket.h>//for socket(),bind,AF_INET,SOCKSTREAM,linsten,accept
#include<arpa/inet.h>//for sockaddr_in,inet_addr,htons,inet_ntoa,sockaddr 
#include<netinet/in.h>//for htons,ntohs
#include<stdlib.h>//for exit(),malloc,system
#include<unistd.h>//for read(),write(),close(int fd)
#include<pthread.h>//for pthread_create,pthread_t,pthread_exit
#include<errno.h>//for errno
#include<string.h>//for strlen()

#define MAX 3
#define PORT 9007
#define RED  "\x1B[31m"
#define GRN  "\x1B[32m"
#define YEL  "\x1B[33m"
#define BLU  "\x1B[34m"
#define MAG  "\x1B[35m"
#define CYN  "\x1B[36m"
#define WHT  "\x1B[37m"
#define RESET "\x1B[0m"

char ip[16];
int client[MAX][2];
int active_client,server;
struct sockaddr_in other[MAX];
pthread_t tid[MAX],tid_thread_creator,tid_cmd;

void *subserver(void *arg);
void *thread_creator(void *arg);
void *cmd(void *arg);
void terminate_all();
void terminate(int i);
void quit();

int main(){
    active_client=0;
    for(int i=0;i<MAX;i++){
        client[i][1]=0;
    }

    system("hostname -I > ip.txt");
    FILE *A;
    A=fopen("ip.txt","r");
    fscanf(A,"%s ",ip);
	fclose(A);
	system("rm ip.txt");
    
    struct sockaddr_in serveraddr;
    serveraddr.sin_addr.s_addr=inet_addr(ip);
    // serveraddr.sin_addr.s_addr=INADDR_ANY;
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(PORT);

    server=socket(AF_INET,SOCK_STREAM,0);
    int stat=bind(server,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
    if(stat==-1){
        int try=1;
        printf(RED"Error in Creating Connection \n");
        while(stat==-1){
            printf("[Trial no:- %d] Error no :- %d\r",try++,errno);
            usleep(30000);
            stat=bind(server,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
        }
        printf("\n"RESET);
        // exit(1);
    }
    printf("Connection Established\n");
    printf(GRN"Server Started \n"RESET);
    printf(CYN"AT LINK :- http://%s:%d\n"RESET,ip,PORT);

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
    close(server);
    return 0;
}

void terminate_all(){
    int c=0;
    for(int i=0;i<MAX;i++){
        if(client[i][1]==1){
            terminate(i);
        }
        else{
            c++;
        }
    }
    if(c==MAX){
        printf("No Active Client to Terminate\n");
    }
}

void terminate(int i){
    close(client[i][0]);
	client[i][1]=0;
    active_client--;
	pthread_cancel(tid[i]);
	printf("Terminating Client [%s:%d]\n",inet_ntoa(other[i].sin_addr),ntohs(other[i].sin_port));
}

void quit(){
	terminate_all();
	printf("Server Closed\n");
	pthread_cancel(tid_thread_creator);
	close(server);
	exit(1);
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
        else if(strcmp("terminate\n",buff)==0){
            terminate_all();
        }
        else if(strcmp("list\n",buff)==0){
            printf("\n        +");
            for(int i=0;i<MAX;i++){
                printf("---+");
            }
            printf("\nlist :- |");
            for(int i=0;i<MAX;i++){
                printf(" %d |",client[i][0]);
            }
            printf("\n        +");
            for(int i=0;i<MAX;i++){
                printf("---+");
            }
            printf("\nstat :- |");
            for(int i=0;i<MAX;i++){
                printf(" %d |",client[i][1]);
            }
            printf("\n        +");
            for(int i=0;i<MAX;i++){
                printf("---+");
            }
            printf("\n");
        }

    }
    pthread_exit(NULL);
}

void *subserver(void *arg){
    int x=*(int *)arg;
	// printf("%d\n",x);
    printf("\nClient Connected [%s:%d] (%d)\n",inet_ntoa(other[x].sin_addr),ntohs(other[x].sin_port),x+1);
	char method[10];
    char resourse[100];
    char accept[100];
    char accept_language[50];
    char userAgent[200];
    char acceptEncoding[20];
    char host[50];
    char connection[20];
    char response[493];
    int count=0;
    int stat=1;
    while(stat){
		char filename[1];
		stat=recv(client[x][0],filename,sizeof(filename),0);
		// if(stat<=0){
			
		// 	break;
		// }
		// else
        {
            char tmp=filename[0];
            int p=tmp;
            if(p==92 || p==34 || p==39 || p<32){
                response[count++]='\n';
            }
            else{
			    // printf("[%c]%d\n",tmp,p);
                response[count++]=tmp;
            }
		}
	}
            printf("\nClient Disconected [%s:%d] (%d)\n",inet_ntoa(other[x].sin_addr),ntohs(other[x].sin_port),x+1);
			client[x][1]=0;
            close(client[x][0]);
			active_client--;
            printf("%s",response);

	pthread_exit(NULL);
}