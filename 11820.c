same_ti_tc(const char *ti, const char *tc, bool * embedded)
{
    bool same = TRUE;
    double ti_delay = 0.0;
    double tc_delay = 0.0;
    const char *ti_last;

    *embedded = FALSE;
    ti_last = parse_ti_delay(ti, &ti_delay);
    tc = parse_tc_delay(tc, &tc_delay);

    while ((ti < ti_last) && *tc) {
	if (*ti == '\\' && ispunct(UChar(ti[1]))) {
	    ++ti;
	    if ((*ti == '^') && !strncmp(tc, "\\136", 4)) {
		ti += 1;
		tc += 4;
		continue;
	    }
	} else if (ti[0] == '$' && ti[1] == '<') {
	    double no_delay;
	    const char *ss = parse_ti_delay(ti, &no_delay);
	    if (ss != ti) {
		*embedded = TRUE;
		ti = ss;
		continue;
	    }
	}
	if (*tc == '\\' && ispunct(UChar(tc[1]))) {
	    ++tc;
	}
	if (*ti++ != *tc++) {
	    same = FALSE;
	    break;
	}
    }

    if (*embedded) {
	if (same) {
	    same = FALSE;
	} else {
	    *embedded = FALSE;	/* report only one problem */
	}
    }

    return same;
}