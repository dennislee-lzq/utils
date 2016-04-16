#include <jansson.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    char    *key;
    union {
        json_t  *json;  /* for JSON_OBJECT, JSON_ARRAY, JSON_TRUE, JSON_FALSE and JSON_NULL*/
        char    *str;   /* for JSON_STRING */
        json_int_t val; /* for JSON_INTEGER */
        double  real;   /* JSON_REAL */
    };
} ips_json_t;

ips_json_t ips_json;

char *netid[] = {"192.168.1.0/24", "192.168.2.0/24"};
char *json_buf = NULL;

int malloc_times = 0;
int free_times = 0;

void *my_malloc(size_t len)
{
    void *p = malloc(len);

    if (p) {
        malloc_times++;
    }
    return p;
}

void my_free(void *p)
{
    free_times++;
    free(p);
}

void json_dump_error(json_error_t *error)
{
    printf("line:%d, column:%d, position:%d\n", error->line, error->column, error->position);
    printf("source:%s, text:%s\n", error->source, error->text);
}

/*
 * {"mode":"ips", 
 *  "interface":"eth0", 
 *  "home_net":[{"netid", "192.168.1.0/24"},
 *              {"netid", "192.168.2.0/24"}]
 *  }                                             ] 
 */
int json_encode(void)
{
	json_t *root;
    json_t *home_net;
    json_t *obj;
    int i;
	char *buf;
    
	/* ----STEP1: 创建json对象---- */
	root = json_object();
	if (root == NULL) {
		printf("json_object error\n");
		return -1;
	}

	/* ---STEP2: 为JSON对象填充内容--- */

    /* 增加mode:ips */
    if (json_object_set_new(root, "mode", json_string("ips")) == -1) {
        printf("add mode error\n");
        return -1;
    }
    
    /* 增加interface:eth0 */
    if (json_object_set_new(root, "interface", json_string("eth0")) == -1 ) {
        printf("add interface error\n");
        return -1;
    }

    if (json_object_set_new(root, "int", json_integer(100)) == -1 ) {
        return -1;
    }
    if (json_object_set_new(root, "real", json_real(200.22)) == -1 ) {
        return -1;
    }
    if (json_object_set_new(root, "true", json_true()) == -1 ) {
        return -1;
    }
    if (json_object_set_new(root, "flase", json_false()) == -1 ) {
        return -1;
    }
    if (json_object_set_new(root, "null", json_null()) == -1 ) {
        return -1;
    }
    
    /* 增加home_net数组 */
    if (json_object_set_new(root, "home_net", json_array()) == -1 ) {
        printf("add home_net error\n");
        return -1;
    }
    /* 为home_net填充数据 */
    home_net = json_object_get(root, "home_net");
    //printf("home_net ref:%d\n", home_net->refcount);
    if (home_net == NULL) {
        printf("json_array error");
        return -1;
    }
    for (i = 0; i < sizeof(netid)/sizeof(char *); i++) {
        /* XXX: MISS ERROR HANDLING */
        obj = json_object();
        json_object_set_new(obj, "netid", json_string(netid[i]));
        json_array_append(home_net, obj);
        json_decref(obj);
    }
    
    /* ---STEP3:将JSON格式编码--- */
    /* json格式存储到buf中，函数内部会申请内存 */
	buf = json_dumps(root, 0);
	if (buf == NULL) {
		printf("json_dump error\n");
		return -1;
	}
	printf("%s\n", buf);
    json_buf = buf;
    /* json格式写入文件 ips.json */
    if (json_dump_file(root, "ips.json", 0) == -1) {
        printf("json_dump_file error");
        return -1;
    }

    /* ---STEP4: 清理操作--- */
    json_decref(home_net);
	json_decref(root);
    
	return 0;
}

int json_decode(void)
{
    json_t *root_str, *root_file;
    json_error_t error;
    json_t *tmp;

    /* 从buffer load json */
    root_str = json_loads(json_buf, 0, &error);
    //root_str = json_loadb(json_buf, strlen(json_buf), 0, &error);
    if (root_str == NULL) {
        json_dump_error(&error);
        return -1;
    }
    /* 从文件 load json */
    root_file = json_load_file("ips.json", 0, &error);
    if (root_file == NULL) {
        json_dump_error(&error);
        return -1;
    }

    /* 检查下json是否一样，因为我们encode的时候是一样的 */
    if (!json_equal(root_str, root_file)) {
        printf("root str not equal root fsile!!!\n");
        return -1;
    }

    /* 判定json类型 */
    if (!json_is_object(root_str)) {
        printf("json is not obj!\n");
        return -1;
    }

    /* 遍历打印json的内容 */
    tmp = json_object_get(root_str, "mode");
    if (tmp && json_is_string(tmp)) {
        //printf("mode ref:%d\n", tmp->refcount);
        printf("get mode:%s\n", json_string_value(tmp));
    }
    tmp = json_object_get(root_str, "interface");
    if (tmp && json_is_string(tmp)) {
        printf("get interface:%s\n", json_string_value(tmp));
    }
    tmp = json_object_get(root_str, "home_net");
    if (tmp && json_is_array(tmp)) {
        size_t index;
        json_t *value;
        json_array_foreach(tmp, index, value) {
            printf("get netid:%s\n", json_string_value(json_object_get(value, "netid")));
        }
    }

    tmp = json_object_get(root_str, "not_exsit");
    if (!tmp) {
        printf("not_exsit!\n");
    }

    /* 清理操作 */
    json_decref(root_str);
    json_decref(root_file);
    

    return 0;
}

void decode_test()
{
	char *buf = "{'timestamp' : \"2015-04-24 23:22:33\"}";
	int len = strlen(buf);
	json_t *j;
	json_error_t error;

	printf("buf is :%s\n", buf);
	printf("len is :%d\n", len);

	j = json_loadb(buf, len, 0, &error);
	if (j == NULL) {
		json_dump_error(&error);
	}
}

int main()
{
    json_set_alloc_funcs(my_malloc, my_free);

	decode_test();

	exit(1);

    json_buf = NULL;
    json_encode();
    json_decode();
    if (json_buf) {
        my_free(json_buf);
    }
    printf("malloc:%d, free:%d\n", malloc_times, free_times);

    ips_json.key = "hello";
    ips_json.str = "mimi";

    printf("str:%s\n", ips_json.str);
    
    return 0;
}
