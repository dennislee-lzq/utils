

#include <stdio.h>
#include <string.h>
#include <pcre.h>
#include <time.h>
#include "list.h"

#define OVECCOUNT 30 /* should be a multiple of 3 */
#define EBUFLEN 128
#define BUFLEN 1024

#define USER_NAME_LEN 32
#define TIME_STR_LEN  sizeof("2015-01-13 22:54:51")


typedef struct {
    char username[USER_NAME_LEN];
    int  no_entry;
    time_t online_total;
    struct list_head entry_header;  /* for entry list */
    struct list_head lst;           /* for user list */
} user_node_t;

typedef struct {
    char login_time[TIME_STR_LEN];
    char logout_time[TIME_STR_LEN];
    char kickout_time[TIME_STR_LEN];
    struct list_head lst;
} entry_t;

#define ACTION_LOGIN  1
#define ACTION_LOGOUT 2
#define ACTION_KICKOUT 3

typedef struct {
	char username[USER_NAME_LEN];
	char timestr[TIME_STR_LEN];
	int  action;	
} parser_t;


static void do_compile(void);
static int do_search(char *str, size_t len, parser_t *p);
static int do_search_single(char *str, size_t len, pcre *re, int action, parser_t *p);
static void process_line(char *line, size_t len);
static user_node_t *get_user_node(char *username);
static user_node_t *create_user_node(char *username);
static entry_t *create_entry(user_node_t *un);
static entry_t *get_last_entry(user_node_t *un);
static void show_result(void);
static void show_user_detail(user_node_t *un);
static void count_online_time(user_node_t *un);

static LIST_HEAD(g_user_header);
/* 1: timestamp; 2: username  */
static char  pattern_login[] =   "info - user - (.*) - 用户 \"(.*)\" .*登录成功!\r\n";
/* 1: timestamp; 2: username  */
static char  pattern_logout[] =  "info - user - (.*) - 用户 \"(.*)\" .*注销登录\r\n";
/* 1: timestamp; 2: username  */
static char  pattern_kickout[] = "info - user - (.*) - 用户 \"(.*)\" .*强制下线\r\n";

static pcre *re_login;
static pcre *re_logout;
static pcre *re_kickout;

static void do_compile(void)
{
	const char *error;
    int  erroffset;
	
	re_login = pcre_compile(pattern_login, 0, &error, &erroffset, NULL);
	re_logout = pcre_compile(pattern_logout, 0, &error, &erroffset, NULL);
	re_kickout = pcre_compile(pattern_kickout, 0, &error, &erroffset, NULL);
	
	if (re_login == NULL || re_logout == NULL || pattern_kickout == NULL) {
		printf("do compile fail\n");
	}
}

static int do_search(char *str, size_t len, parser_t *p)
{
	if (do_search_single(str, len, re_login, ACTION_LOGIN, p) == 0
		|| do_search_single(str, len, re_logout, ACTION_LOGOUT, p) == 0
		|| do_search_single(str, len, re_kickout, ACTION_KICKOUT, p) == 0) {
		return 0;
	} 
	
	//printf("log str %.*s no match, ignore\n", len, str);
	
    return -1;
}

static int do_search_single(char *str, size_t len, pcre *re, int action, parser_t *p)
{
	int  ovector[OVECCOUNT];
    int  rc;
	char *timestr;
	int time_len;
	char *name;
	int name_len;
	
	rc = pcre_exec(re, NULL, str, strlen(str), 0, 0, ovector, OVECCOUNT);
    #if 0
    int i;
    if (rc > 0) {
        for (i = 0; i < rc; i++) {
            timestr = str + ovector[2 * i];
	        time_len = ovector[2 * i + 1] - ovector[2 * i];
            printf("macth %d: %.*s\n", i, time_len,timestr);
        }
    } else {
        printf("pcre exec fail, rc is %d, action is %d\n", rc, action);
        if (rc == PCRE_ERROR_NOMATCH) {
			printf("Sorry, no match ...\n");
		} else {
			printf("Matching error %d\n", rc);
		}
    }
    #else
    if (rc != 3) {
		//printf("pcre exec fail, rc is %d, action is %d\n", rc, action);
        if (rc == PCRE_ERROR_NOMATCH) {
			//printf("Sorry, no match ...\n");
		} else {
			//printf("Matching error %d\n", rc);
		}
        return -1;
    }
	
    //printf("OK, has matched, rc =%d ...\n", rc);
	p->action = action;
	timestr = str + ovector[2 * 1];
	time_len = ovector[2 * 1 + 1] - ovector[2 * 1];
	memcpy(p->timestr, timestr, time_len);
	
	name = str + ovector[2 * 2];
	name_len = ovector[2 * 2 + 1] - ovector[2 * 2];
	memcpy(p->username, name, name_len);
	#endif
	return 0;
}

static void process_line(char *line, size_t len)
{	
	parser_t p;
	user_node_t *un;
    entry_t *ent;
	
	memset(&p, 0, sizeof(parser_t));
	if (do_search(line, len, &p) == 0) {
		un = get_user_node(p.username);
		if (un == NULL) {
			un = create_user_node(p.username);
            if (un == NULL) {
                return;
            }
		}

        switch (p.action) {
        case ACTION_LOGIN:
            ent = create_entry(un);
            if (ent) {
                memcpy(ent->login_time, p.timestr, TIME_STR_LEN);
                un->no_entry++;
            }
            break;
        case ACTION_LOGOUT:
            ent = get_last_entry(un);
            if (ent) {
                memcpy(ent->logout_time, p.timestr, TIME_STR_LEN);
            }
            break;
        case ACTION_KICKOUT:
            ent = get_last_entry(un);
            if (ent) {
                memcpy(ent->kickout_time, p.timestr, TIME_STR_LEN);
            }
            break;
        default:
            printf("Unknown action\n");
            break;
        }
	}

    return;
}

