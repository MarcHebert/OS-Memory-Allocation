//Marc Hebert
//260574038

#include <stdlib.h>
#include <stdio.h>//for print debugger statements

//constants, bounds, bookkeeping etc.
#define NAX_TOP_BLOCK_SIZE 128000 //bytes 128kb
unsigned long int topBlockSize = 0;

//policy
// 0 = first fit
// 1 or anything else = best fit

int policy = 0;// default is first fit



void *my_malloc(int size);

extern char *my_malloc_error;

void my_free(void *ptr);

//Specifies the memory allocation policy
void my_mallopt(int pol);
{
	if (pol != 0)
		policy = 1;
	else
		policy = 0;
}

//Prints memory allocation specifics
void my_mallinfo()
{
	//print
	//total number of bytes allocated
	//total free space
	//largest contiguous free space
	//other
}
