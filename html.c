#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <inttypes.h>
#include <libxml/HTMLparser.h>
#include <libxml/uri.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#include "urlencode.h"

#ifdef OPENBSD
#include <unistd.h>
#define PLEDGE 1
#define UNVEIL 1
#endif

/*
 * ASCII whitespace is U+0009 TAB, U+000A LF, U+000C FF, U+000D CR, 
 * or U+0020 SPACE
 */
#define isaspace(c) (c != '\v' && isspace(c))

#define GREEN "\033[32m"
#define MAX_URIS 512 /* Default maximum number of uris */
#define OPTIONS HTML_PARSE_NOERROR | HTML_PARSE_NODEFDTD | HTML_PARSE_NONET
#define PURPLE "\033[35m"
#define RESET "\033[39m" /* Default foreground color */

typedef enum { 
	APPLICATION_X_WWW_FORM_URLENCODED = 'a', MULTIPART_FORM_DATA = 'm',
	TEXT_PLAIN = 't'
} enctype_t;

typedef enum { GET = '?', POST = ' ' } method_t;

typedef enum { A, BUTTON, FORM, INPUT, OPTION, SELECT, TEXTAREA, DEFAULT } tag_t;

struct _form {
	xmlChar *action;
	method_t method;
	enctype_t enctype;
	xmlChar *data;
	int data_len;
};

/* Add input type using a union */

struct _input {
	xmlChar *name;
	xmlChar *value;
};

struct tag {
	const char *name;
	tag_t type;
} tags[] = {
	{"a", A},
	{"button", BUTTON},
	{"form", FORM},
	{"input", INPUT},
	{"option", OPTION},
	{"select", SELECT},
	{"textarea", TEXTAREA},
};

#define NUM_TAGS (sizeof(tags) / sizeof(tags[0]))

htmlDocPtr doc;
xmlChar **uris;
struct _form form;
intmax_t max_uris;
int num_uris = 1;

xmlChar *
get_val(const xmlAttr *attr, const xmlChar *name)
{
	for (; attr != NULL; attr = attr->next) {
		if (xmlStrcasecmp(attr->name, name) == 0) { 
			return (attr->children) ? attr->children->content : 
			    BAD_CAST("");
		}
	}

	return NULL;
}

void
add_uri(const xmlChar *uri)
{
	if (num_uris < max_uris) {
		if ((uris[num_uris] = xmlBuildURI(uri, uris[0])) == NULL)
			uris[num_uris] = BAD_CAST("");
		++num_uris;
	} else
		exit(EXIT_FAILURE);
}

void
strip(xmlChar *uri)
{
	xmlChar *nwsc;

	/* get first non whitespace character */
	for (nwsc = uri; isaspace(*nwsc); ++nwsc)
		;

	/* copy from first non whitespace character to last non whitespace 
	 * character that is not a null byte to index 0 
	 */
	while (*nwsc != '\0' && !isaspace(*nwsc))
		*uri++ = *nwsc++;

	*uri = '\0';
}

void
handle_a(const xmlAttr *properties)
{
	xmlChar *href;
	if ((href = get_val(properties, BAD_CAST("href"))) != NULL) {
		/* 
		 * The href attribute must have a value that is 
		 * a valid URL potentially surrounded by spaces
		 */
		strip(href);
		add_uri(href);
		printf(GREEN "%d" RESET " ", num_uris);
	}
}

#define chk_val(attr, val) xmlStrncasecmp(attr, BAD_CAST(val), sizeof(BAD_CAST(val)))

void
handle_form(const xmlAttr *properties)
{
	xmlChar *method, *enctype;
	if ((method = get_val(properties, BAD_CAST("method"))) != NULL) {
		if (chk_val(method, "get") == 0)
			form.method = GET;
		else if (chk_val(method, "post") == 0)
			form.method = POST;
		/* Dialog forms are not submitted */
		else if (chk_val(method, "dialog") == 0)
			return;
		else
			form.method = GET;
	} else 
		form.method = GET;
	/*
	 * The action attribute must have a value that is a valid 
	 * non-empty URL potentially surrounded by spaces
	 */
	if ((form.action = get_val(properties, BAD_CAST("action"))) != NULL)
		strip(form.action);
	if ((enctype = get_val(properties, BAD_CAST("enctype"))) != NULL) {
		if (chk_val(enctype, "application/x-www-form-urlencoded") == 0)
			form.enctype = APPLICATION_X_WWW_FORM_URLENCODED;
		else if (chk_val(enctype, "multipart/form-data") == 0)
			form.enctype = MULTIPART_FORM_DATA;
		else if (chk_val(enctype, "text/plain") == 0)
			form.enctype = TEXT_PLAIN;
		else
			form.enctype = APPLICATION_X_WWW_FORM_URLENCODED;
	} else
		form.enctype = APPLICATION_X_WWW_FORM_URLENCODED;
}

void
handle_input(const xmlAttr *properties)
{
	struct _input input;
	xmlChar *value, *buf;
	int len;

	if (form.action == NULL)
		return;

	if ((input.name = get_val(properties, BAD_CAST("name"))) == NULL)
		return;

	if ((value = get_val(properties, BAD_CAST("value"))) != NULL)
		input.value = BAD_CAST(url_encode((const char *)value));
	else
		input.value = BAD_CAST("");

	len = xmlStrlen(input.name) + 1 + xmlStrlen(input.value) + 2;
	/* Free? */
	if ((buf = malloc(len)) == NULL)
		errx(1, "handle_input malloc");

	if ((len = xmlStrPrintf(buf, len, "%s=%s&", input.name, input.value)) < 0)
		errx(1, "handle_input xmlStrPrintf");
	if ((form.data = xmlStrncat(form.data, buf, len)) == NULL)
		errx(1, "handle_input xmlStrncat");
	form.data_len += len;
}