static user_node_t *get_user_node(char *username)
{
	user_node_t *un;
	
	list_for_each_entry(un, &g_user_header, lst) {
		if (memcmp(un->username, username, USER_NAME_LEN) == 0) {
			return un;
		}
	}
	
	return NULL;
}

static user_node_t *create_user_node(char *username)
{
	user_node_t *un;
	
	un = malloc(sizeof(user_node_t));
	if (un) {
		memset(un, 0, sizeof(user_node_t));
		memcpy(un->username, username, USER_NAME_LEN);
		INIT_LIST_HEAD(&un->entry_header);
        list_add_tail(&un->lst, &g_user_header);
	}

    return un;
}

static entry_t *create_entry(user_node_t *un)
{
    entry_t *ent;
    
    ent = malloc(sizeof(entry_t));
    if (ent == NULL) {
        return NULL;
    } 
    list_add_tail(&ent->lst, &un->entry_header);
    
    return ent;
}
	
static entry_t *get_last_entry(user_node_t *un)
{
    entry_t *ent;
	
	if (list_empty(&un->entry_header)) {
		return NULL;
	}
    
    //ent = list_entry(&(un->entry_header.prev), entry_t, lst);
	ent = list_last_entry(&un->entry_header, entry_t, lst);
	
	return ent;
}

static void show_result(void)
{
    user_node_t *un;

    list_for_each_entry(un, &g_user_header, lst) {
        count_online_time(un);
        printf("%s, %d, %.2f\n", un->username, un->no_entry, (float)un->online_total / 3600);
        //show_user_detail(un);
        //printf("\n");
    }
}

static void show_user_detail(user_node_t *un)
{
    entry_t *ent;
    
    printf("Username: %s, num of entry: %d\n", un->username, un->no_entry);
    printf("%24s -- %24s -- %24s \n", "login time", "logout time", "kickout time");
    list_for_each_entry(ent, &un->entry_header, lst) {
        printf("%24s -- %24s -- %24s \n", ent->login_time, ent->logout_time, ent->kickout_time);
    }
}

static void count_online_time(user_node_t *un)
{
    entry_t *ent;
    struct tm tmp;
    time_t login_sec;
    time_t logout_sec;

    list_for_each_entry(ent, &un->entry_header, lst) {
        if (ent->login_time[0] == 0 
            || (ent->logout_time[0] == 0 && ent->kickout_time[0] == 0)) {
            continue;
        }
        
        sscanf(ent->login_time, "%d-%d-%d %d:%d:%d", &tmp.tm_year, &tmp.tm_mon, &tmp.tm_mday, 
            &tmp.tm_hour, &tmp.tm_min, &tmp.tm_sec);
        //printf("%d-%d-%d %d:%d:%d\n", tmp.tm_year, tmp.tm_mon, tmp.tm_mday, 
        //    tmp.tm_hour, tmp.tm_min, tmp.tm_sec);
        tmp.tm_year -=1900;
        login_sec = mktime(&tmp);
        //printf("login sec:%d\n", (int)login_sec);
        
        if (ent->logout_time[0] != 0) {
            sscanf(ent->logout_time, "%d-%d-%d %d:%d:%d", &tmp.tm_year, &tmp.tm_mon, &tmp.tm_mday, 
                &tmp.tm_hour, &tmp.tm_min, &tmp.tm_sec);
        } else if (ent->kickout_time[0] != 0) {
            sscanf(ent->kickout_time, "%d-%d-%d %d:%d:%d", &tmp.tm_year, &tmp.tm_mon, &tmp.tm_mday, 
                &tmp.tm_hour, &tmp.tm_min, &tmp.tm_sec);
        } else {
            printf("Invalid entry\n");
            continue;
        }
        tmp.tm_year -=1900;
        logout_sec = mktime(&tmp);
        un->online_total += (logout_sec - login_sec);
    }
}

int main(int argc, char *argv[])
{
	FILE *file;
	char *line;
	size_t len = 0;
	ssize_t read;

	if (argc != 2) {
		printf("Usage: %s filename\n", argv[0]);
		exit(-1);
	}
	
    if ((file = fopen(argv[1], "r")) == NULL) {
        printf("Open log file fail\n");
        exit(-1);
    }
	
	do_compile();
	
	#if 1
	while ((read = getline(&line, &len, file)) != -1) {
        //printf("Retrieved line of length %zu: ", read);
        //printf("%s\n", line);
		process_line(line, len);
    }
	#else
    parser_t p;

	char *str = "info - user - 2014-12-06 11:02:57 - 用户 \"wangshengming\" 使用 PC终端 从IP 221.205.17.252 登录成功!\r\n";

    printf("str:%s\n", str);
    printf("patten:%s\n", pattern_login);
    do_search_single(str, strlen(str), re_login, 1, &p);
	#endif
    show_result();
    
    return 0;
}
