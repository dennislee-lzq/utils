#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
 
//#define EVENT_SIZE(sizeof(struct inotify_event))
//#define BUF_LEN( 1024 * ( EVENT_SIZE + 16 ))
 
int main(int argc, char **argv) {
    int event_size = sizeof(struct inotify_event);
    int buf_len = 1024 * ( event_size + 16 );
    int length, i = 0;
    int fd;
    int wd;
	char buffer[buf_len];
    
	fd = inotify_init();
    if(fd < 0) {
        perror("inotify_init");
    }
	printf("fd:%d\n", fd);
 
    wd = inotify_add_watch(fd, "test_dir", IN_MODIFY | IN_CREATE | IN_DELETE | IN_OPEN | IN_CLOSE);
    printf("wd:%d\n", wd);
	while(1) {
        i = 0;
        printf("range: \n");
        length = read(fd, buffer, buf_len);
        printf("length: %d\n", length);
 
        if(length < 0) {
            perror("read");
        }
 
        while(i < length) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            if(event->len) {
                if(event->mask & IN_CREATE) {
                    if(event->mask & IN_ISDIR) {
                        printf("The directory %s was created. \n", event->name);
                    } else {
                        printf("The file %s was created. \n", event->name);
                    }
                } else if(event->mask & IN_DELETE) {
                    if(event->mask & IN_ISDIR) {
                        printf("The directory %s was delete. \n", event->name);
                    } else {
                        printf("The file %s was delete. \n", event->name);
                    }
                } else if(event->mask & IN_MODIFY) {
                    if(event->mask & IN_ISDIR) {
                        printf("The directory %s was modify. \n", event->name);
                    } else {
                        printf("The file %s was modify. \n", event->name);
                    }
                } else if(event->mask & IN_OPEN) {
                    if(event->mask & IN_ISDIR) {
                        printf("The directory %s was open. \n", event->name);
                    } else {
                        printf("The file %s was open. \n", event->name);
                    }
                } else if(event->mask & IN_CLOSE) {
                    if(event->mask & IN_ISDIR) {
                        printf("The directory %s was close. \n", event->name);
                    } else {
                        printf("The file %s was close. \n", event->name);
                    }
                }
            }
            i += event_size + event->len;
        }
    }
    (void) inotify_rm_watch(fd, wd);
    (void) close(fd);
    exit(0);
}
