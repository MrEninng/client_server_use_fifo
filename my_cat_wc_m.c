#include <stdio.h>
#include <string.h>

#define BUFF_SIZE 1024

int main(int argc, char **argv)
{	
	FILE *input = stdin;
	char buff[BUFF_SIZE];
	size_t str_len = 0;
	while (fgets(buff, BUFF_SIZE, input) != NULL) {
		str_len = strlen(buff) - 1;
		if (str_len == 0) continue; //  Пропускаем пустые строки
		printf ("%ld: %s", str_len, buff);
		str_len = 0;
	}
	
	return 0;
}

