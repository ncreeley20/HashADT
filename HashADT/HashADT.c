//
// File name: HashADT.c
//
// Description: Implementation of the Hash ADT functions that driver will interact with
//
// @author Nick Creeley - nc8004
//
// version control:
// git hw6 repository
//
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

//include standard libraries

#include <stdbool.h>

#include <stddef.h>

#include <stdlib.h>

#include <assert.h>

#include <stdio.h>

//include header file

#include "HashADT.h"

/// The KeyValuePair is a struct representing a key value pair
/// 
/// The key and value are both null pointers, allowing them to store any data

typedef struct KeyValuePair {

    void* key;

    void* value;

} KeyValuePair;


/// The hash table representation of ADT
/// Uses array of KeyValuePairs to represent hash table
/// Holds given fcns from client to use
/// Holds counters to keep track of performance

struct hashtab_s {

    //keep track of capacity
    size_t capacity;

    //track the occupancy
    size_t occupancy;

    //track the collisions occured
    int collisions;

    //track # of rehashes
    int rehashes;

    //handle all the fcns given to work with by client

    size_t (*hash_fcn)(const void *key);

    bool (*equals_fcn)(const void *key1, const void *key2);

    void (*print_fcn)(const void *key, const void *value);

    void (*delete_fcn)(void *key, void *value);

    //the hash table itself, an array of KeyValue pairs (struct)

    KeyValuePair* table;

};

/// ht_create(): the ADT create function
///
/// see headerfile for full documentation

HashADT ht_create(
    size_t (*hash)( const void *key ),
    bool (*equals)( const void *key1, const void *key2 ),
    void (*print)( const void *key, const void *value ),
    void (*delete)( void *key, void *value )
) {

    //check preconditions: make sure hash, equals and print are not null

    assert(hash != NULL);

    assert(equals != NULL);

    assert(print != NULL);

    //create and allocate new structure

    HashADT new;

    new = (HashADT) malloc(sizeof(struct hashtab_s));

    //assert that the mem alloc worked

    assert(new != NULL);

    //set all the tracking fields 
    new -> capacity = INITIAL_CAPACITY;

    new -> occupancy = 0;

    new -> collisions = 0;

    new -> rehashes = 0;

    //set the custom functions to be used by this hashtable

    new -> hash_fcn = hash;

    new -> equals_fcn = equals;

    new -> print_fcn = print;

    new -> delete_fcn = delete;

    //allocate memory for the table of key value pairs using init capacity

    new -> table = (KeyValuePair*)calloc(INITIAL_CAPACITY, sizeof(KeyValuePair));

    //assert that table alloc was completed

    assert(new -> table != NULL);

    return new;

}

/// ht_destroy(): the destroy fcn, calls destroy
///
/// see headerfile for full documentation

void ht_destroy( HashADT t) {

    // assert that the ADT is not null
    
    assert(t != NULL);
    
    // deallocate any dynamic storage
    // if delete fcn != NULL, use to deallocate each pair in table
    
    if (t -> delete_fcn != NULL) {
        
        //use the given delete fcn to free the key value pairs
        for(size_t i = 0; i < t -> capacity; i++){
            
            KeyValuePair pair = t -> table[i];
            
            if(pair.key != NULL) {
                t -> delete_fcn(pair.key, pair.value);
            }
        }

    } 
    
    //free the table and hashtable itself

    free(t -> table);

    free(t);

}

/// ht_dump() prints info about the table
///
/// see headerfile for full documentation

void ht_dump(const HashADT t, bool contents) {
    
    //print out all info about table

    printf("Size: %zu\n",t -> occupancy);

    printf("Capacity: %zu\n", t -> capacity);

    printf("Collisions: %d\n", t -> collisions);

    printf("Rehashes: %d\n", t -> rehashes);

    //if contents true print the contents using print fcn

    if(contents) {
        
        for(size_t i = 0; i < t -> capacity; i++){
            
            //get the key value of each bucket

            KeyValuePair pair = t -> table[i];
            
            //check if key value pair is null
            if(pair.key == NULL) {
                
                printf("%zu: null\n",i);

            } else {

                printf("%zu: (", i);

                t -> print_fcn(pair.key, pair.value);

                printf(")\n");
            }
        }
    }

}

/// ht_has(): check if table has key value pair 
///
/// see headerfile for full documentation

bool ht_has( const HashADT t, const void *key ) {
    
    //get the hashvalue of the key

    size_t hash_value = t -> hash_fcn(key);

    //get index of that specific hash fcn

    size_t orig_index = hash_value % t -> capacity;

    //set index to the starting index

    size_t index = orig_index;

    //check at the designated hashcode spot
    do{

        KeyValuePair pair = t -> table[index];
            
        if(pair.key == NULL) {
            //hit an empty bucket meaing we are done

            break;
        }
        

        if(t->equals_fcn(key,pair.key)) {
            
            return true;
        }

        // a collision has occured

        hash_value++;
        
        //recalculate the new index
        index = hash_value % t -> capacity;
        
        //increase collision counter only if collision has occurred
        t -> collisions++;
        
    } while(index != orig_index);

    return false;

}

