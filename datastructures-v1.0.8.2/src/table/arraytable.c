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
table *table_empty(compare_function *key_cmp_func, free_function key_free_func,
				   free_function value_free_func)
{
	// Allocate the table header.
	table *t = calloc(1, sizeof(table));

	t->size= 100; //funkar detta, ska jag ha detta istället?
	// Create the array to hold the table_entry-ies.
	t->entries = array_1d_create(0, t->size, NULL);
	// Store the key compare function and key/value free functions.
	t->key_cmp_func = key_cmp_func;
	t->key_free_func = key_free_func;
	t->value_free_func = value_free_func;

	return t;

}


bool table_is_empty(const table *t)
{
	// Iterate over all elements. Call print_func on keys/values.
	int pos = array_1d_low(t->entries);

	while(pos <= array_1d_high(t->entries)){
		if (array_1d_has_value(t->entries, pos)){
			// If the array is not empty, and therefore has a value, then we
			// return false
			return false;
	 	}

	pos++;
	}

	//If the array is empty then we return true
	return true;
}


void table_insert(table *t, void *key, void *value)
{
	int pos = array_1d_low(t->entries);
	struct table_entry *entry = array_1d_inspect_value(t->entries, pos);

	while(pos <= array_1d_high(t->entries)){
		if(t->key_cmp_func(entry->key, key) == 0){
			printf("This key already exists, try again");
		}
		else{
			// Allocate the key/value structure.
			struct table_entry *entry = malloc(sizeof(struct table_entry));

			// Set the pointers and insert first in the list. This will
			// cause table_lookup() to find the latest added value.
			entry->key = key;
			entry->value = value;
			array_1d_set_value(t->entries, entry, array_1d_low(t->entries));
		}

	pos++;
	}
}

void *table_lookup(const table *t, const void *key)
{
	// Iterate over the list. Return first match.
	int pos = array_1d_low(t->entries);

	while(pos <= array_1d_high(t->entries)) {
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

void *table_choose_key(const table *t)
{
	// Return first key value.
	int pos = array_1d_low(t->entries);
	struct table_entry *entry = array_1d_inspect_value(t->entries, pos);

	return entry->key;
}

void table_remove(table *t, const void *key)
{
	// Will be set if we need to delay a free.
	void *deferred_ptr = NULL;

	// Start at beginning of the array.
	int pos = array_1d_low(t->entries);

	// Iterate over the array. Remove any entries with matching keys.
	while(pos <= array_1d_high(t->entries)) {
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
			// Remove the list element itself.
			array_1d_set_value(t->entries, NULL, pos); //IS DIS OK???????????????
			// Deallocate the table entry structure.
			free(entry);

			// vill kolla om nästa plats har ett värde, om det gör det då är inte den tomma platsen sist. då måste jag fylla den tomma platsen med någonting.
			while(array_1d_has_value(t->entries, pos + 1 )){
				struct table_entry *temp = array_1d_inspect_value(t->entries, pos+1);
				//
				array_1d_set_value(t->entries, temp, pos);
				array_1d_set_value(t->entries, NULL, pos + 1);
				pos++;
				free(temp);
			}

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

void table_kill(table *t)
{
	// Iterate over the list. Destroy all elements.
	int pos = array_1d_low(t->entries);

	while(pos <= array_1d_high(t->entries)) {
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

void table_print(const table *t, inspect_callback_pair print_func)
{
	// Iterate over all elements. Call print_func on keys/values.
	int pos = array_1d_low(t->entries);

	while(pos <= array_1d_high(t->entries)) {
		struct table_entry *entry = array_1d_inspect_value(t->entries, pos);
		// Call print_func
		array_1d_print(entry->key, entry->value);
		pos++;

	}
}
