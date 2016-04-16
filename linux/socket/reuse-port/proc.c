/*
 * Copyright(C) 2015 Ruijie Network. All rights reserved.
 */
/*
 * reuse_port.c
 * Original Author:  lizhiqiang@ruijie.com.cn, 2015-12-09
 *
 * �ļ�����˵��
 *
 * History
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>              /* ת�������ַ���͵�ͷ�ļ� */
#include <unistd.h>                 /* unix��׼ͷ�ļ� */
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/tcp.h>
#include <signal.h>                  /* �����źŵ�ͷ�ļ� */

#define SERVPORT                3333            /*�����������˿ں� */
#define BACKLOG                 10              /* ���ͬʱ���������� */
#define MAXDATASIZE             18              /* ÿ��������ݴ����� */
#define SELECT_TIMEOUT_SEC      5               /* �ձ����̼߳����������뼶ʱ�� */
#define SELECT_FAILE            -1              /* ����ʱ����socket���ִ��� */
#define SELECT_TIMEOUT          0               /* ������ʱ */
#define SEND_OR_RECV_FAILE      -1              /* ��socket�н��ջ���һ��������Ϣʧ�� */  
#define SUCCESS                 0               /* �ɹ� */

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
    
    struct sockaddr_in server_addr;     /* �洢�������Ϣ�Ľṹ��*/
    struct sockaddr_in client_addr;     /* �洢�ͻ�����Ϣ�Ľṹ��*/

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("create socket fail\n");
        return ;
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_in));/* ����ÿ���Ϊ�˽����8λ���0 */
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

    /* �󶨶˿� */
    if (bind(sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr)) == -1) {
        printf("Bind error: %s (%d)\n", strerror(errno), errno);
        return ;
    }
    
    /* ��socket���óɱ�������ģʽ�����ܹ������󶨶˿��ϵ����� 
     * ����(�����socketֻ�ܽ����������շ���Ϣ) 
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
            /* ���տͻ�����������������û�����󵽴�߳̽�����˯��״̬��ֱ�������󵽴� */
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

