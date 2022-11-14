/* RFC 3986 */
#include <ctype.h>
#include <err.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BITS_IN_HEX 4

/* URI normalizers should use uppercase hexadecimal digits" */
#define HEX "0123456789ABCDEF"

#define UNRESERVED(c) (isalnum(c) || c == '-' || c == '.' || c == '_' || c == '~')

size_t
url_encode(const char *path, char *epath, size_t len)
{
	size_t i, j;

	for (i = 0, j = 0; i < len; ++i) {
		if (UNRESERVED(path[i]))
			epath[j++] = path[i];
		else {
			/* pct-encoded = "%" HEXDIG HEXDIG */
			epath[j] = '%';
			epath[j + 1] = HEX[(path[i] >> BITS_IN_HEX) & '\x0f'];
			epath[j + 2] = HEX[path[i] & '\x0f'];
			j += 3;
		}
	}

	/* return new length */
	return j;
}

int
main()
{
	size_t count, nr;
#ifdef LINE_MAX
	char path[LINE_MAX] = { 0 };
#else
	char path[2048] = { 0 };
#endif
	char epath[sizeof(path) * 3] = { 0 };

#ifdef PLEDGE
	if (pledge("stdio rpath", NULL))
		err(1, "pledge");
#endif
#ifdef UNVEIL
	if (unveil("/dev/stdin", "r"))
		err(1, "unveil");
#endif
	while ((nr = read(STDIN_FILENO, path, sizeof(path))) > 0) {
		count = url_encode(path, epath, nr);
		if (write(STDOUT_FILENO, epath, count) < 0)
			err(1, "write");
	}
}
