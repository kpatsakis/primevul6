rewrite_sgr(char *s, char *attr)
{
    if (s != 0) {
	if (PRESENT(attr)) {
	    size_t len_s = strlen(s);
	    size_t len_a = strlen(attr);

	    if (len_s > len_a && !strncmp(attr, s, len_a)) {
		unsigned n;
		TR(TRACE_DATABASE, ("rewrite:\n\t%s", s));
		for (n = 0; n < len_s - len_a; ++n) {
		    s[n] = s[n + len_a];
		}
		_nc_STRCPY(s + n, attr, strlen(s) + 1);
		TR(TRACE_DATABASE, ("to:\n\t%s", s));
	    }
	}
	return TRUE;
    }
    return FALSE;		/* oops */
}