/// ht_gets(): gets value associated with key from table
///
/// see headerfile for full documentation

const void *ht_get( const HashADT t, const void *key ) {
    
    //make sure table has key 
    assert(ht_has(t, key));

    size_t hashcode;

    size_t index;

    KeyValuePair pair;

    //get value associated with a key

    hashcode = t -> hash_fcn(key);

    index = hashcode % t->capacity;

    //get the key value pair at that spot

    pair = t -> table[index];

    while ( !(t-> equals_fcn(key,pair.key))) {
            
        //increment the hashcode, collision is handled by the get fcn already called

        hashcode++;

        index = hashcode % t -> capacity;

        pair = t -> table[index];

    }

    return pair.value;

}

/// ht_put():  adds a key value pair to table
///
/// see headerfile for full documentation

void *ht_put( HashADT t, const void *key, const void *value ) {
 
    //check if table needs to be rehashed first
    
    double current_threshold = (double) t-> occupancy / t-> capacity;

    if(current_threshold >= LOAD_THRESHOLD) {

        //rehash the table
        
        KeyValuePair* new_table = 
            (KeyValuePair*)calloc(t->capacity * RESIZE_FACTOR,sizeof(KeyValuePair));

        //store the old table data

        KeyValuePair* old_table = t -> table;

        size_t old_capacity = t -> capacity;

        //update to new capacity 
        t -> capacity = old_capacity * RESIZE_FACTOR;

        //iterate through the old table and update the new table

        for(size_t i = 0; i < old_capacity; i++){
            
            KeyValuePair pair = old_table[i];

            if(pair.key == NULL){
                continue;
            }

            //if it is empty we do not care
            
            //rehash the key with new capacity
            
            size_t hash = t->hash_fcn(pair.key);

            size_t index = hash % t->capacity;

            if(new_table[index].key != NULL) {
                
                //linear probe

                do {
                    
                    hash++;

                    index = hash % t->capacity;

                    //update collision counter for each collision

                    t -> collisions++;

                } while (new_table[index].key != NULL);

                //finally found a valid position
                new_table[index] = pair;

            } else {

                //found initial valid position
                new_table[index] = pair;

            }
        }
        //table finished rehashing, update new table

        t -> table = new_table;

        //update the rehash counter

        t -> rehashes++;

        //free the old table

        free(old_table);
    }

    //create and put new pair into table

    KeyValuePair new_pair;

    new_pair.key = (void*) key;

    new_pair.value = (void*) value;

    size_t  new_hash = t -> hash_fcn(key);

    size_t new_index = new_hash % t -> capacity;

    //check if the designated spot for the key is available

    if(t->table[new_index].key == NULL) {
        
        //no collisions, the designated spot is good

        t->table[new_index] = new_pair;
        
        t -> occupancy++;

        return NULL;

    }

    //we either have a collision or same value
    
    do {

    KeyValuePair collision_pair = t -> table[new_index];

    if (t->equals_fcn(key, collision_pair.key)) {
        
        void* old_value = collision_pair.value;

        t -> table[new_index].value = (void*) value;

        return old_value;
    }

    //increment hash and collision counter

    t -> collisions++;

    new_hash++;

    new_index = new_hash % t -> capacity;
    
    } while(t->table[new_index].key != NULL);
    
    //found an empty spot to put it

    t -> table[new_index] = new_pair;
    
    t-> occupancy++;

    return NULL;
}

/// ht_keys(): get collection of keys from table
///
/// see headerfile for full documentation

void **ht_keys( const HashADT t ) {
    
    //put enough memory for all valid keys
    void **keys = (void**)malloc(t->occupancy * sizeof(void*));
    
    //make sure memory was allocated
    assert(keys != NULL);

    size_t key_index= 0;
    
    //copy the keys over
    for (size_t i = 0; i < t->capacity; i++) {

        KeyValuePair pair = t->table[i];

        if (pair.key != NULL) {

            keys[key_index] = pair.key;
            key_index++;
        }
    }

    return keys;
}

/// ht_values(): get the collection of values from table
///
/// see headerfile for full documentation

void **ht_values( const HashADT t ) {

     //put enough memory for all valid values
     void **values = (void**)malloc(t->occupancy * sizeof(void*));

     //make sure memory was allocated
     assert(values != NULL);

     size_t value_index = 0;
    
     //copy the values over  
     for (size_t i = 0; i < t->capacity; i++) {

         KeyValuePair pair = t->table[i];

         if (pair.value != NULL) {

             values[value_index] = pair.value;
             value_index++;
         }
     }
     return values;

}


