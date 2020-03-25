#include<stdio.h>
#include<stdlib.h>//for exit()
#include<sys/socket.h>//for socket()
#include<unistd.h>//for read(), write()
#include<arpa/inet.h>//for socketaddr,inet_addr,sockaddr_in,htons
int main(){
    printf("Client\n");
    int client=socket(AF_INET,SOCK_STREAM,0);
    
    struct sockaddr_in clientaddr;
    // clientaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    clientaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    clientaddr.sin_port=htons(9007);
    clientaddr.sin_family=AF_INET;

    int stat=connect(client,(struct sockaddr *)&clientaddr,sizeof(clientaddr));
    if(stat==-1){
        printf("Error in Connection\n");
        exit(1);
    }
    printf("Connection Establish\n");
    while(1){    
        char buff[100];
        printf("enter the filename :- ");
        fgets(buff,sizeof(buff),stdin);
        int x=send(client,buff,sizeof(buff),0);
        printf("%d\n",x);
    }
    return 0;
}