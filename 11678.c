stripped(char *src)
{
    char *dst = 0;

    while (isspace(UChar(*src)))
	src++;

    if (*src != '\0') {
	size_t len;

	if ((dst = strdup(src)) == NULL) {
	    failed("strdup");
	} else {
	    len = strlen(dst);
	    while (--len != 0 && isspace(UChar(dst[len])))
		dst[len] = '\0';
	}
    }
    return dst;
}