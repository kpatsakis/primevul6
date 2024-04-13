tokenizeFormat(String format)
{
    char **result = 0;

    format = x_skip_blanks(format);
    if (*format != '\0') {
	char *blob = x_strdup(format);
	int pass;

	for (pass = 0; pass < 2; ++pass) {
	    int used = 0;
	    int first = 1;
	    int escaped = 0;
	    int squoted = 0;
	    int dquoted = 0;
	    int n;
	    int argc = 0;

	    for (n = 0; format[n] != '\0'; ++n) {
		if (escaped) {
		    blob[used++] = format[n];
		    escaped = 0;
		} else if (format[n] == '"') {
		    if (!squoted) {
			if (!dquoted)
			    blob[used++] = format[n];
			dquoted = !dquoted;
		    }
		} else if (format[n] == '\'') {
		    if (!dquoted) {
			if (!squoted)
			    blob[used++] = format[n];
			squoted = !squoted;
		    }
		} else if (format[n] == '\\') {
		    blob[used++] = format[n];
		    escaped = 1;
		} else {
		    if (first) {
			first = 0;
			if (pass) {
			    result[argc] = &blob[n];
			}
			++argc;
		    }
		    if (isspace((Char) format[n])) {
			first = !isspace((Char) format[n + 1]);
			if (squoted || dquoted) {
			    blob[used++] = format[n];
			} else if (first) {
			    blob[used++] = '\0';
			}
		    } else {
			blob[used++] = format[n];
		    }
		}
	    }
	    blob[used] = '\0';
	    assert(strlen(blob) <= strlen(format));
	    if (!pass) {
		result = TypeCallocN(char *, argc + 1);
		if (result == 0) {
		    free(blob);
		    break;
		}
	    }
	}
    }
#if OPT_TRACE
    if (result) {
	int n;
	TRACE(("tokenizeFormat %s\n", format));
	for (n = 0; result[n]; ++n) {
	    TRACE(("argv[%d] = %s\n", n, result[n]));
	}
    }
#endif

    return result;
}