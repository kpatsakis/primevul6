check_screen(TERMTYPE2 *tp)
{
    if (_nc_user_definable) {
	int have_XT = tigetflag("XT");
	int have_XM = tigetflag("XM");
	int have_bce = back_color_erase;
	bool have_kmouse = FALSE;
	bool use_sgr_39_49 = FALSE;
	char *name = _nc_first_name(tp->term_names);
	bool is_screen = !strncmp(name, "screen", 6);
	bool screen_base = (is_screen
			    && strchr(name, '.') == 0);

	if (!VALID_BOOLEAN(have_bce)) {
	    have_bce = FALSE;
	}
	if (!VALID_BOOLEAN(have_XM)) {
	    have_XM = FALSE;
	}
	if (!VALID_BOOLEAN(have_XT)) {
	    have_XT = FALSE;
	}
	if (VALID_STRING(key_mouse)) {
	    have_kmouse = !strcmp("\033[M", key_mouse);
	}
	if (VALID_STRING(orig_colors)) {
	    use_sgr_39_49 = uses_SGR_39_49(orig_colors);
	} else if (VALID_STRING(orig_pair)) {
	    use_sgr_39_49 = uses_SGR_39_49(orig_pair);
	}

	if (have_XM && have_XT) {
	    _nc_warning("screen's XT capability conflicts with XM");
	} else if (have_XT && screen_base) {
	    _nc_warning("screen's \"screen\" entries should not have XT set");
	} else if (have_XT) {
	    if (!have_kmouse && is_screen) {
		if (VALID_STRING(key_mouse)) {
		    _nc_warning("value of kmous inconsistent with screen's usage");
		} else {
		    _nc_warning("expected kmous capability with XT");
		}
	    }
	    if (!have_bce && max_colors > 0)
		_nc_warning("expected bce capability with XT");
	    if (!use_sgr_39_49 && have_bce && max_colors > 0)
		_nc_warning("expected orig_colors capability with XT to have 39/49 parameters");
	    if (VALID_STRING(to_status_line))
		_nc_warning("\"tsl\" capability is redundant, given XT");
	} else {
	    if (have_kmouse
		&& !have_XM
		&& !screen_base && strchr(name, '+') == 0) {
		_nc_warning("expected XT to be set, given kmous");
	    }
	}
    }
}