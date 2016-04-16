/*
 * Copyright(C) 2015 Ruijie Network. All rights reserved.
 */
/*
 * reuse_port.c
 * Original Author:  lizhiqiang@ruijie.com.cn, 2015-12-09
 *
 * 文件功能说明
 *
 * History
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>              /* 转换网络地址类型的头文件 */
#include <unistd.h>                 /* unix标准头文件 */
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/tcp.h>
#include <signal.h>                  /* 处理信号的头文件 */

#define SERVPORT                3333            /*服务器监听端口号 */
#define BACKLOG                 10              /* 最大同时连接请求数 */
#define MAXDATASIZE             18              /* 每次最大数据传输量 */
#define SELECT_TIMEOUT_SEC      5               /* 收报文线程监听的阻塞秒级时间 */
#define SELECT_FAILE            -1              /* 监听时发现socket出现错误 */
#define SELECT_TIMEOUT          0               /* 监听超时 */
#define SEND_OR_RECV_FAILE      -1              /* 从socket中接收或发送一个完整消息失败 */  
#define SUCCESS                 0               /* 成功 */

static void proc(void)
{
    int n;
    int sockfd;    
    int server_fd, sin_size;
    int maxfd; 
    fd_set rdset;
    char *buf = "hello, goodbye...";
    int ret;

    printf("enter pid %d\n", (int)getpid());
    
    struct sockaddr_in server_addr;     /* 存储服务端信息的结构体*/
    struct sockaddr_in client_addr;     /* 存储客户端信息的结构体*/

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("create socket fail\n");
        return ;
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_in));/* 这边置空是为了将最后8位填充0 */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVPORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int  reuseaddr = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &reuseaddr, sizeof(int)) == -1) {
        printf("setsockopt(SO_REUSEADDR)  failed\n");
        close(sockfd);
    }

#if 1
//#define SO_REUSEPORT 0x200
    int  reuseport = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (const void *) &reuseport, sizeof(int)) == -1) {
        printf("setsockopt(SO_REUSEPORT)  failed\n");
        close(sockfd);
    }
#endif

    /* 绑定端口 */
    if (bind(sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr)) == -1) {
        printf("Bind error: %s (%d)\n", strerror(errno), errno);
        return ;
    }
    
    /* 将socket设置成被动监听模式，才能够监听绑定端口上的连接 
     * 请求(否则该socket只能进行主动的收发消息) 
     */
    if (listen(sockfd, 3) == -1) {
        printf("Listen error\n");
        return ;
    }

    maxfd = sockfd + 1;

    sin_size = sizeof(struct sockaddr_in);
    while (1) {
        FD_ZERO(&rdset);
        FD_SET(sockfd, &rdset);
        ret = select(maxfd, &rdset, NULL, NULL, NULL);
        printf("pid %d select ret %d\n", (int)getpid(), ret);

        if (FD_ISSET(sockfd, &rdset)) {
            /* 接收客户的连接请求进，如果没有请求到达，线程将进入睡眠状态，直至有请求到达 */
            if ((server_fd = accept(sockfd, (struct sockaddr *)&client_addr, (uint32_t *)&sin_size)) 
                == -1) {
                printf("Accept error\n");
                continue;
            }

            printf("Server get connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port)); 
            n = send(server_fd, buf, strlen(buf), 0);
            //printf("send return n %d\n", n);
            close(server_fd);
            printf("pid %d accept new connection\n\n\n", (int)getpid());
        }
    }
    
}


int main(int argc, char *argv[])
{
    int worker = 1;
    int i;
    pid_t pid;
    
    if (argc == 2) {
        worker = atoi(argv[1]);
        printf("worker set to %d\n", worker);
    }
    
    for (i = 0; i < worker; i++) {
        pid = fork();
        switch (pid) {
        case 0:
            /* child */
            proc();
            return;
        case -1:
            printf("create process %d fail\n", i);
            break;
        default:
            printf("child pid %d\n", (int)pid);
            break;
        }
    }
    
    pause();
}

