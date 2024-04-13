static char *add_name(char *buf, char *s, const char *name)
#endif /* __SOLARIS__ */
{
    size_t len = strlen(name);

#ifndef __SOLARIS__
    if (s - len <= *buf) {
	unsigned pathlen = *bufsize - (s - *buf);
	unsigned newbufsize = *bufsize;
	char *newbuf;

	while (newbufsize < pathlen + len + 1) {
	    if (newbufsize >= 0x80000000)
	    	newbufsize = 0xffffffff;
	    else
	    	newbufsize *= 2;
	}

	newbuf = realloc(*buf, newbufsize);
	if (newbuf == NULL)
		return NULL;

	*buf = newbuf;
	s = newbuf + newbufsize - pathlen;
	memmove(s, newbuf + *bufsize - pathlen, pathlen);
	*bufsize = newbufsize;
    }
    s -= len;
#else /* ! __SOLARIS__ */
    s -= len;
    if (s <= buf) {
        fprintf(stderr, "fuse: path too long: ...%s\n", s + len);
        return NULL;
    }
#endif /* __SOLARIS__ */
    memcpy(s, name, len);
    s--;
    *s = '/';

    return s;
}