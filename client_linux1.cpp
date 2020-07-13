///一对一客户端程序
///linux环境的客户端
///一直只接受服务器发送的数据
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 1223/// 客户机连接远程主机的端口
#define MAXDATASIZE 100 /// 每次可以接收的最大字节
#define IP "0.0.0.0"
#define getLen(zero) sizeof(zero)/sizeof(zero[0])
using namespace std;

int main( ) {

    int sockfd, numbytes;
    char buf[MAXDATASIZE];///缓存接收内容
    struct sockaddr_in their_addr;///和my_addr用法差不多

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
        ///在客户端这里我们不需要绑定什么东西，因为只要向目的IP：端口发起连接请求

        perror("connect():");
        return 0;
    }
    while(1) {///循环接收
        if((numbytes=recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {///recv函数，套接字文件描述符，接收到这字符串里，最大长度
            perror("recv():");
            return 0;
        }
        buf[numbytes] = '\0';
        if(!strcmp(buf, "endS")) {///接收到endS两边一起结束
            break;
        }
        cout<<"Received: "<<buf<<endl;///输出接收的字符
    }
    close(sockfd);
    return 0;

}
