#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

/*
 * Minimum working example for queue.c.
 *
 * Authors: Niclas Borlin (niclas@cs.umu.se)
 *	    Adam Dahlgren Lindstrom (dali@cs.umu.se)
 *
 * Version information:
 *   2018-01-28: v1.0, first public version.
 *   2018-04-03: v1.1, split into versions with/without memhandler.
 */

// Integers are stored via int pointers stored as void pointers.
// Convert the given pointer and print the dereferenced value.
void print_ints(const void *data)
{
	printf("[%d]", *(int*)data);
}

int main(void) 
{
	// Create the queue.
	queue *q=queue_empty(NULL);
	
	for (int i=1; i<=3; i++) {
		// Allocate memory for one int.
		int *v = malloc(sizeof(*v));
		// Set value.
		*v=i;
		// Put value in queue.
		q=queue_enqueue(q,v);
	}

	printf("--QUEUE before dequeueing--\n");
	queue_print(q, print_ints);

	// Get value on top of queue.
	int *v=queue_front(q);
	// Remote element from queue.
	q=queue_dequeue(q);
	// Free value
	free(v);

	printf("--QUEUE after dequeueing--\n");
	queue_print(q, print_ints);

	// Now we must empty the queue and free each value ourselves.
	while (!queue_is_empty(q)) {
		// Get value from top of queue.
		int *v=queue_front(q);
		// Remove element from queue.
		q=queue_dequeue(q);
		// Free value
		free(v);
	}
	// Finally, destroy the bare queue.
	queue_kill(q);
	
}
