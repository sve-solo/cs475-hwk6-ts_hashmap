#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ts_hashmap.h"

//global array of mutexes
pthread_mutex_t *array;

/**
 * Creates a new thread-safe hashmap. 
 *
 * @param capacity initial capacity of the hashmap.
 * @return a pointer to a new thread-safe hashmap.
 */
ts_hashmap_t *initmap(int capacity){
  //malloc space for a new hashmap using struct ts_hashmap_t
  ts_hashmap_t *hashmap = (ts_hashmap_t*) malloc(sizeof(ts_hashmap_t) * 1);

  //initialize args for new hashmap
  hashmap->capacity = capacity;
  hashmap->size = 0;
  //malloc space for table
  hashmap->table = (ts_entry_t**) malloc(sizeof(ts_entry_t*) * capacity);

  //initialize each row in array to NULL
  for(int i = 0; i < capacity; i++){
		hashmap->table[i] = NULL;
	}

  //get space for array by malloc
	array = (pthread_mutex_t*) malloc (sizeof(pthread_mutex_t) * capacity);

	//initialize array of mutex locks, 1 lock per row in table
	for(int i = 0; i < capacity; i++){
		pthread_mutex_init(&array[i], NULL);
	}


  //return pointer to hashmap
  return hashmap;
}

/**
 * Obtains the value associated with the given key.
 * @param map a pointer to the map
 * @param key a key to search
 * @return the value associated with the given key, or INT_MAX if key not found
 */
int get(ts_hashmap_t *map, int key) {
  //cast key as unsigned int
  unsigned int castKey = (unsigned int) key;
  //modulo by size of array (capacity)
  unsigned int where = castKey % (map->capacity);

  //get initialized lock from array, and lock it
  int locked = pthread_mutex_lock(&array[where]);

  if(locked != 0){
    printf("Lock error");
  }

  //get entry at that index
  ts_entry_t *entry = map->table[where];
  
  //if entry does not exist, return INT_MAX
  if(entry == NULL){
    //unlock lock
    int unlocked = pthread_mutex_unlock(&array[where]);
    if(unlocked != 0){
      printf("Lock error");
    }

    return INT_MAX;
  }
  else{
    //will hold current entry 
    ts_entry_t *curr;

    while(entry != NULL){
      //if the given key matches any existing entry's key
      if(key == entry->key){
        //unlock lock
        int unlocked = pthread_mutex_unlock(&array[where]);
        if(unlocked != 0){
          printf("Lock error");
        }

        //return value
        return entry->value;
      }

      //move to next item in linked list if a match was not found
      curr = entry;
      entry = curr->next;

    }
  }

  //unlock lock
  int unlocked = pthread_mutex_unlock(&array[where]);
  if(unlocked != 0){
    printf("Lock error");
  }
  //else, if we never find the key

  return INT_MAX;
}

/**
 * Associates a value associated with a given key.
 * @param map a pointer to the map
 * @param key a key
 * @param value a value
 * @return old associated value, or INT_MAX if the key was new
 */
int put(ts_hashmap_t *map, int key, int value){
  //calculate where to put the entry

  //cast key as unsigned int
  unsigned int castKey = (unsigned int) key;
  //modulo by size of array (capacity)
  unsigned int where = castKey % (map->capacity);

  //get initialized lock from array, and lock it
  int locked = pthread_mutex_lock(&array[where]);

  if(locked != 0){
    printf("Lock error");
  }


  //get entry at that index
  ts_entry_t *entry = map->table[where];
  
  //if entry does not exist
  if(entry == NULL){
    //get space for entry and make a new entry
    entry = (ts_entry_t*) malloc(sizeof(ts_entry_t) * 1);
    //insert the entry: give the new entry a key, value, and next
    entry->key = key;
    entry->value = value;
    entry->next = NULL;
    
    //insert entry
    map->table[where] = entry;

    //increase size since we've added a new entry
    map->size++;


    //unlock lock
    int unlocked = pthread_mutex_unlock(&array[where]);
    if(unlocked != 0){
    printf("Lock error");
    }


    //returns constant
    return INT_MAX;
  }
  //if it does exist, replace old value with new value
  else{
    //will hold current entry 
    ts_entry_t *curr;

    //traverse linked list until you reach end
    while(entry != NULL){
     // printf("entry not null\n");
      //if the given key matches any existing entry's key
      if(key == entry->key){
        //store current value to later return it
        int oldVal = entry->value;

        //replace old value with new value
        entry->value = value;

        //unlock lock
        int unlocked = pthread_mutex_unlock(&array[where]);
        if(unlocked != 0){
          printf("Lock error");
        }

        //return old value
        return oldVal;
      }

      //move to next item in linked list if a match was not found
      curr = entry;
      entry = curr->next;
    }
    
    //if we still haven't found a match, then insert a new entry
    //printf("still not null\n");
    entry = (ts_entry_t*) malloc(sizeof(ts_entry_t) * 1);
    //insert the entry: give the new entry a key, value, and next
    entry->key = key;
    entry->value = value;
    entry->next = NULL;
    curr->next = entry;

    //increment size
    map->size++;

  }
      
  //unlock lock
  int unlocked = pthread_mutex_unlock(&array[where]);
  if(unlocked != 0){
  printf("Lock error");
  }
  
  //return old value
  return INT_MAX;
}

