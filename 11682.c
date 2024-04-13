reset_tabstops(int wide)
{
    if ((init_tabs != 8)
	&& VALID_NUMERIC(init_tabs)
	&& VALID_STRING(set_tab)
	&& VALID_STRING(clear_all_tabs)) {
	int c;

	to_left_margin();
	tputs(clear_all_tabs, 0, out_char);
	if (init_tabs > 1) {
	    if (init_tabs > wide)
		init_tabs = (short) wide;
	    for (c = init_tabs; c < wide; c += init_tabs) {
		fprintf(my_file, "%*s", init_tabs, " ");
		tputs(set_tab, 0, out_char);
	    }
	    to_left_margin();
	}
	return (TRUE);
    }
    return (FALSE);
}