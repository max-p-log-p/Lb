#include <curl/curl.h>

#define UA "Mozilla/5.0 (Windows NT 10.0; rv:78.0) Gecko/20100101 Firefox/78.0"
#define SSLOPT CURLSSLOPT_NO_PARTIALCHAIN
#define SSLV CURL_SSLVERSION_TLSv1_3
#define DP "https"
#define PROT CURLPROTO_HTTPS

CURL *handle;

void
cleanup(void)
{
	curl_easy_cleanup(handle);
}

int
main(int argc, char *argv[])
{
	char *urlp;

	curl_global_init(CURL_GLOBAL_SSL);	

	if ((handle = curl_easy_init()) == NULL)
		exit(EXIT_FAILURE);

	atexit(cleanup);

	if (curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L) != CURLE_OK)
		exit(EXIT_FAILURE);
	if (curl_easy_setopt(handle, CURLOPT_ACCEPT_ENCODING, "") != CURLE_OK)
		exit(EXIT_FAILURE);
	if (curl_easy_setopt(handle, CURLOPT_USERAGENT, UA) != CURLE_OK)
		exit(EXIT_FAILURE);
	if (curl_easy_setopt(handle, CURLOPT_SSL_OPTIONS, SSLOPT) != CURLE_OK)
		exit(EXIT_FAILURE);
	if (curl_easy_setopt(handle, CURLOPT_SSL_VERIFYSTATUS, 1L) != CURLE_OK)
		exit(EXIT_FAILURE);
	if (curl_easy_setopt(handle, CURLOPT_SSLVERSION, SSLV) != CURLE_OK)
		exit(EXIT_FAILURE);
	if (curl_easy_setopt(handle, CURLOPT_COOKIEJAR, argv[3]) != CURLE_OK)
		exit(EXIT_FAILURE);
	if (curl_easy_setopt(handle, CURLOPT_COOKIEFILE, argv[2]) != CURLE_OK)
		exit(EXIT_FAILURE);
	if (curl_easy_setopt(handle, CURLOPT_HTTPGET, 1L) != CURLE_OK)
		exit(EXIT_FAILURE);
	if (curl_easy_setopt(handle, CURLOPT_DEFAULT_PROTOCOL, DP) != CURLE_OK)
		exit(EXIT_FAILURE);
	if (curl_easy_setopt(handle, CURLOPT_REDIR_PROTOCOLS, PROT) != CURLE_OK)
		exit(EXIT_FAILURE);
	if (curl_easy_setopt(handle, CURLOPT_PROTOCOLS, PROT) != CURLE_OK)
		exit(EXIT_FAILURE);
	if (curl_easy_setopt(handle, CURLOPT_URL, argv[1]) != CURLE_OK)
		exit(EXIT_FAILURE);
	if (curl_easy_perform(handle) != CURLE_OK)
		exit(EXIT_FAILURE);
	if (curl_easy_getinfo(handle, CURLINFO_EFFECTIVE_URL, &urlp) != CURLE_OK)
		exit(EXIT_FAILURE);

	exit(EXIT_SUCCESS);
}
