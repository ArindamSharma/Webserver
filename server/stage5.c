#include<stdio.h>
#include<sys/socket.h>//for socket(),bind,AF_INET,SOCKSTREAM,linsten,accept
#include<arpa/inet.h>//for sockaddr_in,inet_addr,htons,inet_ntoa,sockaddr 
#include<netinet/in.h>//for htons,ntohs
#include<stdlib.h>//for exit(),malloc,system
#include<unistd.h>//for read(),write(),close(int fd)
#include<pthread.h>//for pthread_create,pthread_t,pthread_exit
#include<errno.h>//for errno
#include<string.h>//for strcmp strcpy

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
        printf("Error\n");
        printf(RED"Error in Creating Connection \n%s\n",strerror(errno));
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
    if(client[i][1]==1){
        close(client[i][0]);
        client[i][1]=0;
        active_client--;
        pthread_cancel(tid[i]);
        printf("Terminating Client [%s:%d]\n",inet_ntoa(other[i].sin_addr),ntohs(other[i].sin_port));
    }
}

void quit(){
	terminate_all();
	printf("Server Closed\n");
	pthread_cancel(tid_thread_creator);
	close(server);
    pthread_cancel(tid_cmd);
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
            printf("Server Closing ...\n");
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
    printf("Client Connected [%s:%d] \n",inet_ntoa(other[x].sin_addr),ntohs(other[x].sin_port));
    char firstline[135];
    int firstcount=0;
    char method[10];
    char request[100];
    char http[10];
    int status;
    // while(1)
    {
        char tmp_char;
        status=recv(client[x][0],&tmp_char,sizeof(tmp_char),0);
        while(tmp_char!=' '){
            method[firstcount++]=tmp_char;
            status=recv(client[x][0],&tmp_char,sizeof(tmp_char),0);
        }
        firstcount=0;
        status=recv(client[x][0],&tmp_char,sizeof(tmp_char),0);
        while(tmp_char!=' '){
            request[firstcount++]=tmp_char;
            status=recv(client[x][0],&tmp_char,sizeof(tmp_char),0);
        }
        firstcount=0;
        status=recv(client[x][0],&tmp_char,sizeof(tmp_char),0);
        while(tmp_char!='\n')
        {
            // printf("[%c]\n",tmp_char);
            http[firstcount++]=tmp_char;
            status=recv(client[x][0],&tmp_char,sizeof(tmp_char),0);
        }
        http[firstcount-1]='\0';
    }
    printf("[%s-%s-%s]\n",http,method,request);
    if(strcmp(method,"POST")==0){
        
    }
    else if(strcmp(method,"GET")==0){
        
    }
    while(1)
    {
        char line[250];
        int char_count=0;
        while(1)
        {
            char tmp_char;
            status=recv(client[x][0],&tmp_char,sizeof(tmp_char),0);
            if(status>0){
                if(tmp_char=='\n'){
                    line[char_count]='\0';
                    break;
                }
                else{
                    line[char_count++]=tmp_char;
                }
            }
            else{
                client[x][1]=0;
                break;
            }
        }
        if(client[x][1]==0 ||strcmp(line,"\r\0")==0){
            break;
        }
        // printf("%s\n",line);
    }
    if(client[x][1]==0){
        printf("thread is closing because client disconnected\n");
        close(client[x][0]);
        active_client--;
        pthread_exit(NULL);
    }
    printf("Out of While Loop \nBut client is still connected \n");

    printf("Client has to Disconect [%s:%d]\n",inet_ntoa(other[x].sin_addr),ntohs(other[x].sin_port));
    client[x][1]=0;
    close(client[x][0]);
    active_client--;
    pthread_exit(NULL);
}