decode_tabs(const char *tab_list)
{
    int *result = typeCalloc(int, strlen(tab_list) + (unsigned) max_cols);
    int n = 0;
    int value = 0;
    int prior = 0;
    int ch;

    if (result == 0)
	failed("decode_tabs");

    while ((ch = *tab_list++) != '\0') {
	if (isdigit(UChar(ch))) {
	    value *= 10;
	    value += (ch - '0');
	} else if (ch == ',') {
	    result[n] = value + prior;
	    if (n > 0 && result[n] <= result[n - 1]) {
		fprintf(stderr,
			"%s: tab-stops are not in increasing order: %d %d\n",
			_nc_progname, value, result[n - 1]);
		free(result);
		result = 0;
		break;
	    }
	    ++n;
	    value = 0;
	    prior = 0;
	} else if (ch == '+') {
	    if (n)
		prior = result[n - 1];
	}
    }

    if (result != 0) {
	/*
	 * If there is only one value, then it is an option such as "-8".
	 */
	if ((n == 0) && (value > 0)) {
	    int step = value;
	    value = 1;
	    while (n < max_cols - 1) {
		result[n++] = value;
		value += step;
	    }
	}

	/*
	 * Add the last value, if any.
	 */
	result[n++] = value + prior;
	result[n] = 0;
    }

    return result;
}