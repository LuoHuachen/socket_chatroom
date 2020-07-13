///Linux客户端
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 1223
#define maxnum 100
#define IP "192.168.50.130"
#define getLen(zero) sizeof(zero)/sizeof(zero[0])
using namespace std;

void init(const int &sockfd){
    if(send(sockfd, "xiejin", 9, 0) == -1) {
        perror("send()");
        close(sockfd);
        return ;
    }
}

void* recvMsg(void* sockid) {
    const int sockfd=*((int*)sockid);
    char buf[maxnum];
    while(true) {
        int num = recv(sockfd, buf, maxnum, 0);
        if(num == -1) {
            perror("recv()");
            return 0;
        }
        buf[num]='\0';
        cout/**<<"recv: "**/<<buf<<endl;
    }
    return 0;
}
void* sendMsg(void* sockid) {
    const int sockfd=*((int*)sockid);
    char str[maxnum];
    while(true) {
        scanf("%s", str);
        if(send(sockfd, str, strlen(str), 0) == -1) {
            perror("send()");
            close(sockfd);
            return 0;
        }
    }
    return 0;
}

int main() {

    int sockfd, numbytes;
    struct sockaddr_in their_addr;

    puts("USER:");
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket():");
        return 0;
    }

    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(PORT);

    their_addr.sin_addr.s_addr = inet_addr(IP);
    bzero(&(their_addr.sin_zero),getLen(their_addr.sin_zero));
    if(connect(sockfd,(struct sockaddr *)&their_addr,sizeof(struct sockaddr)) == -1) {
        perror("connect():");
        return 0;
    }

    init(sockfd);

    pthread_t thread[2];
    if(pthread_create(&thread[0], NULL, recvMsg, (void *)&(sockfd)) != 0) {
        perror("pthread_create()");
        return 0;
    }
    if(pthread_create(&thread[1], NULL, sendMsg, (void *)&(sockfd)) != 0) {
        perror("pthread_create()");
        return 0;
    }
    while(true);
    pthread_exit(NULL);
    close(sockfd);
    return 0;
}
