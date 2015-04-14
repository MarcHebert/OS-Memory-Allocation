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
#define PTR_SIZE 8 //64 bit compatability
 
//boundary tag scheme
#define SIZE_OFFSET 1 * (PTR_SIZE)
#define FREE_OFFSET 0 * (PTR_SIZE)
#define PREV_SIZE_OFFSET -1 * (PTR_SIZE)
#define PREV_FREE_OFFSET -2 * (PTR_SIZE) 
#define TAG_SIZE 4 * (PTR_SIZE)
#define BLOCK_CONTENT_OFFSET 2 * (PTR_SIZE)	

//specific to free blocks
#define NEXT_FREE_BLOCK_OFFSET	2 * (PTR_SIZE)
#define PREV_FREE_BLOCK_OFFSET  3 * (PTR_SIZE)	

//constants, bounds, bookkeeping etc.

	//error handling
	extern char *my_malloc_error;

	/*key
		P = pointer arithmetic
	*/

int init  = 0;
unsigned long int topBlockSize = 0;
unsigned int totalAllocBytes = 0;
unsigned int totalFreeBytes =0;// or perhaps MAX_BLOCK_SIZE;
unsigned int largestContFreeSpace =0;//or perhaps MAX_BLOCK_SIZE;
unsigned int numBlocks = 0;
unsigned int numFreeBlocks = 0;

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
	//
}

void* getPrevFreeBlock(void* bk)
{
	return *(void**) (bk + PREV_FREE_BLOCK_OFFSET);
	//
}


//accessor functions
void setBlockSize(void* bk, int size)//TODO BOOK KEEPING
{
	*(int*)(bk + SIZE_OFFSET) = (uintptr_t) size;
	//TODO handle bounds and block collision etc.
}

void setIsFree(void* bk, int isFree)
{

	*(int*)(bk+ FREE_OFFSET) = (uintptr_t)isFree;
	if(isFree ==1)
	{
		//TODO see if possible to merge free blocks
	}
}

void setBlockSizeEndTag(void* bk, int size)
{
	uintptr_t blockSize = (uintptr_t) getBlockSize(bk);
	*(int*)(bk+blockSize + PREV_SIZE_OFFSET) = size;
}

void setIsFreeEndTag(void* bk, int isFree)
{
	uintptr_t blockSize = (uintptr_t) getBlockSize(bk);
	*(int*)(bk+blockSize + PREV_FREE_OFFSET) = isFree;
}

void setNextFreeBlock(void* bk, void* nbk)
{
	*(void**)(bk + NEXT_FREE_BLOCK_OFFSET) = nbk;
	//
}

void setPrevFreeBlock(void* bk, void* pbk)
{
	*(void**)(bk + PREV_FREE_BLOCK_OFFSET) = pbk;
	//
}

//helper functions

void print_BlockString(void* bk)
{
	int f;
	printf("___---___BLOCKSTRING___---___\n");
	printf("Addr[%p] ", bk);
	printf("isFree[%d] ",f = getIsFree(bk));
	printf("nextB[%lu] ", (uintptr_t)getNextBlock(bk));
	printf("prevB[%lu] ", (uintptr_t)getPrevBlock(bk));
	if( f != 0)
	{
		printf("nextFB[%lu] ", (uintptr_t)getNextFreeBlock(bk));
		printf("prevFB[%lu]\n", (uintptr_t)getPrevFreeBlock(bk));
	}
	else
		printf("\n");
}

void print_Heap()
{
	
	void* tmp = (void*) progEnd;
	int tmpFree , tmpSize = 0;   

	printf("\n___---___print_Heap___---___\n");
	while(tmp!= NULL)
	{
		print_BlockString(tmp);
		tmp = getNextBlock(tmp);
	}
	printf("___---___End of heap___---___\n");
}

void* findNextFreeBlock(void* bk)
{
	if(freeBlockHead==NULL)
		return NULL;//no free blocks
	void* tmp = freeBlockHead;
	while(tmp != NULL && tmp < bk)
		tmp = getNextFreeBlock(tmp);
	return tmp;
}

