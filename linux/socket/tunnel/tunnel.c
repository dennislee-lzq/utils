#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <linux/if_tun.h>
#include <net/if.h>
#include <assert.h>

int tun_create(char *dev, int flags)
{
     struct ifreq ifr;
     int fd, err;
     
     assert(dev != NULL);
     if ((fd = open("/dev/net/tun", O_RDWR)) < 0)
         return fd;
     memset(&ifr, 0, sizeof(ifr));
     ifr.ifr_flags |= flags;
     if (*dev != '\0') {
         strncpy(ifr.ifr_name, dev, IFNAMSIZ);     
     }
     err=ioctl(fd, 1, (void *)&ifr);
     if (err < 0) {
   	    close(fd);
        return err;
    }
     // set_nonblock (fd);
     strcpy(dev,ifr.ifr_name);
     return fd;
}

#define TUN_NAME_SIZE 64
#define BUF_SIZE 1500

int main(int argc, char *argv[]) 
{
        int tun, ret; 
        char tun_name[TUN_NAME_SIZE]; 
        
        unsigned char buf[BUF_SIZE];
        tun_name[0] = '\0'; 
        tun = tun_create(tun_name, IFF_TUN | IFF_NO_PI); 
        if (tun < 0) {   
                perror("tun_create"); 
                return 1; 
        } 
        printf("TUN name is %s＼n", tun_name); 
        while (1) { 
                unsigned char ip[4]; 
                ret = read(tun, buf, sizeof(buf)); 
                if (ret < 0) 
                        break; 
                memcpy(ip, &buf[12], 4); 
                memcpy(&buf[12], &buf[16], 4); 
                memcpy(&buf[16], ip, 4); 
                buf[20] = 0; 
                *((unsigned short*)&buf[22]) += 8; 
                printf("read %d bytes＼n", ret); 
                ret = write(tun, buf, ret); 
                printf("write %d bytes＼n", ret); 
        } 
        return 0; 
}

#if 0
int main(int argc, char *argv[])
{
        char tun_name[IFNAMSIZ]="tun0";
  
        Tap=tun_create(tun_name, IFF_TAP | IFF_NO_PI);
        if (Tap< 0) {
                perror("tun_create");
                return 1;
                            }
        printf("TAP name is %s\n", tun_name);
        printf("Tap=%d\n",Tap);
        system("ifconfig tap0 10.0.0.185 netmask 255.255.0.0 up");
   
        Tap1=tun_create(tun_name1, IFF_TAP | IFF_NO_PI);
        if (Tap1< 0) {
                perror("tun_create");
                return 1;
                            }
        printf("TAP1 name is %s\n", tun_name1);
        printf("Tap1=%d\n",Tap1);
        system("ifconfig tap1 10.0.0.186 netmask 255.255.0.0 up");
}
#endif
