#include "../include/base.h"
#include "../include/util.h"

void print_cwd() {

	char cwd[CWD_LENGTH];
	int size = CWD_LENGTH;
	if (getcwd(cwd, size) != NULL) {
		printf("cwd = %s\n", cwd);
		return;
	}
	printf("print cwd error\n");
}
