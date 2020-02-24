#include <stdio.h>
#include <stdlib.h>
#include <stack.h>


/*
 * Test program for stack
 *
 * Author: Viktoria Nordkvist (id19vnt@cs.umu.se).
 *
 * Version information:
 *   2020-02-10: 2.0. Second public version.
 *
 */

/*
 * Verifys that a newly created stack is empty. It prints an error message
 * to stderr and exits via exit() with an error signal if the test
 * fails.
 */
void empty_stack_test()
{
	// Create an empty stack.
	stack *s = stack_empty(NULL);

	// Verify that the stack is empty.
	if (!stack_is_empty(s)) {
		// Writes error message to stderr.
		fprintf(stderr, "FAIL: New stack is not empty!\n");
		// Exit with error message.
		exit(EXIT_FAILURE);
	}

	// The implementation passed the test. Clean up.

	// Deallocate the stack structure.
	stack_kill(s);
}

/*
 * Verifys that if one element is inserted into an empty stack, the
 * stack is not empty. Prints an error message to stderr and exits via
 * exit() with an error signal if the test fails.
 */
void one_element_test()
{
	// Create empty stack.
	stack *s = stack_empty(NULL);

	// Create one element.
	int *v = malloc(sizeof(*v));
	*v = 4;

	// Insert element first in stack.
	s = stack_push(s, v);

	// Verify that stack is NOT empty
	if (stack_is_empty(s)) {
		// Write error message to stderr.
		fprintf(stderr, "FAIL: Stack with one element is empty!\n");
		// Exit with error message.
		exit(EXIT_FAILURE);
	}

	// The implementation passed the test. Clean up.

	// Remove all elements from the stack.
	while (!stack_is_empty(s)) {
		int *v = stack_top(s);
		free(v);
		s = stack_pop(s);
	}

	// Deallocate the stack structure.
	stack_kill(s);
}

/*
 * Verifys that if a value is inserted at the top of a empty stack and then
 * removed then we have the original stack again. Prints an error message to
 * stderr and exits via exit() with an error signal if the test fails.
 */
void element_index_test()
{
	// Create empty stack.
	stack *s = stack_empty(NULL);

	// Create one element.
	int *v = malloc(sizeof(*v));
	*v = 4;

	// Insert element first in stack.
	s = stack_push(s, v);

	// Free i
	int *i = stack_top(s);
	free(i);
	//Removes one element.
	s = stack_pop(s);

	//Check if the stack is empty
	if (!stack_is_empty(s)) {
		// Write error message to stderr.
		fprintf(stderr, "FAIL:  Stack with no elements is not empty!\n");
		// Exit with error message.
		exit(EXIT_FAILURE);
    }

	// The implementation passed the test. Clean up.

	// Remove all elements from the stack.
	while (!stack_is_empty(s)) {
		int *v = stack_top(s);
		free(v);
		s = stack_pop(s);
	}

	// Deallocate the stack structure.
	stack_kill(s);

}


/*
 * Verifys that if a value is removed then the following value is now on top of
 * the stack. Prints an error message to stderr and exits via exit() with an
 * error signal if the test fails.
 */
void correct_element_on_top_test()
{
	// Create empty stack.
	stack *s = stack_empty(NULL);

	// Create three element.
	int *i = malloc(sizeof(*i));
	int *j = malloc(sizeof(*j));
	int *k = malloc(sizeof(*k));
	*i = 1;
	*j = 3;
	*k = 5;

	// Insert elements.
	s = stack_push(s, i);
	s = stack_push(s, j);
	s = stack_push(s, k);

	// Free v
	int *v = stack_top(s);
	free(v);

	// Remove one element.
	s = stack_pop(s);

	int *p = stack_top(s);

	//Check if the stack is empty
	if (*p != *j) {
		// Write error message to stderr.
		fprintf(stderr, "FAIL:  Wrong element on top!\n");

		// Remove all elements from the stack.
		while (!stack_is_empty(s)) {
			int *v = stack_top(s);
			free(v);
			s = stack_pop(s);
		}

		// Deallocate the stack structure.
		stack_kill(s);

		// Exit with error message.
		exit(EXIT_FAILURE);
	}

	// The implementation passed the test. Clean up.

	// Remove all elements from the stack.
	while (!stack_is_empty(s)) {
		int *v = stack_top(s);
		free(v);
		s = stack_pop(s);
	}

	// Deallocate the stack structure.
	stack_kill(s);

}


