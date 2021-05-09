#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

int main(void) {

	int sys_return = syscall(333);
	printf("System Call returned: %d",sys_return);
	return 0;
}



