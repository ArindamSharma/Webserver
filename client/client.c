#include<stdio.h>
// #include<pthread.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<errno.h>
int main(){
	printf("Client\n");
    int client=socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in clientaddr;
    clientaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    clientaddr.sin_family=AF_INET;
    clientaddr.sin_port=htons(9007);
	// printf("");
    int stat=connect(client,(struct sockaddr *)&clientaddr,sizeof(clientaddr));
    if(stat==-1){
        printf("Error in Connecting :%d\n",errno);
		exit(1);
    }
    printf("Connected\n");
    printf("Enter the name :-");
    char buff[100];
    fgets(buff,sizeof(buff),stdin);
    send(client,buff,sizeof(buff),0);
    return 0;
}