void* findPrevFreeBlock(void* bk)
{
	if(freeBlockHead==NULL)
		return NULL;//no free blocks
	void* nextTmp = getNextFreeBlock(freeBlockHead);
	void* tmp = freeBlockHead;
	while(nextTmp < bk && nextTmp != NULL)
	{
		tmp  = nextTmp;
		nextTmp = getNextFreeBlock(nextTmp);
	}
	return tmp;
}

void combineFreeBlocks(void* bk1, void* bk2)//assuming bk1 < bk2
{
	printf("\n___---___combineFreeBlocks___---___\n");
	int newSize = getBlockSize(bk1)+getBlockSize(bk2) - TAG_SIZE;
	setBlockSize(bk1, newSize); //losing a set of tags
	setBlockSizeEndTag(bk1, newSize);
	numFreeBlocks--;
	numBlocks--;
	totalFreeBytes = totalFreeBytes + TAG_SIZE;
	if(newSize > largestContFreeSpace)
		largestContFreeSpace = newSize;
}

void* findFirstFit(int size)
{
	printf("\n___---___findFirstFit___---___\n");
	size = size + TAG_SIZE;
	void* tmp = freeBlockHead;
	while(tmp != NULL)
	{
		if(size <= getBlockSize(tmp))
			return tmp;
		tmp = getNextFreeBlock(tmp);
	}
	return NULL;
}

void* findBestFit(int size)
{
	printf("\n___---___findBestFit___---___\n");
	size = size + TAG_SIZE;
	void* bestBlock = NULL;
	int bestSize = MAX_BLOCK_SIZE - TAG_SIZE;
	int tmpSize = 0 ;
	void* tmp = freeBlockHead;
	while(tmp != NULL)
	{
		tmpSize =  getBlockSize(tmp);
		if(tmpSize<bestSize && size<=tmpSize)
		{
			bestSize = tmpSize;
			bestBlock = tmp;
		}
		tmp = getNextFreeBlock(tmp);
	}
	return bestBlock;
}

void* newBlockAlloc(int size)
{
	printf("\n___---___newBlockAlloc___---___\n");
	void* newB = (void*) (uintptr_t) sbrk(TAG_SIZE+ size);
	setIsFree(newB, 0);
	setBlockSize(newB, size + TAG_SIZE);
	setBlockSizeEndTag(newB, size + TAG_SIZE);
	setIsFreeEndTag(newB, 0);
	numBlocks = numBlocks + 1;
	totalAllocBytes = totalAllocBytes + size + TAG_SIZE;
	//printf("\nNEWBLOCK\n");
	//print_BlockString(newB);
	return (void*)(newB + BLOCK_CONTENT_OFFSET);
}

void* defragFreedBlocks(void* fbk)
{
	printf("\n___---___defragFreedBlocks___---___\n");
	void* tmp = fbk;
	void* nextB = getNextBlock(fbk);
	void* prevB = getPrevBlock(fbk);
	if(prevB != NULL)
	{
		if(getIsFree(prevB))
		{
			combineFreeBlocks(prevB, tmp);
			tmp = prevB;
		}
	}
	if(nextB != NULL)
	{
		if(getIsFree(nextB))
		{
			combineFreeBlocks(tmp, nextB);
		}
	}
	return tmp;
}


