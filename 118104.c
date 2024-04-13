doset()
{
	char buf[BUFSZ], buf2[BUFSZ];
	int i, pass, boolcount, pick_cnt, pick_idx, opt_indx;
	boolean *bool_p;
	winid tmpwin;
	anything any;
	menu_item *pick_list;
	int indexoffset, startpass, endpass;
	boolean setinitial = FALSE, fromfile = FALSE;
	int biggest_name = 0;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);

	any = zeroany;
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings,
		 "Booleans (selecting will toggle value):", MENU_UNSELECTED);
	any.a_int = 0;
	/* first list any other non-modifiable booleans, then modifiable ones */
	for (pass = 0; pass <= 1; pass++)
	    for (i = 0; boolopt[i].name; i++)
		if ((bool_p = boolopt[i].addr) != 0 &&
			((boolopt[i].optflags == DISP_IN_GAME && pass == 0) ||
			 (boolopt[i].optflags == SET_IN_GAME && pass == 1))) {
		    if (bool_p == &flags.female) continue;  /* obsolete */
#ifdef WIZARD
		    if (bool_p == &iflags.sanity_check && !wizard) continue;
		    if (bool_p == &iflags.menu_tab_sep && !wizard) continue;
#endif
		    if (is_wc_option(boolopt[i].name) &&
			!wc_supported(boolopt[i].name)) continue;
		    if (is_wc2_option(boolopt[i].name) &&
			!wc2_supported(boolopt[i].name)) continue;
		    any.a_int = (pass == 0) ? 0 : i + 1;
		    if (!iflags.menu_tab_sep)
			Sprintf(buf, "%s%-13s [%s]",
			    pass == 0 ? "    " : "",
			    boolopt[i].name, *bool_p ? "true" : "false");
		    else
			Sprintf(buf, "%s\t[%s]",
			    boolopt[i].name, *bool_p ? "true" : "false");
		    add_menu(tmpwin, NO_GLYPH, &any, 0, 0,
			     ATR_NONE, buf, MENU_UNSELECTED);
		}

	boolcount = i;
	indexoffset = boolcount;
	any = zeroany;
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, "", MENU_UNSELECTED);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings,
		 "Compounds (selecting will prompt for new value):",
		 MENU_UNSELECTED);

#ifdef notyet /* SYSCF */
/* XXX I think this is still fragile.  Fixing initial/from_file and/or changing
 the SET_* etc to bitmaps will let me make this better. */
	if(wizard)
	    startpass = SET_IN_SYS;
	else
#endif
	startpass = DISP_IN_GAME;
	endpass = SET_IN_GAME;

	/* spin through the options to find the biggest name
	   and adjust the format string accordingly if needed */
	biggest_name = 0;
	for (i = 0; compopt[i].name; i++)
		if (compopt[i].optflags >= startpass && compopt[i].optflags <= endpass &&
		    strlen(compopt[i].name) > (unsigned) biggest_name)
			biggest_name = (int) strlen(compopt[i].name);
	if (biggest_name > 30) biggest_name = 30;
	if (!iflags.menu_tab_sep)
		Sprintf(fmtstr_doset_add_menu, "%%s%%-%ds [%%s]", biggest_name);

	/* deliberately put `playmode', `name', `role', `race', `gender' first
	   (also alignment if anything ever comes before it in compopt[]) */
	doset_add_menu(tmpwin, "playmode", 0);
	doset_add_menu(tmpwin, "name", 0);
	doset_add_menu(tmpwin, "role", 0);
	doset_add_menu(tmpwin, "race", 0);
	doset_add_menu(tmpwin, "gender", 0);

	for (pass = startpass; pass <= endpass; pass++)
	    for (i = 0; compopt[i].name; i++)
		if (compopt[i].optflags == pass) {
			if (!strcmp(compopt[i].name, "playmode") ||
			    !strcmp(compopt[i].name, "name") ||
			    !strcmp(compopt[i].name, "role") ||
			    !strcmp(compopt[i].name, "race") ||
			    !strcmp(compopt[i].name, "gender"))
				continue;
			else if (is_wc_option(compopt[i].name) &&
					!wc_supported(compopt[i].name))
				continue;
			else if (is_wc2_option(compopt[i].name) &&
					!wc2_supported(compopt[i].name))
				continue;
			else
				doset_add_menu(tmpwin, compopt[i].name,
					(pass == DISP_IN_GAME) ? 0 : indexoffset);
		}
