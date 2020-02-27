#include <stdlib.h>
#include <stdio.h>

#include "table.h"
#include "array_1d.h"




// ===========INTERNAL DATA TYPES============
struct table {
	array_1d *entries;			//kan ej ha dlist här, vad ska man byta till? array_1d
	compare_function *key_cmp_func;
	free_function key_free_func;
	free_function value_free_func;
	int size;	//fungerar detta???
};

struct table_entry {
	void *key;
	void *value;
};

// ===========INTERNAL FUNCTION IMPLEMENTATIONS============
/**
 * table_empty() - Create an empty table.
 * @key_cmp_func: A pointer to a function to be used to compare keys. See
 *                util.h for the definition of compare_function.
 * @key_free_func: A pointer to a function (or NULL) to be called to
 *		   de-allocate memory for keys on remove/kill.
 * @value_free_func: A pointer to a function (or NULL) to be called to
 *		     de-allocate memory for values on remove/kill.
 *
 * Returns: Pointer to a new table.
 */
table *table_empty(compare_function *key_cmp_func, free_function key_free_func,
				   free_function value_free_func)
{
	// Allocate the table header.
	table *t = calloc(1, sizeof(table));

	t->size= 1000;
	// Create the array to hold the table_entry-ies.
	t->entries = array_1d_create(0, t->size, NULL);
	// Store the key compare function and key/value free functions.
	t->key_cmp_func = key_cmp_func;
	t->key_free_func = key_free_func;
	t->value_free_func = value_free_func;

	return t;

}

/**
 * table_is_empty() - Check if a table is empty.
 * @t: Table to check.
 *
 * Returns: True if table contains no key/value pairs, false otherwise.
 */
bool table_is_empty(const table *t)
{

	//If the array is empty then we return true
	return !array_1d_has_value(t->entries, array_1d_low(t->entries));
}

/**
 * table_insert() - Add a key/value pair to a table.
 * @t: Table to manipulate.
 * @key: A pointer to the key value.
 * @value: A pointer to the value value.
 *
 * Insert the key/value pair into the table. A test is preformed to check if
 * the key is a duplicate, if it is then the pointer to the value value given as
 * input will replace the old value and the old value is removed.
 *
 * Returns: Nothing.
 */
void table_insert(table *t, void *key, void *value)
{
	int pos = array_1d_low(t->entries);
	bool same_key = false;
	struct table_entry *a = malloc(sizeof(struct table_entry));
	// Set the pointers
	a->key = key;
	a->value = value;

	// Allocate the key/value structure.
	while(array_1d_has_value(t->entries, pos)){
		// Inspect the table entry
		struct table_entry *entry = array_1d_inspect_value(t->entries, pos);

		// Compare the supplied key with the key of this entry.
		if (t->key_cmp_func(entry->key, key) == 0){
			same_key = true;
			// If we have a match, call free on the key
			// and/or value if given the responsiblity
			if (t->key_free_func != NULL) {
					t->key_free_func(entry->key);
			}
			if (t->value_free_func != NULL) {
				t->value_free_func(entry->value);
			}
			free(entry);
			// Insert the new element in to the stack
			array_1d_set_value(t->entries, a, pos);
			break;
		}
		// Move on to next element.
		pos++;
	}
	 // Insert the new element in to the stack
	if(!same_key){
		array_1d_set_value(t->entries, a, pos);
	}
}
/**
 * table_lookup() - Look up a given key in a table.
 * @t: Table to inspect.
 * @key: Key to look up.
 *
 * Returns: The value corresponding to a given key, or NULL if the key
 * is not found in the table.
 */
void *table_lookup(const table *t, const void *key)
{
	//printf("table_lookup\n");
	// Iterate over the list. Return first match.
	int pos = array_1d_low(t->entries);

	while(array_1d_has_value(t->entries, pos)) {
		// Inspect the table entry
		struct table_entry *entry = array_1d_inspect_value(t->entries, pos);
		// Check if the entry key matches the search key.
		if (t->key_cmp_func(entry->key, key) == 0) {
			// If yes, return the corresponding value pointer.
			return entry->value;
		}
		// Continue with the next position.
		pos++;
	}
	// No match found. Return NULL.
	return NULL;
}

/**
 * table_choose_key() - Return an arbitrary key.
 * @t: Table to inspect.
 *
 * Return an arbitrary key stored in the table. Can be used together
 * with table_remove() to deconstruct the table since no duplicates will
 * be stored in the table. Undefined for an empty table.
 *
 * Returns: An arbitrary key stored in the table.
 */
