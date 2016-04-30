#include <stdio.h>
#include <string.h> 

#define BUFF_SIZE 1024

int main(int argc, char **argv)
{
	char buff[BUFF_SIZE];
	int str_len = 0;
	while (fgets(buff, BUFF_SIZE, stdin) != NULL) {
		str_len = strlen(buff) - 1;
		if (str_len == 0) continue; //  Пропускаем пустые строки
		for (int i = 0; i < str_len; ++i) {
			printf("%c ", buff[i]);
		}
		printf("\n");
		str_len = 0;
	}
	return 0;
}

