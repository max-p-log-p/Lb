#include "urlencode.h"

int
main(int argc, char *argv[])
{
	if (argc == 2)
		puts(url_encode(argv[1]));
}
