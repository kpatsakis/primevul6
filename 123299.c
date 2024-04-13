render_404(const char *url, isc_httpdurl_t *urlinfo, const char *querystring,
	   const char *headers, void *arg, unsigned int *retcode,
	   const char **retmsg, const char **mimetype, isc_buffer_t *b,
	   isc_httpdfree_t **freecb, void **freecb_args) {
	static char msg[] = "No such URL.\r\n";

	UNUSED(url);
	UNUSED(urlinfo);
	UNUSED(querystring);
	UNUSED(headers);
	UNUSED(arg);

	*retcode = 404;
	*retmsg = "No such URL";
	*mimetype = "text/plain";
	isc_buffer_reinit(b, msg, strlen(msg));
	isc_buffer_add(b, strlen(msg));
	*freecb = NULL;
	*freecb_args = NULL;

	return (ISC_R_SUCCESS);
}