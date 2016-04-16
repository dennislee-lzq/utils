#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TMP_STR "/tmp/XXXXXX"

int main()
{
	char file[sizeof(TMP_STR)];
	char *str;

	str = tmpnam(NULL);
	printf("%s\n", str);
	
	strcpy(file, TMP_STR);
	str = mkdtemp(file);

	printf("%s\n", str);
	pause();

	return 0;
}