#ifdef STATUS_VIA_WINDOWPORT
# ifdef STATUS_HILITES
	any.a_int = -2;
	get_status_hilites(buf2, 60);
	if (!iflags.menu_tab_sep)
		Sprintf(buf, fmtstr_doset_add_menu, any.a_int ? "" : "    ",
			"status_hilites", buf2);
	else
		Sprintf(buf, fmtstr_doset_add_menu_tab, "status_hilites", buf2);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, MENU_UNSELECTED);
# endif
#endif
#ifdef AUTOPICKUP_EXCEPTIONS
	any.a_int = -1;
	Sprintf(buf, "autopickup exceptions (%d currently set)",
		count_ape_maps((int *)0, (int *)0));
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, buf, MENU_UNSELECTED);
#endif /* AUTOPICKUP_EXCEPTIONS */
#ifdef PREFIXES_IN_USE
	any = zeroany;
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE, "", MENU_UNSELECTED);
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings,
		 "Variable playground locations:", MENU_UNSELECTED);
	for (i = 0; i < PREFIX_COUNT; i++)
		doset_add_menu(tmpwin, fqn_prefix_names[i], 0);
#endif
	end_menu(tmpwin, "Set what options?");
	need_redraw = FALSE;
	if ((pick_cnt = select_menu(tmpwin, PICK_ANY, &pick_list)) > 0) {
	    /*
	     * Walk down the selection list and either invert the booleans
	     * or prompt for new values. In most cases, call parseoptions()
	     * to take care of options that require special attention, like
	     * redraws.
	     */
	    for (pick_idx = 0; pick_idx < pick_cnt; ++pick_idx) {
		opt_indx = pick_list[pick_idx].item.a_int - 1;
#ifdef AUTOPICKUP_EXCEPTIONS
		if (opt_indx == -2) {	/* -3 due to -1 offset for select_menu() */
		    (void)special_handling("autopickup_exception",
					   setinitial, fromfile);
		} else
#endif
#ifdef STATUS_VIA_WINDOWPORT
# ifdef STATUS_HILITES
		if (opt_indx == -3) {	/* -3 due to -1 offset for select_menu() */
			if (!status_hilite_menu())
				pline("Bad status hilite(s) specified.");
			else {
				if (wc2_supported("status_hilites"))
					preference_update("status_hilites");
			}
		} else
# endif
#endif
		if (opt_indx < boolcount) {
		    /* boolean option */
		    Sprintf(buf, "%s%s", *boolopt[opt_indx].addr ? "!" : "",
			    boolopt[opt_indx].name);
		    parseoptions(buf, setinitial, fromfile);
		    if (wc_supported(boolopt[opt_indx].name) ||
			wc2_supported(boolopt[opt_indx].name))
			preference_update(boolopt[opt_indx].name);
		} else {
		    /* compound option */
		    opt_indx -= boolcount;

		    if (!special_handling(compopt[opt_indx].name,
							setinitial, fromfile)) {
			Sprintf(buf, "Set %s to what?", compopt[opt_indx].name);
			getlin(buf, buf2);
			if (buf2[0] == '\033')
			    continue;
			Sprintf(buf, "%s:%s", compopt[opt_indx].name, buf2);
			/* pass the buck */
			parseoptions(buf, setinitial, fromfile);
		    }
		    if (wc_supported(compopt[opt_indx].name) ||
			wc2_supported(compopt[opt_indx].name))
			preference_update(compopt[opt_indx].name);
		}
	    }
	    free((genericptr_t)pick_list);
	    pick_list = (menu_item *)0;
	}

	destroy_nhwindow(tmpwin);
	if (need_redraw)
	    (void) doredraw();
	return 0;
}