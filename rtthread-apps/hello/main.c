#include <stdio.h>
int main(int argc, char *argv[])
{
    printf("Hello, world\n");
	sdram_init();
	sdram_test();

    return 0;
}

