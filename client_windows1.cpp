///Windows客户端程序
#include <iostream>
#include <stdlib.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
///在codeblocks下不可以运行这些是因为这个libws2_32.a找不到
///解决方法：Settings->compiler->Global compiler settings->(找到)Linker settings(横着排开的目录)->Add->去MinGW/lib找到libws2_32.a就可以了


#define PORT 1223/// 客户机连接远程主机的端口
#define MAXDATASIZE 100 /// 每次可以接收的最大字节
using namespace std;

int main( ) {
    WORD sockVersion = MAKEWORD(2,2);
    WSADATA wsaData;
    if(WSAStartup(sockVersion, &wsaData)!=0){
        return 0;
    }
    ///windows环境下的Winsock是要初始化的；即：固定代码。

    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct hostent *he;
    struct sockaddr_in their_addr;

    puts("USER:");
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        cout<<WSAGetLastError()<<endl;///这个可以输出WSAError号
        perror("socket");
        return 0;
    }

    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(PORT);

    their_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    memset(their_addr.sin_zero, 0, sizeof(their_addr.sin_zero));
    if(connect(sockfd,(struct sockaddr *)&their_addr,sizeof(struct sockaddr)) == -1) {
        perror("connect");
        return 0;
    }
    while(1) {
        if((numbytes=recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
            perror("recv");
            return 0;
        }
        buf[numbytes] = '\0';
        if(!strcmp(buf, "endS")) {
            break;
        }
        cout<<"Received: "<<buf<<endl;
    }
    closesocket(sockfd);///函数不同
    return 0;
}