//main functions
void *my_malloc(int size)
{
	printf("\n___---___MY_MALLOC___---___\n");
	if(init == 0)
	{
		init = 1;
		progEnd = sbrk(0);
	}

	void* tmp;
	if(policy == FIRST_FIT)
	{
		if((tmp = findFirstFit(size)) == NULL)
			return newBlockAlloc(size);
	}
	else
	{
		if((tmp = findBestFit(size))==NULL)
			return newBlockAlloc(size);
	}

	setIsFree(tmp, 0);

	//NEED TO DEFRAG
	uintptr_t oldSize =(uintptr_t) getBlockSize(tmp);
	//enough space for another block?
	uintptr_t leftoverSize = oldSize-(size+TAG_SIZE);
	if(leftoverSize> TAG_SIZE)//tagsize is minimum size of block
	{
		//set up leftover block
		void* leftoverB = (void*)(tmp+oldSize);
		setBlockSize(leftoverB,leftoverSize);
		setIsFree(leftoverB, 1);
		setIsFreeEndTag(leftoverB,1);
		setBlockSizeEndTag(leftoverB,leftoverSize);

		//setup recycled block
		setBlockSize(tmp, size + TAG_SIZE);
		setBlockSizeEndTag(tmp, size+ TAG_SIZE);

		//bookkeeping
		numBlocks++;
		totalAllocBytes += size+TAG_SIZE;
		totalFreeBytes -= size+TAG_SIZE;
	}
	else
	{
		numFreeBlocks--;
		totalAllocBytes += oldSize;
		totalFreeBytes -= oldSize;
	}
	setIsFreeEndTag(tmp,0);

	//scan list of free mem blocks
	//choose one based on policy
	//if appropriate is found
	//TODO AFTERWORDS split chosen block
	//else alloc another chunk of mem using sbrk()
	//TODO AFTERWORDS split difference
	//check if difference is beyond max and take appropriate action if so
}


void my_free(void* ptr)
{
	printf("\n___---___MY_FREE___---___\n");

	if(ptr ==NULL)
	{
		printf("Freeing nothing, invalid argument\n");
		return;
	}

	void* freeB  = (void*) (ptr - (uintptr_t)BLOCK_CONTENT_OFFSET);
	totalFreeBytes = getBlockSize(freeB) + totalFreeBytes;
	totalAllocBytes = totalAllocBytes - getBlockSize(freeB);
	printf("Block before free\n");
	print_BlockString(freeB);
	numFreeBlocks++;
	setIsFree(freeB,1);

	//check if first freed block
	if(freeBlockHead == NULL)
	{
		freeBlockHead = freeB;
		setPrevFreeBlock(freeBlockHead, NULL);
		setNextFreeBlock(freeBlockHead, NULL);
		printf("First free block - reassigning ptrs:\n");
		print_BlockString(freeB);
	}
	else
	{
		freeB = defragFreedBlocks(freeB);

		//debug here, TODO print prev and next block
		printf("Not the first free block - reassigning ptrs:\n");
		print_BlockString(freeB);

	
		void* nextFreeB = findNextFreeBlock(freeB);
		void* prevFreeB = findPrevFreeBlock(freeB);
		printf("ptrs reassigned:\n");
		setPrevFreeBlock(freeB, prevFreeB);
		setNextFreeBlock(freeB, nextFreeB);
		print_BlockString(freeB);

		//doubly join freed blocks
		if(nextFreeB != NULL)
			setPrevFreeBlock(nextFreeB, freeB);
		if(prevFreeB != NULL)
			setNextFreeBlock(prevFreeB, freeB);
	}
	//bookkeeping
	if(getBlockSize(freeB)> MAX_BLOCK_SIZE)
	{
		setBlockSize(freeB, MAX_BLOCK_SIZE);
	}
	if( getBlockSize(freeB)> largestContFreeSpace)
		largestContFreeSpace  = getBlockSize(freeB);


}

//Specifies the memory allocation policy
void my_mallopt(int pol)
{
	if (pol != FIRST_FIT)
		policy = BEST_FIT;
	else
		policy = FIRST_FIT;
}

//Prints memory allocation specifics
void my_mall_info()
{
	printf("\n___---___HEAP METADATA___---___\n");
	printf("Number of used bytes:\t%d\n", totalAllocBytes);
	printf("Number of free bytes:\t%d\n", totalFreeBytes);
	printf("Number of blocks:\t%d\n", numBlocks);
	printf("Number of free blocks:\t%d\n", numFreeBlocks);
	printf("Size of largest free block:\t%d\n", largestContFreeSpace);

	//print
	//total number of bytes allocated
	//total free space
	//largest contiguous free space
	//other
}

int main()
{
	my_malloc(400);
	print_Heap();
	my_malloc(40);
	print_Heap();
	my_mall_info();
}