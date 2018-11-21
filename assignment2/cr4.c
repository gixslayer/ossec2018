#include <stdio.h>

void main() {
	unsigned long long result;

	__asm__("movq %%cr4, %%rax\n" : "=a"(result));

	printf("Value of CR4 = %llx\n", result);
}
