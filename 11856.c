legal_tab_list(const char *tab_list)
{
    bool result = TRUE;

    if (tab_list != 0 && *tab_list != '\0') {
	if (comma_is_needed(tab_list)) {
	    int n, ch;
	    for (n = 0; tab_list[n] != '\0'; ++n) {
		ch = UChar(tab_list[n]);
		if (!(isdigit(ch) || ch == ',' || ch == '+')) {
		    fprintf(stderr,
			    "%s: unexpected character found '%c'\n",
			    _nc_progname, ch);
		    result = FALSE;
		    break;
		}
	    }
	} else {
	    fprintf(stderr, "%s: trailing comma found '%s'\n", _nc_progname, tab_list);
	    result = FALSE;
	}
    } else {
	fprintf(stderr, "%s: no tab-list given\n", _nc_progname);
	result = FALSE;
    }
    return result;
}