/*
 * Verifys that the value inserted at the top of a empty stack can be
 * retrieved via top. Prints an error message to stderr
 * and exits via exit() with an error signal if the test fails.
 */
void element_value_test()
{
	// Create empty stack.
	stack *s = stack_empty(NULL);

	// Create element with value 4.
	int *v = malloc(sizeof(*v));
	*v = 4;

	// Insert element.
	s = stack_push(s, v);

	// Verify that value 4 is at the top of the stack.
	int *inspected_val = stack_top(s);
	if (*inspected_val != 4) {
		// Write error message to stderr.
		fprintf(stderr, "FAIL: Expected %d first in stack, got %d!\n",
			    4,*inspected_val);
		// Exit with error message.
		exit(EXIT_FAILURE);
	}

	// Empty the stack.
	while (!stack_is_empty(s)) {
		int *v = stack_top(s);
		free(v);
		s = stack_pop(s);
	}

	// Everything OK, clean up after the test. No memory leaks
	// allowed for passed tests.
	stack_kill(s);
}


/*
 * Verifys that if the top element is removed then put back in, it is the
 * same stack as before. Prints an error message to stderr
 * and exits via exit() with an error signal if the test fails.
 */
void top_element_test()
{
	// Create empty stack.
	stack *s = stack_empty(NULL);

	// Create three element.
	int *i = malloc(sizeof(*i));
	int *j = malloc(sizeof(*j));
	int *k = malloc(sizeof(*k));
	*i = 1;
	*j = 3;
	*k = 5;

	// Insert elements.
	s = stack_push(s, i);
	s = stack_push(s, j);
	s = stack_push(s, k);

	// Remove first element
	s = stack_pop(s);

	// Inserts first element again
	s = stack_push(s, k);


	int *p = stack_top(s);
	// Verify that value 4 is at the top of the stack.
	if (*p != *k) {
		// Write error message to stderr.
		fprintf(stderr, "FAIL: Wrong element on top!\n");
		// Exit with error message.
		exit(EXIT_FAILURE);
	}

	// The implementation passed the test. Clean up.

	// Remove all elements from the stack.
	while (!stack_is_empty(s)) {
		int *v = stack_top(s);
		free(v);
		s = stack_pop(s);
	}

	// Deallocate the stack structure.
	stack_kill(s);

}

int main (void)
{

	fprintf(stderr,"Running TEST1: empty_stack_test(): ");
	empty_stack_test();
	fprintf(stderr,"OK.\n");
	fprintf(stderr,"Running TEST2: one_element_test(): ");
	one_element_test();
	fprintf(stderr,"OK.\n");
	fprintf(stderr, "Running TEST3: element_index_test(): ");
	element_index_test();
	fprintf(stderr,"OK.\n");
	fprintf(stderr,"Running TEST4: correct_element_on_top_test(): ");
	correct_element_on_top_test();
	fprintf(stderr,"OK.\n");
	fprintf(stderr,"Running TEST5: element_value_test(): ");
	element_value_test();
	fprintf(stderr,"OK.\n");
	fprintf(stderr,"Running TEST6: top_element_test(): ");
	top_element_test();
	fprintf(stderr,"OK.\n");

	fprintf(stderr,"SUCCESS: Implementation passed all tests. Normal exit.\n");
	return 0;
}
