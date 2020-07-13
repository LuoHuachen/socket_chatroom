///linux环境的服务端,
///实现套接字对同一主机的通讯
///循环输入发送至客户端
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MYPORT 1223///开应一个端口
#define IP "0.0.0.0"///服务器的IPv4地址
#define BACKLOG 10
#define getLen(zero) sizeof(zero) / sizeof(zero[0]) ///得到数组最大大小
using namespace std;

int main() {
    int sockfd, new_fd;
    struct sockaddr_in my_addr;
    puts("SERVER:");
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
        ///socket()函数发生错误则返回-1，否则会返回套接字文件描述符
        ///对于int socket(int domain, int type, int protocol);中的参数想要详细了解可以看这篇博客：https://blog.csdn.net/liuxingen/article/details/44995467

        perror("socket():");///显示错误
        return 0;
    }
    my_addr.sin_family = AF_INET;///通讯在IPv4网络通信范围内
    my_addr.sin_port = htons(MYPORT);///我的端口
    my_addr.sin_addr.s_addr = inet_addr(IP);///用来得到一个32位的IPv4地址，inet_addr将"127.0.0.1"转换成s_addr的无符号整型。
    bzero(&(my_addr.sin_zero), getLen(my_addr.sin_zero));///sin_zero是为了让sockaddr与sockaddr_in两个数据结构保持大小相同而保留的空字节。

    if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {///bind()函数将套接字与该IP：端口绑定起来。
        perror("bind():");
        return 0;
    }
    if(listen(sockfd, BACKLOG) == -1) {///启动监听，等待接入请求，BACKLOG是在进入队列中允许的连接数目
        perror("listen():");
        return 0;
    }

    socklen_t sin_size;
    struct sockaddr_in their_addr;
    if((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1) {
        ///当你监听到一个来自客户端的connect请求时，要选择是将他放在请求队列里还是允许其连接，这里写的其实是单进客户的，所以说无等待。
        ///这个函数还返回了一个新的套接字，用于与该进程通讯。
        perror("accept():");
        return 0;
    }
    printf("server: got connection from %s\n", inet_ntoa(their_addr.sin_addr));///inet_ntoa可以将inet_addr函数得到的无符号整型转为字符串IP

    char str[1007];

    while(1) {///循环发送 以endS结束与这一进程的通讯，endS也作为客户端停止工作的标志送出
        puts("send:");
        scanf("%s", str);
        if(send(new_fd, str, strlen(str), 0) == -1) {
            ///send()函数，new_fd是accept返回的套接字文件描述符，str就你要发送的数据，数据长度，对于最后一位flag
            /// flags取值有：
            /// 0： 与write()无异

            perror("send():");
            close(new_fd);///发送失败就关闭该通讯
            return 0;
        }
        if(!strcmp("endS", str))
            break;
    }
    close(new_fd);///正常结束要关闭这些已建立的套接字
    close(sockfd);

    return 0;
}
