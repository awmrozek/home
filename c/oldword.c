#include <stdio.h>
#include <ctype.h>

typedef struct ll {
	char c;
	struct ll* next;
} ll_t;

ll_t *head;
ll_t *head_old;

int len;
int len_old;

//ll_t *tail;
//ll_t *tail_old;

void print_list (ll_t* list) {
	ll_t *h = list;

	while (h != NULL) {
		printf ("%c", (*h).c);
		h = (*h).next;
	}
	printf ("\n");
}

int len2 (ll_t* list) {
	if (list == NULL)
		return 0;

	int c = 0;
	ll_t* l = list;
	while (l != NULL) {
		l = (*l).next;
		c++;
	}
	return c;
}

void del (ll_t* list) {
	ll_t* l = list;
	//printf( "deleting %x\n", list);
	while (l != NULL) {
		ll_t* next = (*l).next;
		free (l);
		l = next;
	}
}

int main(void)
{
	char c;
	while ((c = getchar()) != EOF) {
		// is word
		if (isalpha (c) != 0) {
			if (head == NULL) {
				head = malloc(sizeof(ll_t));
				head->c = c;
				head->next = NULL;
				len = 1;
			} else {
				ll_t *cur = head;
				while (cur->next != NULL) cur = (*cur).next;
				(*cur).next = malloc (sizeof(ll_t));
				ll_t *n = (*cur).next;
				(*n).next = NULL;
				(*n).c = c;
				len++;
			}
		} else {
			//printf("len: %d\n", len(head));
			//print_list (head);
			// new word

			// get old len
			int old = len_old;
			int new = len;

			//printf ("old : %d new : %d\n", old, new)
			if (old >= new) {
				/*printf ("old better than new. bypass\n");
				// old better than new
				// : save old clear new	*/
				del (head);
				head = NULL;
				len = 0;
			} else if (new > old) {
				//printf ("new favourite!\n");
				del (head_old);
				head_old = head;
				len_old = len;
				head = NULL;
				len = 0;

				//printf ("old %d new %d\n", len (head_old), len(head));
				//print_list (head_old);
				//print_list (head);

			}
		}
	}

	int old = len_old;
	int new = len;

	if (old > new) {
//		printf ("%d", old);
		print_list (head_old);
	} else {
//		printf ("%d", new);
		print_list (head);
	}

	printf ("\n");
	return 0;
}

