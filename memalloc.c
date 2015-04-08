//Marc Hebert
//260574038

#include <stdlib.h>
#include <stdio.h>//for print debugger statements
#include <stdint.h>// int pointer type

/*block structure

if free block
	[
	+9		prevFreeBlock unsigned int (4 bytes)
	+5 		nextFreeBlock unsigned int (4 bytes)
	]
else
	+5		main content
tagging scheme
	+1		blockSize unsigned int (4 bytes)
 blckaddr	isFree (1 if free, 0 otherwise) char (1 byte)
	-1 			isPrevFree char	(1 byte)
	-5 			prevBlockSize unsigned int (4 bytes)
*/	

#define MAX_BLOCK_SIZE 128000 //bytes 128kb

//boundary tag scheme
#define SIZE_OFFSET 1
#define FREE_OFFSET 0
#define PREV_SIZE_OFFSET -5
#define PREV_FREE_OFFSET -1
#define TAG_SIZE 10

//constants, bounds, bookkeeping etc.

	//error handling
	extern char *my_malloc_error;

	/*key
		P = pointer arithmetic
	*/

#define TRUE 1
#define FALSE 0

int init  = FALSE;
unsigned long int topBlockSize = 0;
unsigned int totalNumBytesAlloc = 0;
unsigned int totalFreeSpace = MAX_BLOCK_SIZE;
unsigned int largestContFreeSpace = MAX_BLOCK_SIZE;
unsigned int numBlocks = 1;
unsigned int numFreeBlocks = 1;

unsigned int progBreak = 0;
unsigned int progEnd = 0;
void* headFreeBlock = NULL;

//policy
#define FIRST_FIT 0 
#define BEST_FIT 1
int policy = FIRST_FIT;// default is first fit

//accessor functions
int getBlockSize(void* bk)
{
	return *(int*) (bk + SIZE_OFFSET);
}

int getIsFree(void* bk)
{
	return *(int*) (bk);
}

void* getNextBlock(void* bk)
{
	//errorhandling
	int nextBlock = (uintptr_t)bk + getBlockSize(bk);
	if (nextBlock >=  sbrk(0))
	{
		if(nextBlock > sbrk(0))
		{
			//*my_malloc_error = 'P';
		}
		return NULL;
	}
	else
		return (void *) (bk + getBlockSize(bk));
}

void* getPrevBlock(void* bk)
{
	if((uintptr_t)bk == progEnd)//if first block
	{
		return NULL;
	}

	int prevBlockSize = (uintptr_t)(bk+ PREV_SIZE_OFFSET);
	return (void*) (bk - prevBlockSize);
}


void print_BlockString(void* bk)
{
	printf("\n___---___BLOCKSTRING___---___\n");
	printf("Addr: [%p]\n", bk);
	//printf("isFree")
}


void heap_init()
{

}

void *my_malloc(int size)
{
	//scan list of free mem blocks
	//choose one based on policy
	//if appropriate is found
	//TODO AFTERWORDS split chosen block
	//else alloc another chunk of mem using sbrk()
	//TODO AFTERWORDS split difference
	//check if difference is beyond max and take appropriate action if so
}


void my_free(void *ptr);

//Specifies the memory allocation policy
void my_mallopt(int pol)
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

int main()
{
	//testing
	void* p;
	//printf("Size of void pointer: %d \n", sizeof(p));
	return 0;
}