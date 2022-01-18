#include "my_malloc.h"
#define METADATA_SIZE sizeof(metadata_t)

unsigned long memory_size = 0;
unsigned long freelist_size = 0;
metadata_t* freelist_head = NULL;
metadata_t* block_head = NULL;

/* Find the first fit block in free list */
metadata_t* findFirstBlock(size_t size) {
	metadata_t* curr = freelist_head;

	while (curr != NULL) {
		// If the free block's size >= required size
		if (curr->size >= size) {
			return curr;
		}
		curr = curr->next;
	}
	// No fit block, return NULL
	return NULL;
}

/* Find the best fit block in free list */
metadata_t* findBestBlock(size_t size) {
	metadata_t* best = NULL;
	metadata_t* curr = freelist_head;

	while (curr != NULL) {
		// If the free block's size == required size, choose this block and return
		if (curr->size == size)
			return curr;
		// If the free block's size > required size
		if (curr->size > size) {
			// Update the best size
			if ((curr->size < best->size) || best == NULL) {
				best = curr;
			}
		}
		curr = curr->next;
	}

	return best;
}

/* Add newly freed block into the FRONT of free list */
void addFreelist(metadata_t* curr) {
	// Update freelist_size
	freelist_size += (curr->size + METADATA_SIZE);
	// Update the pointer's information
	curr->free = 1;
	curr->prev = NULL;
	curr->next = freelist_head;
	// Prevent NULL->prev
	if (freelist_head != NULL) {
		freelist_head->prev = curr;
	}
	freelist_head = curr;
}

/* Remove the fit block from free list */
void removeFreelist(metadata_t* curr) {
	// head or head + tail (the only in the free list)
	if (curr == freelist_head) {
		freelist_head = curr->next;
		if (freelist_head != NULL) {
			freelist_head->prev = NULL;
		}
	}
	// tail
	else if (curr->prev != NULL && curr->next == NULL) {
		curr->prev->next = NULL;
	}
	// middle
	else {
		curr->prev->next = curr->next;
		curr->next->prev = curr->prev;
	}
}

/* Create a new block if no fit block is found */
void* useNewBlock(size_t size) {
	// The size to generate is block's size + METADATA_SIZE
	size_t toSbrk = METADATA_SIZE + size;
	metadata_t* new_block = sbrk(toSbrk);
	if (new_block == (void*) - 1) {
		return NULL;
	}

	// Update memory_size
	memory_size += toSbrk;

	if (block_head == NULL) {
		block_head = new_block;
	}
	//Update new_block's information
	new_block->size = size;
	new_block->free = 0;
	new_block->next = NULL;
	new_block->prev = NULL;

	return (char*)new_block + METADATA_SIZE;
}

/* Split the block */
metadata_t* splitBlock(metadata_t* curr, size_t size) {
	// Create a new block
	metadata_t* new_block = (metadata_t*)((char*)curr + size + METADATA_SIZE);

	// Update new block
	new_block->free = 1;
	new_block->size = curr->size - size - METADATA_SIZE;
	new_block->prev = NULL;
	new_block->next = NULL;

	// Add the new block into free list
	addFreelist(new_block);

	return new_block;
}

/* Use an old block */
void* useOldBlock(metadata_t* curr, size_t size) {
	// Can be splitted
	if (curr->size > size + METADATA_SIZE) {
		metadata_t* new_block = splitBlock(curr, size);
		//removeFreelist(curr);	
		freelist_size -= (size + METADATA_SIZE);
	}
	// Cannot be splitted
	else {
		//removeFreelist(curr);
		freelist_size -= (curr->size + METADATA_SIZE);
	}

	removeFreelist(curr);

	// Update information
	curr->free = 0;
	curr->next = NULL;
	curr->prev = NULL;

	return (char*)curr + METADATA_SIZE;
}

/* Merge the metadata and its neighbors */
void mergeFreelist(metadata_t* curr) {
	// Next and itself
	if (curr->next != NULL && (char*)curr + METADATA_SIZE + curr->size == (char*)curr->next) {
		curr->size = curr->size + METADATA_SIZE + curr->next->size;
		removeFreelist(curr->next);
	}

	// Previous and itself
	if (curr->prev != NULL && (char*)curr->prev + METADATA_SIZE + curr->prev->size == (char*)curr) {
		curr->prev->size = curr->prev->size + METADATA_SIZE + curr->size;
		removeFreelist(curr);
	}
}

//First Fit malloc/free
void* ff_malloc(size_t size) {
	metadata_t* find = findFirstBlock(size);
	if (find == NULL) {
		return useNewBlock(size);
	}
	else {
		return useOldBlock(find, size);
	}
}

void ff_free(void* ptr) {
	metadata_t* meta = (metadata_t*)((char*)ptr - METADATA_SIZE);
	addFreelist(meta);
	mergeFreelist(meta);
}

//Best Fit malloc/free 
void* bf_malloc(size_t size) {
	metadata_t* find = findBestBlock(size);
	if (find == NULL) {
		return useNewBlock(size);
	}
	else {
		return useOldBlock(find, size);
	}
}

void bf_free(void* ptr) {
	ff_free(ptr);
}

unsigned long get_data_segment_size() {
	return memory_size;
}
unsigned long get_data_segment_free_space_size() {
	return freelist_size;
}