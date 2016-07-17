#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int result;

	opterr = 0;

	while ((result=getopt(argc, argv, "ab:c::")) != -1) {
		switch(result) {
		case 'a':
			printf("option=a, optopt=%c, optarg=%s\n", optopt, optarg);
			break;
		case 'b':
			printf("option=b, optopt=%c, optarg=%s\n", optopt, optarg);
			break;
		case 'c':
			printf("option=c, optopt=%c, optarg=%s\n", optopt, optarg);
			break;
		default:
			printf("Unknown arg\n");
			break;
		}
	}

	return 0;
}
