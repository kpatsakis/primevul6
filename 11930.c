trimmed_tab_list(const char *source)
{
    char *result = strdup(source);
    int ch, j, k, last;

    if (result != 0) {
	for (j = k = last = 0; result[j] != 0; ++j) {
	    ch = UChar(result[j]);
	    if (isspace(ch)) {
		if (last == '\0') {
		    continue;
		} else if (isdigit(last) || last == ',') {
		    ch = ',';
		}
	    } else if (ch == ',') {
		;
	    } else {
		if (last == ',')
		    result[k++] = (char) last;
		result[k++] = (char) ch;
	    }
	    last = ch;
	}
	result[k] = '\0';
    }
    return result;
}