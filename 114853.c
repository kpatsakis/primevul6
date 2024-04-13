static size_t all_data_cb(const void *ptr, size_t size, size_t nmemb,
			  void *user_data)
{
	struct data_buffer *db = user_data;
	size_t oldlen, newlen;

	oldlen = db->len;
	if (unlikely(nmemb == 0 || size == 0 || oldlen >= SIZE_MAX - size))
		return 0;
	if (unlikely(nmemb > (SIZE_MAX - oldlen) / size))
		nmemb = (SIZE_MAX - oldlen) / size;

	size_t len = size * nmemb;
	void *newmem;
	static const unsigned char zero = 0;

	if (db->idlemarker) {
		const unsigned char *cptr = ptr;
		for (size_t i = 0; i < len; ++i)
			if (!(isspace(cptr[i]) || cptr[i] == '{')) {
				*db->idlemarker = CURL_SOCKET_BAD;
				db->idlemarker = NULL;
				break;
			}
	}

	newlen = oldlen + len;

	newmem = realloc(db->buf, newlen + 1);
#ifdef DEBUG_DATABUF
	applog(LOG_DEBUG, "data_buffer_write realloc(%p, %lu) => %p", db->buf, (long unsigned)(newlen + 1), newmem);
#endif
	if (!newmem)
		return 0;

	db->buf = newmem;
	db->len = newlen;
	memcpy(db->buf + oldlen, ptr, len);
	memcpy(db->buf + newlen, &zero, 1);	/* null terminate */

	return nmemb;
}