check_colors(TERMTYPE2 *tp)
{
    char *value;

    if ((max_colors > 0) != (max_pairs > 0)
	|| ((max_colors > max_pairs) && (initialize_pair == 0)))
	_nc_warning("inconsistent values for max_colors (%d) and max_pairs (%d)",
		    max_colors, max_pairs);

    PAIRED(set_foreground, set_background);
    PAIRED(set_a_foreground, set_a_background);
    PAIRED(set_color_pair, initialize_pair);

    if (VALID_STRING(set_foreground)
	&& VALID_STRING(set_a_foreground)) {
	if (!_nc_capcmp(set_foreground, set_a_foreground)) {
	    _nc_warning("expected setf/setaf to be different");
	} else if (same_color(set_foreground, set_a_foreground, max_colors)) {
	    _nc_warning("setf/setaf are equivalent");
	}
    }

    if (VALID_STRING(set_background)
	&& VALID_STRING(set_a_background)) {
	if (!_nc_capcmp(set_background, set_a_background)) {
	    _nc_warning("expected setb/setab to be different");
	} else if (same_color(set_background, set_a_background, max_colors)) {
	    _nc_warning("setb/setab are equivalent");
	}
    }

    /* see: has_colors() */
    if (VALID_NUMERIC(max_colors) && VALID_NUMERIC(max_pairs)
	&& ((VALID_STRING(set_foreground)
	     && VALID_STRING(set_background))
	    || (VALID_STRING(set_a_foreground)
		&& VALID_STRING(set_a_background))
	    || set_color_pair)) {
	if (!VALID_STRING(orig_pair) && !VALID_STRING(orig_colors))
	    _nc_warning("expected either op/oc string for resetting colors");
    }
    if (can_change) {
	if (!VALID_STRING(initialize_pair) &&
	    !VALID_STRING(initialize_color)) {
	    _nc_warning("expected initc or initp because ccc is given");
	}
    } else {
	if (VALID_STRING(initialize_pair) ||
	    VALID_STRING(initialize_color)) {
	    _nc_warning("expected ccc because initc is given");
	}
    }
    value = tigetstr("RGB");
    if (VALID_STRING(value)) {
	int r, g, b;
	char bad;
	int code = sscanf(value, "%d/%d/%d%c", &r, &g, &b, &bad);
	if (code != 3 || r <= 0 || g <= 0 || b <= 0) {
	    _nc_warning("unexpected value for RGB capability: %s", value);
	}
    }
}