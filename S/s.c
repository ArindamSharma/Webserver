#include<stdio.h>
#include<sys/socket.h>//for socket()
#include<arpa/inet.h>//for sockaddr_in,inet_addr,htons
#include<stdlib.h>//for exit()
#include<unistd.h>//for read(),write()
int main(){
    int server=socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in serveraddr,other;
    serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(9007);

    int stat=bind(server,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
    if(stat==-1){
        printf("Error in Creating Connection \n");
        exit(1);
    }
    printf("Connection Established\n");
    printf("Server Started\n");
    listen(server,5);
    printf("Listening...\n");
    socklen_t size=sizeof(other);
    int client=accept(server,(struct sockaddr *)&other,&size);
    while(1)
    {
        char fname[100];
        int x=recv(client,fname,sizeof(fname),0);
        if(x<=0){
            break;
        }
        printf("%s",fname);
    }
    return 0;
}