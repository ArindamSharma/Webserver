#include<stdio.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>

pthread_t tid;
int pid;
void * runner(void * arg){
    system("gcc server.c -lpthread");
    system("./a.out");
    // pid=getpid();
    printf("%d\n",getpid() );
    pthread_exit(NULL);
}
int main(){
    struct stat attr1,attr2;
    stat("server.c",&attr1);
    printf("Started\n");
    pthread_create(&tid,NULL,runner,NULL);
    printf("%d\n",getpid());
    while(1){
        sleep(1);
        stat("server.c",&attr2);
        if(attr1.st_mtime!=attr2.st_mtime){
            attr1=attr2;
            // pthread_cancel(tid);
            printf("Restarting\n");
            fputs("exit\n",stdin);
            // system("kill -SIGINT 'pgrep a.out | sed -n 2p'");
            // pthread_create(&tid,NULL,runner,NULL);
        }
    }
    return 0;
}