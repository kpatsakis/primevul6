check_sgr_param(TERMTYPE2 *tp, int code, const char *name, char *value)
{
    if (VALID_STRING(value)) {
	int ncv = ((code != 0) ? (1 << (code - 1)) : 0);
	char *test = tgoto(value, 0, 0);
	if (is_sgr_string(test)) {
	    int param = 0;
	    int count = 0;
	    int skips = 0;
	    int color = (value == set_a_foreground ||
			 value == set_a_background ||
			 value == set_foreground ||
			 value == set_background);
	    while (*test != 0) {
		if (isdigit(UChar(*test))) {
		    param = 10 * param + (*test - '0');
		    ++count;
		} else {
		    if (count) {
			/*
			 * Avoid unnecessary warning for xterm 256color codes.
			 */
			if (color && (param == 38 || param == 48))
			    skips = 3;
			if ((skips-- <= 0) && (param == code))
			    break;
		    }
		    count = 0;
		    param = 0;
		}
		++test;
	    }
	    if (count != 0 && param == code) {
		if (code == 0 ||
		    no_color_video < 0 ||
		    !(no_color_video & ncv)) {
		    _nc_warning("\"%s\" SGR-attribute used in %s",
				sgr_names[code],
				name);
		}
	    }
	}
    }
}