/**
 * Removes an entry in the map
 * @param map a pointer to the map
 * @param key a key to search
 * @return the value associated with the given key, or INT_MAX if key not found
 */
int del(ts_hashmap_t *map, int key) {
  //cast key as unsigned int
  unsigned int castKey = (unsigned int) key;
  //modulo by size of array (capacity)
  unsigned int where = castKey % (map->capacity);

  //get initialized lock at index and lock it
  int locked = pthread_mutex_lock(&array[where]);
  if(locked != 0){
      printf("Lock error");
  }

  //get entry at that index
  ts_entry_t *entry = map->table[where];
  
  //if entry does not exist, return INT_MAX
  if(entry == NULL){
    //unlock lock
    int unlocked = pthread_mutex_unlock(&array[where]);
    if(unlocked != 0){
      printf("Lock error");
    }
    return INT_MAX;
  }
  else{
    //will hold current entry 
    ts_entry_t *curr;
    //if entry is the first and only thing in the list
    if(key == entry->key && entry->next == NULL){
      //store value to later return it
      int oldValue = entry->value;

      //remove the entry
      free(entry);
      entry = NULL;

      //set the table to NULL
      map->table[where] = NULL;

      //decrement size
      map->size--;

      //unlock lock
      int unlocked = pthread_mutex_unlock(&array[where]);
      if(unlocked != 0){
        printf("Lock error");
      }
      return oldValue;
    }
    //counts how many entries we've looped through
    int counter = 1;
    while(entry != NULL){
      //if the given key matches any existing entry's key
      if(key == entry->key){
        //entry to delete is the first entry in the list
         if(counter == 1){
          //temporarily store next entry
          ts_entry_t *next = entry->next;
          //store old value to later return it
          int oldValue = entry->value;

          //remove the entry
          free(entry);
          entry = NULL;
          
          //set the table to next entry in list
          map->table[where] = next;
          
          //decrement size
          map->size--;
          
          //unlock lock
          int unlocked = pthread_mutex_unlock(&array[where]);
            if(unlocked != 0){
              printf("Lock error");
            }
          return oldValue;
         }
         //entry to delete is in middle of list
         else{
          curr->next = entry->next;
          
          //store old value to later return it
          int oldValue = entry->value;

          //remove the entry
          free(entry);
          entry = NULL;

          //decrement size
          map->size--;

          //unlock lock
          int unlocked = pthread_mutex_unlock(&array[where]);
          if(unlocked != 0){
            printf("Lock error");
          }

          return oldValue;
         }
          
      }
      counter++;
      //move to next item in linked list if a match was not found
      curr = entry;
      entry = curr->next;
      
      }
  }
  
  //unlock lock
  int unlocked = pthread_mutex_unlock(&array[where]);
  if(unlocked != 0){
    printf("Lock error");
  }

  //else, if we never find the key
  return INT_MAX;
}




/**
 * @return the load factor of the given map
 */
double lf(ts_hashmap_t *map) {
  return (double) map->size / map->capacity;
}

/**
 * Prints the contents of the map
 */
void printmap(ts_hashmap_t *map) {
  for (int i = 0; i < map->capacity; i++) {
    printf("[%d] -> ", i);
    ts_entry_t *entry = map->table[i];
    while (entry != NULL) {
      printf("(%d,%d)", entry->key, entry->value);
      if (entry->next != NULL)
        printf(" -> ");
      entry = entry->next;
    }
    printf("\n");
  }
}