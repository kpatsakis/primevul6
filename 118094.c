option_help()
{
    char buf[BUFSZ], buf2[BUFSZ];
    register int i;
    winid datawin;

    datawin = create_nhwindow(NHW_TEXT);
    Sprintf(buf, "Set options as OPTIONS=<options> in %s", configfile);
    opt_intro[CONFIG_SLOT] = (const char *) buf;
    for (i = 0; opt_intro[i]; i++)
	putstr(datawin, 0, opt_intro[i]);

    /* Boolean options */
    for (i = 0; boolopt[i].name; i++) {
	if (boolopt[i].addr) {
#ifdef WIZARD
	    if (boolopt[i].addr == &iflags.sanity_check && !wizard) continue;
	    if (boolopt[i].addr == &iflags.menu_tab_sep && !wizard) continue;
#endif
	    next_opt(datawin, boolopt[i].name);
	}
    }
    next_opt(datawin, "");

    /* Compound options */
    putstr(datawin, 0, "Compound options:");
    for (i = 0; compopt[i].name; i++) {
	Sprintf(buf2, "`%s'", compopt[i].name);
	Sprintf(buf, "%-20s - %s%c", buf2, compopt[i].descr,
		compopt[i+1].name ? ',' : '.');
	putstr(datawin, 0, buf);
    }

    for (i = 0; opt_epilog[i]; i++)
	putstr(datawin, 0, opt_epilog[i]);

    display_nhwindow(datawin, FALSE);
    destroy_nhwindow(datawin);
    return;
}