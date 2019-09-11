#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	uint8_t(*handler)(void*);
	uint8_t args[16];
} vector;


uint8_t print_stuff(char *string) {
	for (uint8_t i = 0; i < 8; i++)
		printf("%X", (uint8_t)string[i]);
	puts("");
	char *res = *(uint64_t*)string;
	printf("From print_stuff, string is at %p, points to %p\n", string, *string);
	puts(res);
	return 0;
}

struct test {
	char *string;
};

uint8_t interrupt(vector v) {
	return v.handler(v.args);
}

int main(int argc, char **argv) {
	char *testr = malloc(40);
	memcpy(testr, "testing this makes me happy.", 29);
	printf("Global string is at %p\n", testr);
	struct test machin = { .string = testr };
	printf("From main, string is at %p\n", machin.string);
	printf("%d\n", sizeof(vector));
	vector v;
	v.handler = print_stuff;
	*(struct test*)&v.args = machin;
	printf("In vector, string is at %p\n", (void*)v.args);
	uint8_t res = interrupt(v);
	printf("%d\n", res);
	return 0;
}
