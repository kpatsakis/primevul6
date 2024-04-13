check_infotocap(TERMTYPE2 *tp, int i, const char *value)
{
    const char *name = ExtStrname(tp, i, strnames);
    int params = ((i < (int) SIZEOF(parametrized))
		  ? parametrized[i]
		  : ((*value == 'k')
		     ? 0
		     : has_params(value)));
    int to_char = 0;
    char *ti_value;
    char *tc_value;
    bool embedded;

    assert(SIZEOF(parametrized) == STRCOUNT);
    if ((ti_value = _nc_tic_expand(value, TRUE, to_char)) == ABSENT_STRING) {
	_nc_warning("tic-expansion of %s failed", name);
    } else if ((tc_value = _nc_infotocap(name, ti_value, params)) == ABSENT_STRING) {
	_nc_warning("tic-conversion of %s failed", name);
    } else if (params > 0) {
	int limit = 5;
	int count;
	bool first = TRUE;

	if (!strcmp(name, "setf")
	    || !strcmp(name, "setb")
	    || !strcmp(name, "setaf")
	    || !strcmp(name, "setab")) {
	    if ((limit = max_colors) > 16)
		limit = 16;
	}
	for (count = 0; count < limit; ++count) {
	    char *ti_check = check_1_infotocap(name, ti_value, count);
	    char *tc_check = check_1_infotocap(name, tc_value, count);

	    if (strcmp(ti_check, tc_check)) {
		if (first) {
		    fprintf(stderr, "check_infotocap(%s)\n", name);
		    fprintf(stderr, "...ti '%s'\n", ti_value);
		    fprintf(stderr, "...tc '%s'\n", tc_value);
		    first = FALSE;
		}
		_nc_warning("tparm-conversion of %s(%d) differs between\n\tterminfo %s\n\ttermcap  %s",
			    name, count, ti_check, tc_check);
	    }
	    free(ti_check);
	    free(tc_check);
	}
    } else if (params == 0 && !same_ti_tc(ti_value, tc_value, &embedded)) {
	if (embedded) {
	    _nc_warning("termcap equivalent of %s cannot use embedded delay", name);
	} else {
	    _nc_warning("tic-conversion of %s changed value\n\tfrom %s\n\tto   %s",
			name, ti_value, tc_value);
	}
    }
}