


// ========DEN JAG FÖRÖSKER FIXA ===============================
void table_remove(table *t, const void *key)
{
	// Will be set if we need to delay a free.
	void *deferred_ptr = NULL;
	bool was_removed = false;

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

			// Deallocate the table entry structure.
			free(entry);
			was_removed = true;

		} else {
			// No match, move on to next element in the list.
			pos++;
		}
	}

	if(was_removed){
		// vill kolla om nästa plats har ett värde, om det gör det då är inte den tomma platsen sist. då måste jag fylla den tomma platsen med någonting.
		while(array_1d_has_value(t->entries, pos + 1 )){
			struct table_entry *temp = array_1d_inspect_value(t->entries, pos+1);
			array_1d_set_value(t->entries, temp, pos);
			array_1d_set_value(t->entries, NULL, pos + 1);
			pos++;
		}
		// Remove the list element itself.
		//array_1d_set_value(t->entries, NULL, pos); //IS DIS OK???????????????
	}
	if (deferred_ptr != NULL) {
		// Take care of the delayed free.
		t->key_free_func(deferred_ptr);
	}
}

// ========ORGINALET JAG VET FUNKAR ===============================

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