void *table_choose_key(const table *t)
{
	// Return first key value.
	int pos = array_1d_low(t->entries);
	struct table_entry *entry = array_1d_inspect_value(t->entries, pos);

	return entry->key;
}

/**
 * table_remove() - Remove a key/value pair in the table.
 * @t: Table to manipulate.
 * @key: Key for which to remove pair.
 *
 * Will call any free functions set for keys/values. If an element is removed
 * then the rest of the table will be pushed one step to the left. Does nothing
 * if key is not found in the table.
 *
 * Returns: Nothing.
 */
 void table_remove(table *t, const void *key)
 {
 	// Will be set if we need to delay a free.
 	void *deferred_ptr = NULL;

 	// Start at beginning of the array.
 	int pos = array_1d_low(t->entries);

 	// Iterate over the array. Remove any entries with matching keys.
 	while(array_1d_has_value(t->entries, pos)) {
 		// Inspect the table entry
 		struct table_entry *entry = array_1d_inspect_value(t->entries, pos);

 		// Compare the supplied key with the key of this entry.
 		if (t->key_cmp_func(entry->key, key) == 0) {
 			// If we have a match, call free on the key
 			// and/or value if given the responsiblity
 			if (t->key_free_func != NULL) {
 				if (entry->key == key) {
 					// The given key points to the same
 					// memory as entry->key. Freeing here
 					// would trigger a memory error in the
 					// next iteration. Instead, defer free
 					// of this pointer to the very end.
 					deferred_ptr = entry->key;
 				} else {
 					t->key_free_func(entry->key);
 				}
 			}
 			if (t->value_free_func != NULL) {
 				t->value_free_func(entry->value);
 			}

 			// vill kolla om nästa plats har ett värde, om det gör det då är inte den tomma platsen sist. då måste jag fylla den tomma platsen med någonting.
 			while(array_1d_has_value(t->entries, pos + 1 )){
 				struct table_entry *temp = array_1d_inspect_value(t->entries, pos+1);
 				array_1d_set_value(t->entries, temp, pos);
 				array_1d_set_value(t->entries, NULL, pos + 1);
 				pos++;
 			}
 			// Remove the list element itself.
 			array_1d_set_value(t->entries, NULL, pos); //IS DIS OK???????????????
 			// Deallocate the table entry structure.
 			free(entry);

 		} else {
 			// No match, move on to next element in the list.
 			pos++;
 		}
 	}
 	if (deferred_ptr != NULL) {
 		// Take care of the delayed free.
 		t->key_free_func(deferred_ptr);
 	}
 }


/**
 * table_kill() - Destroy a table.
 * @t: Table to destroy.
 *
 * Return all dynamic memory used by the table and its elements. If a
 * free_func was registered for keys and/or values at table creation,
 * it is called each element to free any user-allocated memory
 * occupied by the element values.
 *
 * Returns: Nothing.
 */
void table_kill(table *t)
{
	// Iterate over the list. Destroy all elements.
	int pos = array_1d_low(t->entries);

	while(array_1d_has_value(t->entries, pos)) {
		// Inspect the key/value pair.
		struct table_entry *entry = array_1d_inspect_value(t->entries, pos);
		// Free key and/or value if given the authority to do so.
		if (t->key_free_func != NULL) {
			t->key_free_func(entry->key);
		}
		if (t->value_free_func != NULL) {
			t->value_free_func(entry->value);
		}
		// Move on to next element.
		pos++;
		// Deallocate the table entry structure.
		free(entry);
	}
	// Kill what's left of the list...
	array_1d_kill(t->entries);
	// ...and the table.
	free(t);
}

/**
 * table_print() - Print the given table.
 * @t: Table to print.
 * @print_func: Function called for each key/value pair in the table.
 *
 * Iterates over the key/value pairs in the table and prints them.
 * Will print all stored elements.
 *
 * Returns: Nothing.
 */
void table_print(const table *t, inspect_callback_pair print_func)
{
	// Iterate over all elements. Call print_func on keys/values.
	int pos = array_1d_low(t->entries);

	while(array_1d_has_value(t->entries, pos)) {
		struct table_entry *entry = array_1d_inspect_value(t->entries, pos);
		// Call print_func
		print_func(entry->key, entry->value);
		pos++;
	}
}
