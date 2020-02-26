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

void table_insert(table *t, void *key, void *value)
{
	printf("table_insert\n");

	int pos = array_1d_low(t->entries);
	printf(" %d \n", pos);
	bool same_key = false;
	printf("%s \n", same_key ? "true" : "false");
	int insert_pos = 0;
	printf(" %d \n", insert_pos);
	// Allocate the key/value structure.


	while(pos <= array_1d_high(t->entries)){
		printf("was able to inspect ");
		printf("\n");
		struct table_entry *entry = array_1d_inspect_value(t->entries, pos);
		printf("allocate entry\n");

		if(entry->key == key){
			same_key = true;
			free(entry);
			break;
		}
		/*if (t->key_cmp_func(entry->key, key) == 0){
			same_key = true;
			printf("This key already exists, try again\n");
			free(entry);
			break;
		}*/

		free(entry);
		printf("was able to free entry\n");
		pos++;
	}

	// Set the pointers and insert first in the list. This will
	//if(!same_key){

		// cause table_lookup() to find the latest added value.
		struct table_entry *a = malloc(sizeof(struct table_entry));

		a->key = key;
		a->value = value;

		for(int i = array_1d_low(t->entries); i <= array_1d_high(t->entries);
			i++){
			if (!array_1d_has_value(t->entries, i)){
				insert_pos = i;
				break;
			}
		}

		array_1d_set_value(t->entries, a, insert_pos);
		free(a);
	//}
//}
}

int main(void){

	table t;
	for(int i =1; i < 5; i++){
		t[i] = i;
	}


	table_insert(t, 2, 4);

	return 0;
}
