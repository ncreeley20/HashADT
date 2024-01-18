/// \file HashADT.h
/// \brief A generic hash table data structure.
///
/// @author CSCI243 Instructors

#ifndef HASHADT_H
#define HASHADT_H

#include <stdbool.h>    // bool
#include <stddef.h>     // size_t

/// Initial capacity of table upon creation
#define INITIAL_CAPACITY 16

/// The load at which the table will rehash
#define LOAD_THRESHOLD 0.75

/// The table size will double upon each rehash
#define RESIZE_FACTOR 2

///
/// General Notes on hash table Operation
///
/// - The operations identified below are the public interface to HashADT.
///
/// - The client supplies functions for hashing the key, checking key
///   equality, printing key, value pairs, and deleting pairs.
///
/// - The table assumes ownership of keys and values.  The delete function
///   (by default) will free the (key,value) pair upon destruction; to
///   prevent this, the application should pass a NULL pointer for the
///   delete function, which causes the delete function to NOT free the
///   (key, value) pair.
///
/// - There is no remove functionality. Entries remain until you call destroy.
///
/// - The destroy calls a no-operation delete if the client passes NULL destroy.
///
/// - Wherever a function has a precondition, and the client violates the
///   condition, and the code detects the violation, then the function will
///   assert failure and abort.
///
/// - Wherever a function has a postcondition, and the client violates the
///   condition, and the code detects the violation, then the function will
///   assert failure and abort.
///

///
/// The HashADT data type is a pointer to an opaque structure; clients
/// cannot see the structure's content.
///
typedef struct hashtab_s *HashADT;

///
/// Create a new hash table instance.  If delete is NULL, destroying the
/// table will NOT free the (key,value) data pairs.
///
/// @param hash The hash function for key data
/// @param equals The equal function for key comparison
/// @param print The print function for key, value pairs is used by dump().
/// @param delete The delete function for key, value pairs is used by destroy().
/// 
/// @exception Assert fails if it cannot allocate space
/// 
/// @pre hash, equals and print are valid function pointers.
/// 
/// @return A newly created table
///
HashADT ht_create(
    size_t (*hash)( const void *key ),
    bool (*equals)( const void *key1, const void *key2 ),
    void (*print)( const void *key, const void *value ),
    void (*delete)( void *key, void *value )
);

///
/// Destroy the table instance, and call delete function on (key,value) pair.
/// 
/// @param t The table to destroy
/// 
/// @pre t is a valid instance of table.
/// 
/// @post t is not a valid instance of table.
///
void ht_destroy( HashADT t );

///
/// Print information about hash table (size, capacity, collisions, rehashes). 
/// 
/// If contents is true, also print the entire contents of the hash table
/// using the registered print function with each non-null entry.
/// 
/// @param t The table to display
/// @param contents Do a full dump including the entire table contents
/// 
/// @pre t is a valid instance of table.
///
void ht_dump( const HashADT t, bool contents );

///
/// Get the value associated with a key from the table.  This function
/// uses the registered hash function to locate the key, and the
/// registered equals function to check for equality.
/// 
/// @param t The table
/// @param key The key
/// 
/// @pre The table must have the key, or the function will assert failure
/// @pre has( t, key) is true.
/// @pre t is a valid instance of table, and key is not NULL.
/// 
/// @return The value associated with the key
///
const void *ht_get( const HashADT t, const void *key );

///
/// Check if the table has a key.  This function uses the registered hash
/// function to locate the key, and the registered equals function to
/// check for equality.
/// 
/// @param t The table
/// @param key The key
/// 
/// @pre t is a valid instance of table, and key is not NULL.
/// 
/// @return Whether the key exists in the table.
///
bool ht_has( const HashADT t, const void *key );

///
/// Add a key value pair to the table, or update an existing key's value.
/// This function uses the registered hash function to locate the key,
/// and the registered equals function to check for equality.
/// 
/// @param t The table
/// @param key The key
/// @param value The value
/// 
/// @exception Assert fails if it cannot allocate space
/// 
/// @pre t is a valid instance of table.
/// 
/// @post if size reached the LOAD_THRESHOLD, table has grown by RESIZE_FACTOR.
/// 
/// @return The old value associated with the key, if one exists.
///
void *ht_put( HashADT t, const void *key, const void *value );

///
/// Get the collection of keys from the table.  This function allocates
/// space to store the keys, which the caller is responsible for freeing.
/// 
/// @param t The table
/// 
/// @exception Assert fails if it cannot allocate space
/// 
/// @pre t is a valid instance of table.
/// 
/// @post client is responsible for freeing the returned array.
/// 
/// @return A dynamic array of keys
///
void **ht_keys( const HashADT t );

///
/// Get the collection of values from the table.  This function allocates
/// space to store the values, which the caller is responsible for freeing.
/// 
/// @param t The table
/// 
/// @exception Assert fails if it cannot allocate space
/// 
/// @pre t is a valid instance of table.
/// 
/// @post client is responsible for freeing the returned array.
/// 
/// @return A dynamic array of values
///
void **ht_values( const HashADT t );

#endif // HASHADT_H