void
print_text(const xmlChar *text)
{
	int i, isPrevSpace;

	for (i = 0, isPrevSpace = 1; text[i] != '\0'; ++i) {
		/* If previous character was not whitespace 
		 * or current character is not whitespace, 
		 * and current character is printable, 
		 * print the character
		 */
		if (!isPrevSpace || !iswspace(text[i]))
			putwchar((text[i] == '\r') ? '\n' : text[i]);
		isPrevSpace = iswspace(text[i]);
	}
	if (!isPrevSpace)
		puts("");
}

void
end_tag(const xmlNode *node)
{
	int len;
	xmlChar *buf, *uri;

	if (chk_val(node->name, "form") == 0) {
		if ((uri = xmlBuildURI(form.action, uris[0])) == NULL)
			errx(1, "end_tag xmlBuildURI");

		/* Strip trailing & */
		form.data[form.data_len] = '\0';

		len = xmlStrlen(uri) + 1 + (form.data_len - 1) + 1;

		/* Free? */
		if ((buf = malloc(len)) == NULL)
			errx(1, "end_tag malloc");		

		if (xmlStrPrintf(buf, len, "%s%c%s", uri, form.method, form.data) > 0)
			uris[num_uris++] = buf;
		else
			errx(1, "end_tag xmlStrPrintf");

		printf(PURPLE "%d%c %s" RESET "\n", num_uris, form.enctype, form.action);

		*form.data = '\0';
		form.data_len = 0;
	}
}

tag_t
search_tag(const char *tag_name)
{
	int low, mid, high, res;
	low = 0;
	high = NUM_TAGS - 1;
	while (low <= high) {
		mid = (high + low) / 2;
		if ((res = strcmp(tag_name, tags[mid].name)) < 0)
			high = mid - 1;
		else if (res > 0)
			low = mid + 1;
		else
			return tags[mid].type;
	}
	return DEFAULT;
}

void
handle_node(const xmlNode *node)
{
	switch (node->type) {
	case XML_ELEMENT_NODE:
		switch (search_tag((const char *)node->name)) {
		case A:
			handle_a(node->properties);
			break;
		case BUTTON:
		case FORM:
			handle_form(node->properties);
			break;
		case INPUT:
			handle_input(node->properties);
		case OPTION:
		case SELECT:
		case TEXTAREA:
		default:
			break;
		}
		break;
	case XML_TEXT_NODE:
		if (node->content == NULL)
			break;
		print_text(node->content);
	default:
		break;
	}
}

/* Preorder Depth First Traversal */
void
pre_dft(const xmlNode *root)
{
	const xmlNode *cur;

	cur = root;

	while (cur != NULL) {
		/* Root */
		handle_node(cur);

		/* Left */
		for (; cur->children != NULL; cur = cur->children)
			handle_node(cur->children);

		/* Right */
		while (cur->next != NULL) {
			handle_node(cur->next);
			cur = cur->next;

			/* Left */
			for (; cur->children != NULL; cur = cur->children)
				handle_node(cur->children);
		}

		/* Up */
		while (cur->next == NULL && cur->parent != NULL) {
			end_tag(cur->parent);
			cur = cur->parent;
		}

		cur = cur->next;
	}
}

void
free_doc(void)
{
	xmlFreeDoc(doc);
}

void
free_uris(void)
{
	free(uris);
}

void
free_uri0(void)
{
	free(uris[0]);
}

int
main(int argc, char *argv[])
{
	xmlNode *root;
	char *endptr;
	size_t n;

#ifdef UNVEIL
	if (unveil("/dev/stdin", "r") == -1)
		errx(1, "unveil");
#endif
#ifdef PLEDGE
	if (pledge("stdio rpath", NULL) == -1)
		errx(1, "pledge");
#endif

	errno = n = 0;
	endptr = NULL;
	max_uris = (argc >= 2) ? strtoimax(argv[1], &endptr, 10) : MAX_URIS;

	if (max_uris <= 0 || errno != 0)
		errx(1, "invalid number of urls");

	if (endptr != NULL && *endptr != '\0')
		errx(1, "invalid character: %c", *endptr);

	if (sizeof(xmlChar *) && max_uris > INT_MAX / sizeof(xmlChar *))
		errx(1, "overflow");

	if ((uris = malloc(sizeof(xmlChar *) * max_uris)) == NULL)
		err(1, "malloc");

	if (atexit(free_uris) != 0) {
		free(uris);
		errx(1, "atexit free_uris");
	}
	
	uris[0] = NULL;
	if (getline((char **)&uris[0], &n, stdin) == -1)
		err(1, "getline");

	/* Delete newline for xmlBuildURL */
	uris[0][strcspn((const char *)uris[0], "\n")] = '\0';

	if (atexit(free_uri0) == -1) {
		free(uris[0]);
		errx(1, "atexit freeuri0");
	}

	if ((doc = htmlReadFd(0, (const char *)uris[0], NULL, OPTIONS)) == NULL)
		errx(1, "htmlReadFile");

#ifdef PLEDGE
	if (pledge("stdio", NULL) == -1)
		errx(1, "pledge");
#endif

	if (atexit(free_doc) != 0) {
		xmlFreeDoc(doc);
		errx(1, "atexit free_doc");
	}

	if ((root = xmlDocGetRootElement(doc)) == NULL)
		errx(1, "no root element");

	pre_dft(root);

	for (--num_uris; num_uris >= 0; --num_uris)
		printf("\n%s", uris[num_uris]);

	exit(EXIT_SUCCESS);
}
