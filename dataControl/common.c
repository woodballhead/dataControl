#include "common.h"
#include<stdio.h>
#include<stdlib.h>
void lemprintf(char* str) {
	printf(str);
}

void lemfree(char* str) {
	free(str);
}
/*
void lemfree(int * x) {
	free(x);
}
*/