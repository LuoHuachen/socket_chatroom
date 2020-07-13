///Windows客户端
///采用两个线程，分别进行读和写
#include <thread>
#include <iostream>
#include <stdlib.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")


#define PORT 1223
#define maxnum 100
using namespace std;



void recvMsg(const int &sockfd) {///接收信息
    char buf[maxnum];
    while(true) {
        int num = recv(sockfd, buf, maxnum, 0);
        if(num == -1) {
            perror("recv()");
            return ;
        }
        buf[num]='\0';
        cout<<buf<<endl;
    }
    return ;
}
void init(const int &sockfd) {///发送初始化信息,即昵称
    char str[10] = "Thanks_up";
    if(send(sockfd, str, strlen(str), 0) == -1) {
        perror("send()");
    }
    return ;
}

int main( ) {
    WORD sockVersion = MAKEWORD(2,2);
    WSADATA wsaData;
    if(WSAStartup(sockVersion, &wsaData)!=0){
        return 0;
    }

    int sockfd, numbytes;
    char buf[maxnum];
    struct hostent *he;

    struct sockaddr_in their_addr;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        cout<<WSAGetLastError()<<endl;
        perror("socket");
        return 0;
    }

    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(PORT);
    their_addr.sin_addr.s_addr = inet_addr("**.**.**.**");
    memset(their_addr.sin_zero, 0, sizeof(their_addr.sin_zero));

    if(connect(sockfd,(struct sockaddr *)&their_addr,sizeof(struct sockaddr)) == -1) {
        perror("connect");
        return 0;
    }
    char str[1007];
    init(sockfd);
    thread taskRecv(recvMsg, sockfd);///将读和写分成两个线程来执行，在分得的时间片内可以看似并行的完成读写任务
    taskRecv.detach();///让这个线程不阻塞
    while(true) {
        scanf("%s", str);
        if(send(sockfd, str, strlen(str), 0) == -1) {
            perror("send()");
            break;
        }
    }
    closesocket(sockfd);
    return 0;
}
