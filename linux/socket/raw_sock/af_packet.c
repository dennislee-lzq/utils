
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <linux/filter.h>
#include <sys/mman.h>

#define MSG_MAX_LEN 8192
#define MSG_DEF_LEN 128

#define printk printf

int main(void)
{
    char buffer[MSG_MAX_LEN], con_buf[MSG_DEF_LEN], name_buf[MSG_DEF_LEN];
    struct timeval timev;
    struct iovec  io;
    struct msghdr msg;
    int ret, len, max;
    fd_set readfds;
	int fd;

	fd =  socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (fd < 0) {
        printk("failed to create socket, return: %d, err: %s\n", fd, strerror(errno));
        return -1;
    }
	struct sockaddr_ll sll; // 注意结构体是sockaddr_ll
	memset( &sll, 0, sizeof(sll) );
	sll.sll_family = AF_PACKET;
	struct ifreq ifstruct;
	strcpy(ifstruct.ifr_name, "eth0");
	ioctl(fd, SIOCGIFINDEX, &ifstruct); //控制I/O设备
	sll.sll_ifindex = ifstruct.ifr_ifindex;
	sll.sll_protocol = htons(ETH_P_ALL);
	if(bind(fd, (struct sockaddr *) &sll, sizeof(sll)) == -1 ) { //在这里当然仍然需要绑定的
		printk("failed to bind socket, return: %d, err: %s\n", fd, strerror(errno));
        return -1;
	}

    timev.tv_sec = 20;
    timev.tv_usec = 0;
    printk("start select\n");

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
        max = fd + 1;

        ret = select(max, &readfds, NULL, NULL, NULL);
        if (ret <= 0) {
            printk("select return: %d\n", ret);
            continue;
        }

        if (!FD_ISSET(fd, &readfds)) {
            printk("is not fdset\n");
            continue;
        }
		
		printk("select ret:%d\n", ret);
		//sleep(1);

        io.iov_base = buffer;
        io.iov_len = MSG_MAX_LEN;

        msg.msg_iov = &io;
        msg.msg_iovlen = 1;
        msg.msg_control = con_buf;
        msg.msg_controllen = MSG_DEF_LEN;
        msg.msg_name = name_buf;
        msg.msg_namelen = MSG_DEF_LEN;

        ret = recvmsg(fd, &msg, 0);
        if (ret > 0) {
				//if (ret > 600)
				printk("packet ret:%d\n", ret);
                len = sendmsg(fd, &msg, 0);
        } else {
            printk("<error> recvmsg return: %d\n", ret);
        }
    } /* end of while(1) */

	
	return 0;
}
