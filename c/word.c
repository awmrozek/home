#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main () {
	int	clen;
	int	len;
	char	c;

	len = 0;
	clen = 0;
	c = 0;

	while ((c = getc(stdin)) != EOF) {
		if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
			clen++;
		} else {
			if (clen > len)
				len = clen;
			clen = 0;
		}
	}
	// asdfasdfasdf 12
	// %i characters in longest word: %s\n
	printf("%d\n", len);
}

