have_header(isc_httpd_t *httpd, const char *header, const char *value,
	    const char *eov, const char **fvalue) {
	char *cr, *nl, *h;
	size_t hlen, vlen = 0;

	h = httpd->headers;
	hlen = strlen(header);
	if (value != NULL) {
		INSIST(eov != NULL);
		vlen = strlen(value);
	}

	for (;;) {
		if (strncasecmp(h, header, hlen) != 0) {
			/*
			 * Skip to next line;
			 */
			cr = strchr(h, '\r');
			if (cr != NULL && cr[1] == '\n') {
				cr++;
			}
			nl = strchr(h, '\n');

			/* last header? */
			h = cr;
			if (h == NULL || (nl != NULL && nl < h)) {
				h = nl;
			}
			if (h == NULL) {
				return (false);
			}
			h++;
			continue;
		}

		/*
		 * Skip optional leading white space.
		 */
		h += hlen;
		while (*h == ' ' || *h == '\t') {
			h++;
		}

		/*
		 * Set the found value.
		 */
		if (fvalue != NULL) {
			*fvalue = h;
		}

		if (value == NULL) {
			return (true);
		}

		/*
		 * Terminate token search on NULL or EOL.
		 */
		while (*h != 0 && *h != '\r' && *h != '\n') {
			if (strncasecmp(h, value, vlen) == 0) {
				if (strchr(eov, h[vlen]) != NULL) {
					return (true);
					/*
					 * Skip to next token.
					 */
				}
			}
			/*
			 * Skip to next token.
			 */
			h += strcspn(h, eov);
			if (h[0] == '\r' && h[1] == '\n') {
				h++;
			}
			if (h[0] != 0) {
				h++;
			}
		}

		return (false);
	}
}