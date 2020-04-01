#include<stdio.h>
#include<sys/socket.h>//for socket()
#include<arpa/inet.h>//for sockaddr_in,inet_addr,htons
#include<stdlib.h>//for exit()
#include<unistd.h>//for read(),write()
#include<pthread.h>//for pthread_create
// #include<string.h>
#define MAX 5
#define PORT 9007
int main(){
    int server=socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in serveraddr,other;
    serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(PORT);

    int stat=bind(server,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
    if(stat==-1){
        printf("Error in Creating Connection \n");
        exit(1);
    }
    printf("Connection Established\n");
    printf("Server Started\n");

    listen(server,MAX);
    printf("Listening...\n");

    socklen_t size=sizeof(other);
    int client=accept(server,(struct sockaddr *)&other,&size);
    // while(1)
    // {
        char fname[100];
        int x=recv(client,fname,sizeof(fname),0);
        printf("%s\n",fname);
        FILE *A;
        A=fopen(fname,"rb");
        int tmp=fgetc(A),c=0;
        send(client,&tmp,sizeof(&tmp),0);
        while(tmp!=EOF){
            tmp=fgetc(A);
            c++;
            send(client,&tmp,sizeof(&tmp),0);
        }
        fclose(A);
        printf("\n");
    // }
    return 0;
}