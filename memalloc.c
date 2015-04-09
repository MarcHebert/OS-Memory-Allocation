//Marc Hebert
//260574038

#include <stdlib.h>
#include <stdio.h>//for print debugger statements
#include <stdint.h>// int pointer type

/*block structure

if free block
	[
	+3(PTR_SIZE)		prevFreeBlock unsigned int (4 bytes)
	+2(PTR_SIZE)		nextFreeBlock unsigned int (4 bytes)
	]
else
	+2(PTR_SIZE)		main content
tagging scheme
	+1(PTR_SIZE)		blockSize unsigned int (4 bytes)
 blckaddr				isFree (1 if free, 0 otherwise) char (1 byte)
	-1(PTR_SIZE)		isPrevFree char	(1 byte)
	-2(PTR_SIZE)		prevBlockSize unsigned int (4 bytes)
*/	

#define MAX_BLOCK_SIZE 128000 //bytes 128kb
#define PTR_SIZE 8
 
//boundary tag scheme
#define SIZE_OFFSET 1 * (PTR_SIZE)
#define FREE_OFFSET 0 * (PTR_SIZE)
#define PREV_SIZE_OFFSET -1 * (PTR_SIZE)
#define PREV_FREE_OFFSET -2 * (PTR_SIZE) 
#define TAG_SIZE 4 * (PTR_SIZE)

//specific to free blocks
#define NEXT_FREE_BLOCK_OFFSET	2*(PTR_SIZE)
#define PREV_FREE_BLOCK_OFFSET  3*(PTR_SIZE)	

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

uintptr_t progBreak = 0;
uintptr_t progEnd = 0;
void* freeBlockHead = NULL;

//policy
#define FIRST_FIT 0 
#define BEST_FIT 1
int policy = FIRST_FIT;// default is first fit

//getter functions
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

void* getNextFreeBlock(void* bk)
{
	return *(void**) (bk + NEXT_FREE_BLOCK_OFFSET);
}

void* getPrevFreeBlock(void* bk)
{
	return *(void**) (bk + PREV_FREE_BLOCK_OFFSET);
}

void print_BlockString(void* bk)
{
	int f;
	printf("\n___---___BLOCKSTRING___---___\n");
	printf("Addr:\t[%p]\n", bk);
	printf("isFree:\t[%d]\n",f = getIsFree(bk));
	printf("nextB:\t[%lu]\n", (uintptr_t)getNextBlock(bk));
	printf("prevB:\t[%lu]\n", (uintptr_t)getPrevBlock(bk));
	if( f != 0)
	{
		printf("nextFB:\t[%lu]\n", (uintptr_t)getNextFreeBlock(bk));
		printf("prevFB:\t[%lu]\n", (uintptr_t)getPrevFreeBlock(bk));
	}
}

void* firstFit(int size)
{
	void* tmp = freeBlockHead;
	while(tmp != NULL)
	{
		if(size <= getBlockSize(tmp) - TAG_SIZE)
			return tmp;
		tmp = getNextFreeBlock(tmp);
	}
}

//accessor functions
void setBlockSize(void* bk, int size)//TODO BOOK KEEPING
{
	*(int*)(bk + SIZE_OFFSET) = size;
	//TODO handle bounds and block collision etc.
}

void setIsFree(void* bk, int isFree)
{

	*(char*)(bk+ FREE_OFFSET) = (char)isFree;
	if(isFree ==1)
	{
		//TODO see if possible to merge free blocks
	}
}

void setNextFreeBlock(void* bk, void* nbk)
{
	*(void**)(bk + NEXT_FREE_BLOCK_OFFSET) = nbk;
}

void setPrevFreeBlock(void* bk, void* pbk)
{
	*(void**)(bk + PREV_FREE_BLOCK_OFFSET) = pbk;
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
	printf("Size of void pointer: %lu \n", sizeof(char*));
	return 0;
}