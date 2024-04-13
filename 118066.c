doset_add_menu(win, option, indexoffset)
    winid win;			/* window to add to */
    const char *option;		/* option name */
    int indexoffset;		/* value to add to index in compopt[], or zero
				   if option cannot be changed */
{
    const char *value = "unknown";		/* current value */
    char buf[BUFSZ], buf2[BUFSZ];
    anything any;
    int i;

    any = zeroany;
    if (indexoffset == 0) {
	any.a_int = 0;
	value = get_compopt_value(option, buf2);
    } else {
	for (i=0; compopt[i].name; i++)
	    if (strcmp(option, compopt[i].name) == 0) break;

	if (compopt[i].name) {
	    any.a_int = i + 1 + indexoffset;
	    value = get_compopt_value(option, buf2);
	} else {
	    /* We are trying to add an option not found in compopt[].
	       This is almost certainly bad, but we'll let it through anyway
	       (with a zero value, so it can't be selected). */
	    any.a_int = 0;
	}
    }
    /* "    " replaces "a - " -- assumes menus follow that style */
    if (!iflags.menu_tab_sep)
	Sprintf(buf, fmtstr_doset_add_menu, any.a_int ? "" : "    ", option, value);
    else
	Sprintf(buf, fmtstr_doset_add_menu_tab, option, value);
    add_menu(win, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, MENU_UNSELECTED);
}