
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "ts_hashmap.h"

//capacity number of locks initialized, 1 per linked list
//when you call a method -- put/del/get --
//find the row we're going to, and lock it
//do work
//then unlock once done

//in main: spawn threads, join them
//routine fxn: call put/del/get

//declare hashmap, which should be global because it's malloc'd on heap
ts_hashmap_t *hashmap;

//global array of mutexes
//pthread_mutex_t *array;

void *routine(){
	for(int i = 0; i < 5; i++){
		//get random number 0 - 2
		int r = rand() % 3;

		//do put
		if(r == 0){
			//get random number
			int s = rand() % 100;
			put(hashmap, s, s); 
			//printf("put s: %d\n", s);
		} //do get
			else if(r == 1){
			//get random number
			int s = rand() % 100;
			get(hashmap, s);
			//printf("get s: %d\n", s);
			//printf("getto: %d\n", getto);

		} //do del
		else if(r == 2){
			//get random number
			int s = rand() % 100;
			del(hashmap, s); 
			//printf("del s: %d\n", s);
			//printf("delto: %d\n", delto);
		}


	}
	return NULL;

}

int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Usage: %s <num threads> <hashmap capacity>\n", argv[0]);
		return 1;
	}

    srand((time(NULL)));
	int num_threads = atoi(argv[1]);
	int capacity = (unsigned int) atoi(argv[2]);

	//initialize hashmap
	hashmap = initmap(capacity);

	// //get space for array by malloc
	// array = (pthread_mutex_t*) malloc (sizeof(pthread_mutex_t) * capacity);

	// //initialize array of mutex locks, 1 lock per row in table
	// for(int i = 0; i < capacity; i++){
	// 	pthread_mutex_init(&array[i], NULL);
	// }

	//initalize threads
	//allocate space to hold threads
	pthread_t *threads = (pthread_t*) malloc(num_threads * sizeof(pthread_t));

	//printf("numthreads: %d", num_threads);
	//create threads
	for(int i = 0; i < num_threads; i++){
		pthread_create(&threads[i], NULL, routine, NULL);
	}

	//join threads
	for(int i = 0; i < num_threads; i++){
		pthread_join(threads[i], NULL);
	}


	//print hashmap
	printmap(hashmap);

	//free threads
	free(threads);
	threads = NULL;

	//free mutex locks
	for(int i = 0; i < capacity; i++){
		pthread_mutex_destroy(&array[i]);
	}

	//free array
	free(array);
	array = NULL;
	
	//free hashmap

	//free hashmap.table
	for(int i = 0; i < capacity; i++){
		//get entry at index
		ts_entry_t *entry = hashmap->table[i];
		ts_entry_t *curr;
		
		//free each row (linked list) in table
		while(entry != NULL){
			curr = entry;
			entry = entry->next;
			free(curr);
			curr = NULL;
			//entry = curr->next;
		}

    	hashmap->table[i] = NULL;
	}

	free(hashmap->table);
	hashmap->table = NULL;

	free(hashmap);
	hashmap = NULL;


	return 0;
}

