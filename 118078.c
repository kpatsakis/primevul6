special_handling(optname, setinitial, setfromfile)
const char *optname;
boolean setinitial,setfromfile;
{
    winid tmpwin;
    anything any;
    int i;
    char buf[BUFSZ];

    /* Special handling of menustyle, pickup_burden, pickup_types,
     * disclose, runmode, msg_window, menu_headings, and number_pad options.
#ifdef AUTOPICKUP_EXCEPTIONS
     * Also takes care of interactive autopickup_exception_handling changes.
#endif
     */
    if (!strcmp("menustyle", optname)) {
	const char *style_name;
	menu_item *style_pick = (menu_item *)0;
	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	for (i = 0; i < SIZE(menutype); i++) {
		style_name = menutype[i];
		/* note: separate `style_name' variable used
		   to avoid an optimizer bug in VAX C V2.3 */
		any.a_int = i + 1;
		add_menu(tmpwin, NO_GLYPH, &any, *style_name, 0,
			 ATR_NONE, style_name, MENU_UNSELECTED);
	}
	end_menu(tmpwin, "Select menustyle:");
	if (select_menu(tmpwin, PICK_ONE, &style_pick) > 0) {
		flags.menu_style = style_pick->item.a_int - 1;
		free((genericptr_t)style_pick);
	}
	destroy_nhwindow(tmpwin);
    } else if (!strcmp("paranoid_confirmation", optname)) {
	menu_item *paranoia_picks = (menu_item *)0;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any = zeroany;
	for (i = 0; paranoia[i].flagmask != 0; ++i) {
	    if (paranoia[i].flagmask == PARANOID_BONES && !wizard) continue;
	    any.a_int = paranoia[i].flagmask;
	    add_menu(tmpwin, NO_GLYPH, &any, *paranoia[i].argname, 0,
		     ATR_NONE, paranoia[i].explain, 
		     (flags.paranoia_bits & paranoia[i].flagmask) ?
			MENU_SELECTED : MENU_UNSELECTED);
	}
	end_menu(tmpwin, "Actions requiring extra confirmation:");
	i = select_menu(tmpwin, PICK_ANY, &paranoia_picks);
	if (i >= 0) {
	    /* player didn't cancel; we reset all the paranoia options
	       here even if there were no items picked, since user
	       could have toggled off preselected ones to end up with 0 */
	    flags.paranoia_bits = 0;
	    if (i > 0) {
		/* at least one item set, either preselected or newly picked */
		while (--i >= 0)
		    flags.paranoia_bits |= paranoia_picks[i].item.a_int;
		free((genericptr_t)paranoia_picks);
	    }
	}
	destroy_nhwindow(tmpwin);
    } else if (!strcmp("pickup_burden", optname)) {
	const char *burden_name, *burden_letters = "ubsntl";
	menu_item *burden_pick = (menu_item *)0;
	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any = zeroany;
	for (i = 0; i < SIZE(burdentype); i++) {
		burden_name = burdentype[i];
		any.a_int = i + 1;
		add_menu(tmpwin, NO_GLYPH, &any, burden_letters[i], 0,
			 ATR_NONE, burden_name, MENU_UNSELECTED);
	}
	end_menu(tmpwin, "Select encumbrance level:");
	if (select_menu(tmpwin, PICK_ONE, &burden_pick) > 0) {
		flags.pickup_burden = burden_pick->item.a_int - 1;
		free((genericptr_t)burden_pick);
	}
	destroy_nhwindow(tmpwin);
    } else if (!strcmp("pickup_types", optname)) {
	/* parseoptions will prompt for the list of types */
	parseoptions(strcpy(buf, "pickup_types"), setinitial, setfromfile);
    } else if (!strcmp("disclose", optname)) {
	int pick_cnt, pick_idx, opt_idx;
	menu_item *disclosure_category_pick = (menu_item *)0;
	/*
	 * The order of disclose_names[]
	 * must correspond to disclosure_options in decl.h
	 */
	static const char *disclosure_names[] = {
		"inventory", "attributes", "vanquished", "genocides", "conduct"
	};
	int disc_cat[NUM_DISCLOSURE_OPTIONS];
	const char *disclosure_name;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any = zeroany;
	for (i = 0; i < NUM_DISCLOSURE_OPTIONS; i++) {
		disclosure_name = disclosure_names[i];
		any.a_int = i + 1;
		add_menu(tmpwin, NO_GLYPH, &any, disclosure_options[i], 0,
			 ATR_NONE, disclosure_name, MENU_UNSELECTED);
		disc_cat[i] = 0;
	}
	end_menu(tmpwin, "Change which disclosure options categories:");
	if ((pick_cnt = select_menu(tmpwin, PICK_ANY, &disclosure_category_pick)) > 0) {
	    for (pick_idx = 0; pick_idx < pick_cnt; ++pick_idx) {
		opt_idx = disclosure_category_pick[pick_idx].item.a_int - 1;
		disc_cat[opt_idx] = 1;
	    }
	    free((genericptr_t)disclosure_category_pick);
	    disclosure_category_pick = (menu_item *)0;
	}
	destroy_nhwindow(tmpwin);

	for (i = 0; i < NUM_DISCLOSURE_OPTIONS; i++) {
	    if (disc_cat[i]) {
		char dbuf[BUFSZ];
		menu_item *disclosure_option_pick = (menu_item *)0;

		Sprintf(dbuf, "Disclosure options for %s:", disclosure_names[i]);
		tmpwin = create_nhwindow(NHW_MENU);
		start_menu(tmpwin);
		any.a_char = DISCLOSE_NO_WITHOUT_PROMPT;
		add_menu(tmpwin, NO_GLYPH, &any, 'a', 0,
			ATR_NONE,"Never disclose and don't prompt", MENU_UNSELECTED);
		any = zeroany;
		any.a_char = DISCLOSE_YES_WITHOUT_PROMPT;
		add_menu(tmpwin, NO_GLYPH, &any, 'b', 0,
			ATR_NONE,"Always disclose and don't prompt", MENU_UNSELECTED);
		any = zeroany;
		any.a_char = DISCLOSE_PROMPT_DEFAULT_NO;
		add_menu(tmpwin, NO_GLYPH, &any, 'c', 0,
			ATR_NONE,"Prompt and default answer to \"No\"", MENU_UNSELECTED);
		any = zeroany;
		any.a_char = DISCLOSE_PROMPT_DEFAULT_YES;
		add_menu(tmpwin, NO_GLYPH, &any, 'd', 0,
			ATR_NONE,"Prompt and default answer to \"Yes\"", MENU_UNSELECTED);
		end_menu(tmpwin, dbuf);
		if (select_menu(tmpwin, PICK_ONE, &disclosure_option_pick) > 0) {
			flags.end_disclose[i] = disclosure_option_pick->item.a_char;
			free((genericptr_t)disclosure_option_pick);
		}
		destroy_nhwindow(tmpwin);
	    }
	}
    } else if (!strcmp("runmode", optname)) {
	const char *mode_name;
	menu_item *mode_pick = (menu_item *)0;
	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any = zeroany;
	for (i = 0; i < SIZE(runmodes); i++) {
		mode_name = runmodes[i];
		any.a_int = i + 1;
		add_menu(tmpwin, NO_GLYPH, &any, *mode_name, 0,
			 ATR_NONE, mode_name, MENU_UNSELECTED);
	}
	end_menu(tmpwin, "Select run/travel display mode:");
	if (select_menu(tmpwin, PICK_ONE, &mode_pick) > 0) {
		flags.runmode = mode_pick->item.a_int - 1;
		free((genericptr_t)mode_pick);
	}
	destroy_nhwindow(tmpwin);
    } else if (!strcmp("msg_window", optname)) {
#ifdef TTY_GRAPHICS
	/* by Christian W. Cooper */
	menu_item *window_pick = (menu_item *)0;
	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any = zeroany;
	any.a_char = 's';
	add_menu(tmpwin, NO_GLYPH, &any, 's', 0,
		ATR_NONE, "single", MENU_UNSELECTED);
	any.a_char = 'c';
	add_menu(tmpwin, NO_GLYPH, &any, 'c', 0,
		ATR_NONE, "combination", MENU_UNSELECTED);
	any.a_char = 'f';
	add_menu(tmpwin, NO_GLYPH, &any, 'f', 0,
		ATR_NONE, "full", MENU_UNSELECTED);
	any.a_char = 'r';
	add_menu(tmpwin, NO_GLYPH, &any, 'r', 0,
		ATR_NONE, "reversed", MENU_UNSELECTED);
	end_menu(tmpwin, "Select message history display type:");
	if (select_menu(tmpwin, PICK_ONE, &window_pick) > 0) {
		iflags.prevmsg_window = window_pick->item.a_char;
		free((genericptr_t)window_pick);
	}
	destroy_nhwindow(tmpwin);
#endif
    } else if (!strcmp("align_message", optname) ||
		!strcmp("align_status", optname)) {
	menu_item *window_pick = (menu_item *)0;
	char abuf[BUFSZ];
	boolean msg = (*(optname+6) == 'm');

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any = zeroany;
	any.a_int = ALIGN_TOP;
	add_menu(tmpwin, NO_GLYPH, &any, 't', 0,
		ATR_NONE, "top", MENU_UNSELECTED);
	any.a_int = ALIGN_BOTTOM;
	add_menu(tmpwin, NO_GLYPH, &any, 'b', 0,
		ATR_NONE, "bottom", MENU_UNSELECTED);
	any.a_int = ALIGN_LEFT;
	add_menu(tmpwin, NO_GLYPH, &any, 'l', 0,
		ATR_NONE, "left", MENU_UNSELECTED);
	any.a_int = ALIGN_RIGHT;
	add_menu(tmpwin, NO_GLYPH, &any, 'r', 0,
		ATR_NONE, "right", MENU_UNSELECTED);
	Sprintf(abuf, "Select %s window placement relative to the map:",
		msg ? "message" : "status");
	end_menu(tmpwin, abuf);
	if (select_menu(tmpwin, PICK_ONE, &window_pick) > 0) {
		if (msg) iflags.wc_align_message = window_pick->item.a_int;
		else iflags.wc_align_status = window_pick->item.a_int;
		free((genericptr_t)window_pick);
	}
	destroy_nhwindow(tmpwin);
    } else if (!strcmp("number_pad", optname)) {
	static const char *npchoices[] = {
		" 0 (off)", " 1 (on)", " 2 (on, MSDOS compatible)",
		" 3 (on, phone-style digit layout)",
		" 4 (on, phone-style layout, MSDOS compatible)",
		"-1 (off, 'z' to move upper-left, 'y' to zap wands)"
	};
	menu_item *mode_pick = (menu_item *)0;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any = zeroany;
	for (i = 0; i < SIZE(npchoices); i++) {
		any.a_int = i + 1;
		add_menu(tmpwin, NO_GLYPH, &any, 'a' + i, 0,
			 ATR_NONE, npchoices[i], MENU_UNSELECTED);
	}
	end_menu(tmpwin, "Select number_pad mode:");
	if (select_menu(tmpwin, PICK_ONE, &mode_pick) > 0) {
		switch (mode_pick->item.a_int - 1) {
		case 0: iflags.num_pad = FALSE; iflags.num_pad_mode = 0; break;
		case 1: iflags.num_pad = TRUE;  iflags.num_pad_mode = 0; break;
		case 2: iflags.num_pad = TRUE;  iflags.num_pad_mode = 1; break;
		case 3: iflags.num_pad = TRUE;  iflags.num_pad_mode = 2; break;
		case 4: iflags.num_pad = TRUE;  iflags.num_pad_mode = 3; break;
			/* last menu choice: number_pad == -1 */
		case 5: iflags.num_pad = FALSE; iflags.num_pad_mode = 1; break;
		}
		reset_commands(FALSE);
		number_pad(iflags.num_pad ? 1 : 0);
		free((genericptr_t)mode_pick);
	}
	destroy_nhwindow(tmpwin);
    } else if (!strcmp("menu_headings", optname)) {
	static const char *mhchoices[3] = {"bold", "inverse", "underline"};
	const char *npletters = "biu";
	menu_item *mode_pick = (menu_item *)0;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any = zeroany;
	for (i = 0; i < SIZE(mhchoices); i++) {
		any.a_int = i + 1;
		add_menu(tmpwin, NO_GLYPH, &any, npletters[i], 0,
			 ATR_NONE, mhchoices[i], MENU_UNSELECTED);
	}
	end_menu(tmpwin, "How to highlight menu headings:");
	if (select_menu(tmpwin, PICK_ONE, &mode_pick) > 0) {
		int mode = mode_pick->item.a_int - 1;
		switch(mode) {
			case 2:
				iflags.menu_headings = ATR_ULINE;
				break;
			case 0:
				iflags.menu_headings = ATR_BOLD;
				break;
			case 1:
			default:
				iflags.menu_headings = ATR_INVERSE;
		}
		free((genericptr_t)mode_pick);
	}
	destroy_nhwindow(tmpwin);
    } else if (!strcmp("autopickup_exception", optname)) {
#ifdef AUTOPICKUP_EXCEPTIONS
	int pick_cnt, pick_idx, opt_idx, pass;
	int totalapes = 0, numapes[2] = {0,0};
	menu_item *pick_list = (menu_item *)0;
	char apebuf[1+BUFSZ];	/* so &apebuf[1] is BUFSZ long for getlin() */
	struct autopickup_exception *ape;
	static const struct ape_action {
	    char letr;
	    const char *desc;
	} action_titles[] = {
	    { 'a', "add new autopickup exception" },		/* [0] */
	    { 'l', "list autopickup exceptions" },		/* [1] */
	    { 'r', "remove existing autopickup exception" },	/* [2] */
	    { 'x', "exit this menu" },				/* [3] */
	};

 ape_again:
	opt_idx = 0;
	totalapes = count_ape_maps(&numapes[AP_LEAVE], &numapes[AP_GRAB]);
	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any = zeroany;
	for (i = 0; i < SIZE(action_titles); i++) {
		any.a_int++;
		/* omit list and remove if there aren't any yet */
		if (!totalapes && (i == 1 || i == 2)) continue;
		add_menu(tmpwin, NO_GLYPH, &any, action_titles[i].letr,
			 0, ATR_NONE, action_titles[i].desc,
#if 0		/* this ought to work but doesn't... */
			 (action_titles[i].letr == 'x') ? MENU_SELECTED :
#endif
			 MENU_UNSELECTED);
	}
	end_menu(tmpwin, "Do what?");
	if ((pick_cnt = select_menu(tmpwin, PICK_ONE, &pick_list)) > 0) {
		for (pick_idx = 0; pick_idx < pick_cnt; ++pick_idx) {
			opt_idx = pick_list[pick_idx].item.a_int - 1;
		}
		free((genericptr_t)pick_list);
		pick_list = (menu_item *)0;
	}
	destroy_nhwindow(tmpwin);

	if (pick_cnt < 1 || opt_idx == 3) {
		;	/* done--fall through to function exit */
	} else if (opt_idx == 0) {	/* add new */
		getlin("What new autopickup exception pattern?", &apebuf[1]);
		mungspaces(&apebuf[1]);	/* regularize whitespace */
		if (apebuf[1] == '\033') {
		    ;	/* fall through to function exit */
		} else {
		    if (apebuf[1]) {
			apebuf[0] = '\"';
			/* guarantee room for \" prefix and \"\0 suffix;
			   -2 is good enough for apebuf[] but -3 makes
			   sure the whole thing fits within normal BUFSZ */
			apebuf[sizeof apebuf - 3] = '\0';
			Strcat(apebuf, "\"");
			add_autopickup_exception(apebuf);
		    }
		    goto ape_again;
		}
	} else {	/* list or remove */
		tmpwin = create_nhwindow(NHW_MENU);
		start_menu(tmpwin);
		for (pass = AP_LEAVE; pass <= AP_GRAB; ++pass) {
		    if (numapes[pass] == 0) continue;
		    ape = iflags.autopickup_exceptions[pass];
		    any = zeroany;
		    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings,
				(pass == 0) ? "Never pickup" : "Always pickup",
				MENU_UNSELECTED);
		    for (i = 0; i < numapes[pass] && ape; i++) {
			any.a_void = (opt_idx == 1) ? 0 : ape;
			Sprintf(apebuf, "\"%s\"", ape->pattern);
			add_menu(tmpwin, NO_GLYPH, &any,
				0, 0, ATR_NONE, apebuf, MENU_UNSELECTED);
			ape = ape->next;
		    }
		}
		Sprintf(apebuf, "%s autopickup exceptions",
			(opt_idx == 1) ? "List of" : "Remove which");
		end_menu(tmpwin, apebuf);
		pick_cnt = select_menu(tmpwin,
					(opt_idx == 1) ?  PICK_NONE : PICK_ANY,
					&pick_list);
		if (pick_cnt > 0) {
		    for (pick_idx = 0; pick_idx < pick_cnt; ++pick_idx)
			remove_autopickup_exception(
	       (struct autopickup_exception *)pick_list[pick_idx].item.a_void);
		}
		free((genericptr_t)pick_list);
		pick_list = (menu_item *)0;
		destroy_nhwindow(tmpwin);
		if (pick_cnt >= 0) goto ape_again;
	}
#endif /* AUTOPICKUP_EXCEPTIONS */
    } else if (!strcmp("symset", optname)
 	    || !strcmp("roguesymset", optname)) {
	menu_item *symset_pick = (menu_item *)0;
	boolean primaryflag = (*optname == 's'),
		  rogueflag = (*optname == 'r'),
		ready_to_switch = FALSE,
		nothing_to_do = FALSE;
#ifdef LOADSYMSETS
	int res;
	char *symset_name, fmtstr[20];
	struct symsetentry *sl;
	int setcount = 0;
#endif
	int chosen = -2, which_set;
 
#ifdef REINCARNATION
	if (rogueflag) which_set = ROGUESET;
	else
#endif
	which_set = PRIMARY;

#ifndef REINCARNATION
	if (rogueflag) return TRUE;
#endif

#ifdef LOADSYMSETS
	/* clear symset[].name as a flag to read_sym_file() to build list */
	symset_name = symset[which_set].name;
	symset[which_set].name = (char *)0;
	symset_list = (struct symsetentry *)0;

	res = read_sym_file(which_set);
	if (res && symset_list) {
		char symsetchoice[BUFSZ];
		int let = 'a', biggest = 0, thissize = 0;
		sl = symset_list;
		while (sl) {
		    /* check restrictions */
		    if ((!rogueflag && sl->rogue)     ||
		        (!iflags.unicodedisp && sl->unicode) ||
			(!primaryflag && sl->primary)) {
		    	sl = sl->next;
		    	continue;
		    }
		    setcount++;
		    /* find biggest name */
		    if (sl->name) thissize = strlen(sl->name);
		    if (thissize > biggest) biggest = thissize;
		    sl = sl->next;
		}
		if (!setcount) {
			pline("There are no appropriate %ssymbol sets available.",
				  (rogueflag) ? "rogue level " :
				(primaryflag) ? "primary "     :
				"");
			return TRUE;
		}

		Sprintf(fmtstr,"%%-%ds %%s", biggest + 5);
		tmpwin = create_nhwindow(NHW_MENU);
		start_menu(tmpwin);
		any.a_int = 1;
		add_menu(tmpwin, NO_GLYPH, &any, let++, 0,
			 ATR_NONE, "Default Symbols", MENU_UNSELECTED);

		sl = symset_list;
		while (sl) {
		    /* check restrictions */
		    if ((!rogueflag && sl->rogue) ||
		        (!iflags.unicodedisp && sl->unicode) ||
			(!primaryflag && sl->primary)) {
		    	sl = sl->next;
		    	continue;
		    }
		    if (sl->name) {
			any.a_int = sl->idx + 2;
			Sprintf(symsetchoice, fmtstr, sl->name,
				sl->desc ? sl->desc : "");
			add_menu(tmpwin, NO_GLYPH, &any, let, 0,
			 ATR_NONE, symsetchoice, MENU_UNSELECTED);
			if (let == 'z') let = 'A';
			else let++;
		    }
		    sl = sl->next;
		}
		end_menu(tmpwin, "Select symbol set:");
		if (select_menu(tmpwin, PICK_ONE, &symset_pick) > 0) {
			chosen = symset_pick->item.a_int - 2;
			free((genericptr_t)symset_pick);
		}
		destroy_nhwindow(tmpwin);

		if (chosen > -1) {
		    /* chose an actual symset name from file */
		    sl = symset_list;
		    while (sl) {
			if (sl->idx == chosen) {
			    if (symset_name) {
					free((genericptr_t)symset_name);
					symset_name = (char *)0;
			    }
			    /* free the now stale attributes */
			    clear_symsetentry(which_set, TRUE);

			    /* transfer only the name of the symbol set */
			    symset[which_set].name =
					(char *)alloc(strlen(sl->name)+1);
			    Strcpy(symset[which_set].name, sl->name);
			    ready_to_switch = TRUE;
			    break;
			}
			sl = sl->next;
		    }
		}

		else if (chosen == -1) {
			/* explicit selection of defaults */
			/* free the now stale symset attributes */
			if (symset_name) free ((genericptr_t)symset_name);
			symset_name = (char *)0;
			clear_symsetentry(which_set, TRUE);
		}
		else nothing_to_do = TRUE;
	} else if (!res) {
		/* The symbols file could not be accessed */
		pline("Unable to access \"%s\" file.", SYMBOLS);
		return TRUE;
	} else if (!symset_list) {
		/* The symbols file was empty */
		pline("There were no symbol sets found in \"%s\".",
			SYMBOLS);
		return TRUE;
	}

	/* clean up */
	while (symset_list) {
		sl = symset_list;
		if (sl->name) free((genericptr_t)sl->name);
		sl->name = (char *)0;

		if (sl->desc) free((genericptr_t)sl->desc);
		sl->desc = (char *)0;

		symset_list = sl->next;
		free((genericptr_t)sl);
	}

	if (nothing_to_do) return TRUE;

	if (!symset[which_set].name && symset_name)
	    symset[which_set].name = symset_name;

	/* Set default symbols and clear the handling value */
# ifdef REINCARNATION
	if(rogueflag)
	    init_r_symbols();
	else
# endif
	init_l_symbols();

	if (symset[which_set].name) {
	    if (read_sym_file(which_set))
		ready_to_switch = TRUE;
	    else {
		clear_symsetentry(which_set, TRUE);
		return TRUE;
	    }
	}

	if (ready_to_switch) switch_symbols(TRUE);

# ifdef REINCARNATION
	if (Is_rogue_level(&u.uz)) {
		if (rogueflag)
		    assign_graphics(ROGUESET);
	} else
# endif
	if (!rogueflag) assign_graphics(PRIMARY);
	need_redraw = TRUE;
#endif /*LOADSYMSETS*/
	return TRUE;

    } else {
	/* didn't match any of the special options */
	return FALSE;
    }
    return TRUE;
}