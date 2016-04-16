#include <stdio.h>
#include <string.h>


int f1(char *s, size_t len)
{
	int i;

	int j;

	s[len] = 'a';
}

int f2()
{
	int ii;
	int i;
	char s[4];
	int j;
	int jj;

	i=0;
	j=0;
	memset(s, 0, 4);
	f1(s, 4);

	printf("i=%d, j=%d, s[0]=%c, s[1]=%c, s[2]=%c, s[3]=%c, s[4]=%c", i, j, s[0], s[1], s[2], s[3], s[4]);

	return 0;
}

int main()
{
	int k;
	char str[100];

	f2();

	return 0;
}
