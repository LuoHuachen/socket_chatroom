///采用epoll实现客户端
#include <set>
#include <vector>
#include <string>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#define LISNUM 10
#define MYPORT 1223
#define MAXLEN 1007
#define MYIP "0.0.0.0"
using namespace std;

set<int> socketset;///维护现有的socketfd，用于全发送
vector<int> deletefd;///存储异常的文件描述符

int socketBind( );///socket()、bind()

void doEpoll(int &sockfd);

void handleEvents(int &epollfd, struct epoll_event *events, int &num, int &sockfd, char *buffer);

void handleAccept(int &epollfd, int &sockfd);

void handleRecv(int &epollfd, int &sockfd, char *buffer);

void allSend(char buffer[], int &nowfd, int &epollfd);

void handleSend(int &epollfd, int &sockfd, char *buffer);

void addEvent(int &epollfd, int &sockfd, int state);

void deleteEvent(int &epollfd, int sockfd, int state);

void modifyEvent(int &epollfd, int &sockfd, int state);

int main( ) {
    int sockfd = socketBind( );
    if(listen(sockfd, LISNUM) == -1) {
        perror("listen()");
        return 0;
    }
    cout<<"listen ok"<<endl;
    doEpoll(sockfd);
    return 0;
}

int socketBind( ){///socket()、bind()
    int sockfd;
    struct sockaddr_in my_addr;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket()");
        exit(1);
    }
    cout<<"socket ok"<<endl;
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(MYPORT);
    my_addr.sin_addr.s_addr = inet_addr(MYIP);

    memset(my_addr.sin_zero, 0, sizeof(my_addr.sin_zero));

    if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind()");
        exit(1);
    }
    puts("bind ok");
    return sockfd;
}

void doEpoll(int &sockfd) {
    int epollfd = epoll_create(LISNUM);///创建好一个epoll后会产生一个fd值
    struct epoll_event events[LISNUM];


    int ret;
    char buffer[MAXLEN]={};
    addEvent(epollfd, sockfd, EPOLLIN);///对sockfd这个连接，我们关心的是是否有客户端要连接他，所以说要将读事件设为关心

    while(true) {///持续执行
        ret = epoll_wait(epollfd, events, LISNUM, -1);
        handleEvents(epollfd, events, ret, sockfd, buffer);///对得到的事件进行处理
    }
    close(epollfd);
}

void handleEvents(int &epollfd, struct epoll_event *events, int &num, int &sockfd, char *buffer){
    int listenfd;
    for(int i = 0; i < num; ++i) {
        listenfd = events[i].data.fd;
        if((listenfd == sockfd)&&(events[i].events & EPOLLIN)) {
            handleAccept(epollfd, sockfd);///处理客户端连接请求
        } else if(events[i].events & EPOLLIN) {
            handleRecv(epollfd, listenfd, buffer);///处理客户端发送的信息

        }
    }
}

void handleAccept(int &epollfd, int &sockfd) {
    int clientfd;
    struct sockaddr_in clientaddr;
    socklen_t clientaddrlen = 1;
    if((clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &clientaddrlen)) == -1) {
        perror("accept()");
    } else {
        socketset.insert(clientfd);
        addEvent(epollfd, clientfd, EPOLLIN);///处理连接，我们关心这个连接的读事件
    }
}

void handleRecv(int &epollfd, int &sockfd, char *buffer) {
    int len = recv(sockfd, buffer, MAXLEN, 0);
    if(len <= 0) {
        perror("recv()");
        socketset.erase(sockfd);
        deleteEvent(epollfd, sockfd, EPOLLIN);
    } else {
        cout<<buffer<<endl;
        allSend(buffer, sockfd, epollfd);///成功接收到一个字符串就转发给全部客户端
    }
}

void allSend(char buffer[], int &nowfd, int &epollfd) {
    ///modifyEvent(epollfd, nowfd, EPOLLOUT);
    if(buffer[0] == '\0')
        return ;
    for(auto it = socketset.begin(); it != socketset.end() ; ++ it) {
        if(*it != nowfd){
            cout<<"__"<<buffer<<"________"<<endl;
            if(send(*it, buffer, strlen(buffer), 0) == -1) {
                perror("send()");
                deletefd.push_back(*it);///直接erase会导致迭代器失效

                deleteEvent(epollfd, *it, EPOLLIN);
            }
        }
    }
    for(size_t i = 0; i < deletefd.size(); ++i) { ///单独删除
        socketset.erase(deletefd[i]);
    }
    deletefd.clear();
    ///modifyEvent(epollfd, nowfd, EPOLLIN);
    memset(buffer, 0, MAXLEN);
}

void addEvent(int &epollfd, int &sockfd, int state) {
    struct epoll_event ev;
    ev.events=state;
    ev.data.fd = sockfd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev);
}

void deleteEvent(int &epollfd, int sockfd, int state) {
    struct epoll_event ev;
    close(sockfd);
    ev.events=state;
    ev.data.fd = sockfd;
    epoll_ctl(epollfd, EPOLL_CTL_DEL, sockfd, &ev);
}

void modifyEvent(int &epollfd, int &sockfd, int state) {
    struct epoll_event ev;
    ev.events=state;
    ev.data.fd = sockfd;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, sockfd, &ev);
}
