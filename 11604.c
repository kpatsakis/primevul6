check_delays(TERMTYPE2 *tp, const char *name, const char *value)
{
    const char *p, *q;
    const char *first = 0;
    const char *last = 0;

    for (p = value; *p != '\0'; ++p) {
	if (p[0] == '$' && p[1] == '<') {
	    const char *base = p + 2;
	    const char *mark = 0;
	    bool maybe = TRUE;
	    bool mixed = FALSE;
	    int proportional = 0;
	    int mandatory = 0;

	    first = p;

	    for (q = base; *q != '\0'; ++q) {
		if (*q == '>') {
		    if (mark == 0)
			mark = q;
		    break;
		} else if (*q == '*' || *q == '/') {
		    if (*q == '*')
			++proportional;
		    if (*q == '/')
			++mandatory;
		    if (mark == 0)
			mark = q;
		} else if (!(isalnum(UChar(*q)) || strchr("+-.", *q) != 0)) {
		    maybe = FALSE;
		    break;
		} else if (proportional || mandatory) {
		    mixed = TRUE;
		}
	    }
	    last = *q ? (q + 1) : q;
	    if (*q == '\0') {
		maybe = FALSE;	/* just an isolated "$<" */
	    } else if (maybe) {
		float check_f;
		char check_c;
		int rc = sscanf(base, "%f%c", &check_f, &check_c);
		if ((rc != 2) || (check_c != *mark) || mixed) {
		    _nc_warning("syntax error in %s delay '%.*s'", name,
				(int) (q - base), base);
		} else if (*name == 'k') {
		    _nc_warning("function-key %s has delay", name);
		} else if (proportional && !line_capability(name)) {
		    _nc_warning("non-line capability using proportional delay: %s", name);
		} else if (!xon_xoff &&
			   !mandatory &&
			   strchr(_nc_first_name(tp->term_names), '+') == 0) {
		    _nc_warning("%s in %s is used since no xon/xoff",
				(proportional
				 ? "proportional delay"
				 : "delay"),
				name);
		}
	    } else {
		p = q - 1;	/* restart scan */
	    }
	}
    }

    if (!strcmp(name, "flash") ||
	!strcmp(name, "beep")) {

	if (first != 0) {
	    if (first == value || *last == 0) {
		/*
		 * Delay is on one end or the other.
		 */
		_nc_warning("expected delay embedded within %s", name);
	    }
	} else {
	    int flag;

	    /*
	     * Check for missing delay when using VT100 reverse-video.
	     * A real VT100 might not need this, but terminal emulators do.
	     */
	    if ((p = skip_DECSCNM(value, &flag)) != 0 &&
		flag > 0 &&
		(q = skip_DECSCNM(p, &flag)) != 0 &&
		flag == 0) {
		_nc_warning("expected a delay in %s", name);
	    }
	}
    }
}