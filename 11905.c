matches(char **needle, const char *haystack)
/* does entry in needle list match |-separated field in haystack? */
{
    bool code = FALSE;
    size_t n;

    if (needle != 0) {
	for (n = 0; needle[n] != 0; n++) {
	    if (_nc_name_match(haystack, needle[n], "|")) {
		code = TRUE;
		break;
	    }
	}
    } else
	code = TRUE;
    return (code);
}