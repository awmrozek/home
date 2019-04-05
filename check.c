#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// låt oss frimodigt träda fram i Skeppstedts spännande värld

typedef struct list_t	list_t;

struct list_t {
	list_t*		succ;
	list_t*		pred;
	void*		data;
};

list_t* mem;

void find_memory () {
	printf("Call to find_memory\n");
	if (!mem) 
		return;
	
	list_t* ptr = mem;
	while(ptr->succ) {
		free(ptr->data);
		ptr = ptr->succ;
		free(ptr);
	}
}

void* xfree(list_t *ptr) {
	ptr->succ = ptr;
	ptr->pred = ptr;

	if (!mem) {
		mem = ptr;
		ptr->succ = ptr;
		ptr->pred = ptr;
	} else {
		list_t* a = mem->succ;
		list_t* b = mem;

		ptr->succ = a;
		ptr->pred = b;
		a->pred = ptr;
		b->succ = ptr;
	}

	/*list_t* iterptr = mem;
	printf ("List after xfree: --------------------------------\n");
	do {
		printf ("pred = 0x%x\tptr = 0x%x\tsucc = 0x%x\tdata = 0x%x\n", 
				iterptr->pred, 
				iterptr,
				iterptr->succ, 
				iterptr->data);
		iterptr = iterptr->succ;
	} while (iterptr != mem);
	printf (" ... round the mountain all the way\n");
	printf ("first = 0x%x\n", mem);
	printf ("--------------------------------------------------\n");*/
}

void* xmalloc(size_t size) {
	void* p = malloc(size);
	if (!p) {
		find_memory();
		p = malloc(size);
	}

	if (!p) {
		fprintf(stderr, "out of memory\n");
		exit(1);
	}
}

void print_list(list_t *);
void mem_stats(list_t* nl) {
	printf("\nMem stats -----------------\n");
	printf("Allocated memory:\n");
	print_list(nl);
	printf("Free cells:\n");
	print_list(mem);
	printf("\n\n");
}

static double sec(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);

	return tv.tv_sec + 1e-6 * tv.tv_usec;
}

int empty(list_t* list)
{
	return list == list->succ;
}

list_t *new_list(void* data)
{
	list_t*		list;

	if (!mem) {
		printf ("creating node.\n");
		list = malloc(sizeof(list_t));
	} else {
		printf("reusing node.\n");
		if (mem->pred == mem && mem->succ == mem) {
			printf ("one node in pool.\n");
			list=mem;
			mem = NULL;
		} else {
			printf ("multiple nodes.\n");
			mem->pred->succ = mem-> succ;
			mem->succ->pred = mem-> pred; 
			list=mem;
			mem=mem->succ;
			list->pred = NULL;
			list->succ = NULL;
		}
	}

	assert(list != NULL);

	list->succ = list->pred = list;
	list->data = data;

	return list;
}

void print_list(list_t* list) {
	if (list == NULL) {
		printf("Empty.\n");
		return;
	}

	printf ("List at 0x%x\n", list);
	list_t* lstptr = list;

	do {
		printf("\tpred = 0x%x\tptr = 0x%x\tsucc = 0x%x\tdata = 0x%x\n",
				lstptr->pred,
				lstptr,
				lstptr->succ,
				lstptr->data);

		lstptr = lstptr->succ;
	} while (lstptr != list);
	printf ("End of list.\n");
}

void add(list_t* list, void* data)
{
	list_t*		link;
	list_t*		temp;

	link		= new_list(data);

	list->pred->succ= link;
	link->succ	= list;
	temp		= list->pred;
	list->pred	= link;
	link->pred	= temp;
}

void take_out(list_t* list)
{
	list->pred->succ = list->succ;
	list->succ->pred = list->pred;
	list->succ = list->pred = list;
}

void* take_out_first(list_t* list)
{
	list_t*	succ;
	void*	data;

	if (list->succ->data == NULL)
		return NULL;

	data = list->succ->data;

	succ = list->succ;
	take_out(succ);
	xfree(succ);

	return data;
}

static size_t nextsize()
{
#if 1
	return rand() % 4096;
#else
	size_t		size;
	static int	i;
	static size_t	v[] = { 24, 520, 32, 32, 72, 8000, 16, 24, 212 };

	size = v[i];

	i = (i + 1) % (sizeof v/ sizeof v[0]);
	
	return size;
#endif
}

static void fail(char* s)
{
	fprintf(stderr, "check: %s\n", s);
	abort();
}

int main () {
	list_t* nl = new_list(101);
	int	i;
	for (i = 1; i < 6; ++i) {
		add(nl, i);
		mem_stats(nl);
	}
	for (i = 1; i < 6; ++i) {
		void* t = take_out_first(nl);
		printf("take = 0x%x\n", t);
		mem_stats(nl);
	}
	printf("Harder tests\n");

	for (i = 10; i < 16; ++i) {
		add(nl, i);
		printf("add = 0x%x\n", i);
		mem_stats(nl);
	}
}

