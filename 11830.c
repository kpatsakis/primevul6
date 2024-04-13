write_it(ENTRY * ep)
{
    unsigned n;
    int ch;
    char *s, *d, *t;
    char result[MAX_ENTRY_SIZE];

    /*
     * Look for strings that contain %{number}, convert them to %'char',
     * which is shorter and runs a little faster.
     */
    for (n = 0; n < STRCOUNT; n++) {
	s = ep->tterm.Strings[n];
	if (VALID_STRING(s)
	    && strchr(s, L_BRACE) != 0) {
	    d = result;
	    t = s;
	    while ((ch = *t++) != 0) {
		*d++ = (char) ch;
		if (ch == '\\') {
		    if ((*d++ = *t++) == '\0')
			break;
		} else if ((ch == '%')
			   && (*t == L_BRACE)) {
		    char *v = 0;
		    long value = strtol(t + 1, &v, 0);
		    if (v != 0
			&& *v == R_BRACE
			&& value > 0
			&& value != '\\'	/* FIXME */
			&& value < 127
			&& isprint((int) value)) {
			*d++ = S_QUOTE;
			*d++ = (char) value;
			*d++ = S_QUOTE;
			t = (v + 1);
		    }
		}
	    }
	    *d = 0;
	    if (strlen(result) < strlen(s))
		_nc_STRCPY(s, result, strlen(s) + 1);
	}
    }

    _nc_set_type(_nc_first_name(ep->tterm.term_names));
    _nc_curr_line = (int) ep->startline;
    _nc_write_entry(&ep->tterm);
}