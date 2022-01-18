#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct metadata {
	struct metadata* prev;
	struct metadata* next;
	size_t size; // not including METADATA_SIZE
	int free; // 1 when free
};
typedef struct metadata metadata_t;

//First Fit malloc/free
void* ff_malloc(size_t size);
void ff_free(void* ptr);
//Best Fit malloc/free 
void* bf_malloc(size_t size);
void bf_free(void* ptr);

metadata_t* splitBlock(metadata_t* curr, size_t size);
void* useNewBlock(size_t size);
void* useOldBlock(metadata_t* curr, size_t size);
metadata_t* findFirstBlock(size_t size);
metadata_t* findBestBlock(size_t size);

void addFreelist(metadata_t* curr);
void removeFreelist(metadata_t* curr);
void mergeFreelist(metadata_t* curr);

unsigned long get_data_segment_size();
unsigned long get_data_segment_free_space_size();
