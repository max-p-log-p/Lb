#include <ctype.h>
#include <limits.h>
#include <err.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BITS_IN_HEX 4
#define HEX "0123456789ABCDEF"
#define UNRESERVED(c) (isalnum(c) || c == '-' || c == '.' || c == '_' || c == '~')

/* RFC 3986 */
const char *
url_encode(const char *path)
{
	char *epath;
	int i, j;
	size_t newlen;

	newlen = strlen(path);

	/* Calculate length */
	for (i = 0; path[i] != '\0'; ++i)
		if (!UNRESERVED(path[i]))
			newlen += 2;
	
	if ((epath = malloc(newlen + 1)) == NULL) /* Null byte */
		err(1, "malloc");

	/* Encode */
	for (i = 0, j = 0; path[i] != '\0'; ++i) {
		if (UNRESERVED(path[i]))
			epath[j++] = path[i];
		else {
			epath[j] = '%';
			epath[j + 1] = HEX[(path[i] >> BITS_IN_HEX) & '\x0f'];
			epath[j + 2] = HEX[path[i] & '\x0f'];
			j += 3;
		}
	}

	epath[newlen] = '\0';

	return epath;
}
