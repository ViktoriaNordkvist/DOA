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
	//printf("table_empty\n");
	// Allocate the table header.
	table *t = calloc(1, sizeof(table)); //VALGRIND FEL

	t->size= 1000; //funkar detta, ska jag ha detta istället?
	// Create the array to hold the table_entry-ies.
	t->entries = array_1d_create(0, t->size, NULL);		//VALGRINDFEL
	// Store the key compare function and key/value free functions.
	t->key_cmp_func = key_cmp_func;
	t->key_free_func = key_free_func;
	t->value_free_func = value_free_func;

	return t;

}


bool table_is_empty(const table *t)
{
	//printf("table_is_empty\n");
	// Iterate over all elements. Call print_func on keys/values.
	int pos = array_1d_low(t->entries);
	bool empty = true;

	while(pos <= array_1d_high(t->entries)){
		if (array_1d_has_value(t->entries, pos)){
			empty = false;
	 	}

	pos++;
	}
	//If the array is empty then we return true
	return empty;
}


void table_insert(table *t, void *key, void *value)
{
	//printf("table_insert\n");
	int pos = array_1d_low(t->entries);
	bool same_key = false;
	struct table_entry *a = malloc(sizeof(struct table_entry));		//VALGRINDFEL
	a->key = key;
	a->value = value;

	// Allocate the key/value structure.
	while(array_1d_has_value(t->entries, pos)){
		struct table_entry *entry = array_1d_inspect_value(t->entries, pos);

		if (t->key_cmp_func(entry->key, key) == 0){
			same_key = true;

			if (t->key_free_func != NULL) {
					t->key_free_func(entry->key);
			}
			if (t->value_free_func != NULL) {
				t->value_free_func(entry->value);
			}
			free(entry);
			array_1d_set_value(t->entries, a, pos);

			break;
		}
		pos++;
	}
	// Set the pointers and insert first in the list. This will
	if(!same_key){
		array_1d_set_value(t->entries, a, pos);
	}
	free(a);
}

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

void *table_choose_key(const table *t)
{
	//printf("table_choose_key\n");
	// Return first key value.
	int pos = array_1d_low(t->entries);
	struct table_entry *entry = array_1d_inspect_value(t->entries, pos);

	return entry->key;
}

void table_remove(table *t, const void *key)
{
	//printf("table_remove\n");
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
				//printf("kommit in och bör kunna påbörja att byta plats\n");
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

void table_kill(table *t)
{
	//printf("table_kill\n");
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
	free(entry); //ÄR DETTA RÄTT?
	// Kill what's left of the list...
	array_1d_kill(t->entries);
	// ...and the table.
	free(t);
}

void table_print(const table *t, inspect_callback_pair print_func)
{
	//printf("table_print\n");
	// Iterate over all elements. Call print_func on keys/values.
	int pos = array_1d_low(t->entries);

	while(array_1d_has_value(t->entries, pos)) {
		struct table_entry *entry = array_1d_inspect_value(t->entries, pos);
		// Call print_func
		print_func(entry->key, entry->value);
		pos++;

	}
}
