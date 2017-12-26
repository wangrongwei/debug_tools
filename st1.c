/*
 * 内核调试使用 strace
 *
 */

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	//int fd;
	//printf("we want to open a file\n");
	while(1)
	{
		int fd;
		fd = fopen("etc/dev/media0","r");
		//printf("we want to open a file\n");
		if(fd == NULL){
			printf("Error!\n");
			//return EXIT_SUCCESS;

		}
		else
			close(fd);
		sleep(3);
	}
	return EXIT_SUCCESS;
}




