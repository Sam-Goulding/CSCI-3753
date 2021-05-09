#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

int main(void) {

	int result = 0;
	int *p = &result;
	int sys_return = syscall(334,5,8,p);
	printf("\nSystem Call returned: %d\n",sys_return);
	printf("Result of syscall: %d\n", result);
	return 0;
}
