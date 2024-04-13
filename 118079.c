get_compopt_value(optname, buf)
const char *optname;
char *buf;
{
	char ocl[MAXOCLASSES+1];
	static const char none[] = "(none)", randomrole[] = "random",
		     to_be_done[] = "(to be done)",
		     defopt[] = "default",
		     defbrief[] = "def";
	int i;

	buf[0] = '\0';
	if (!strcmp(optname,"align_message"))
		Sprintf(buf, "%s", iflags.wc_align_message == ALIGN_TOP     ? "top" :
				   iflags.wc_align_message == ALIGN_LEFT    ? "left" :
				   iflags.wc_align_message == ALIGN_BOTTOM  ? "bottom" :
				   iflags.wc_align_message == ALIGN_RIGHT   ? "right" :
				   defopt);
	else if (!strcmp(optname,"align_status"))
		Sprintf(buf, "%s", iflags.wc_align_status == ALIGN_TOP     ? "top" :
				   iflags.wc_align_status == ALIGN_LEFT    ? "left" :
				   iflags.wc_align_status == ALIGN_BOTTOM  ? "bottom" :
				   iflags.wc_align_status == ALIGN_RIGHT   ? "right" :
				   defopt);
	else if (!strcmp(optname,"align"))
		Sprintf(buf, "%s", rolestring(flags.initalign, aligns, adj));
#ifdef WIN32CON
	else if (!strcmp(optname,"altkeyhandler"))
		Sprintf(buf, "%s", iflags.altkeyhandler[0] ?
			iflags.altkeyhandler : "default");
#endif
#ifdef BACKWARD_COMPAT
	else if (!strcmp(optname, "boulder"))
# ifdef UNICODE_DRAWING
		Sprintf(buf, "\\x%04X", iflags.bouldersym ?
# else
		Sprintf(buf, "%c", iflags.bouldersym ?
# endif
			iflags.bouldersym :
			showsyms[(int)objects[BOULDER].oc_class + SYM_OFF_O]);
#endif
	else if (!strcmp(optname, "catname"))
		Sprintf(buf, "%s", catname[0] ? catname : none );
	else if (!strcmp(optname, "disclose")) {
		for (i = 0; i < NUM_DISCLOSURE_OPTIONS; i++) {
			char topt[2];
			if (i) Strcat(buf," ");
			topt[1] = '\0';
			topt[0] = flags.end_disclose[i];
			Strcat(buf, topt);
			topt[0] = disclosure_options[i];
			Strcat(buf, topt);
		}
	}
	else if (!strcmp(optname, "dogname"))
		Sprintf(buf, "%s", dogname[0] ? dogname : none );
	else if (!strcmp(optname, "dungeon"))
		Sprintf(buf, "%s", to_be_done);
	else if (!strcmp(optname, "effects"))
		Sprintf(buf, "%s", to_be_done);
	else if (!strcmp(optname, "font_map"))
		Sprintf(buf, "%s", iflags.wc_font_map ? iflags.wc_font_map : defopt);
	else if (!strcmp(optname, "font_message"))
		Sprintf(buf, "%s", iflags.wc_font_message ? iflags.wc_font_message : defopt);
	else if (!strcmp(optname, "font_status"))
		Sprintf(buf, "%s", iflags.wc_font_status ? iflags.wc_font_status : defopt);
	else if (!strcmp(optname, "font_menu"))
		Sprintf(buf, "%s", iflags.wc_font_menu ? iflags.wc_font_menu : defopt);
	else if (!strcmp(optname, "font_text"))
		Sprintf(buf, "%s", iflags.wc_font_text ? iflags.wc_font_text : defopt);
	else if (!strcmp(optname, "font_size_map")) {
		if (iflags.wc_fontsiz_map) Sprintf(buf, "%d", iflags.wc_fontsiz_map);
		else Strcpy(buf, defopt);
	}
	else if (!strcmp(optname, "font_size_message")) {
		if (iflags.wc_fontsiz_message) Sprintf(buf, "%d",
							iflags.wc_fontsiz_message);
		else Strcpy(buf, defopt);
	}
	else if (!strcmp(optname, "font_size_status")) {
		if (iflags.wc_fontsiz_status) Sprintf(buf, "%d", iflags.wc_fontsiz_status);
		else Strcpy(buf, defopt);
	}
	else if (!strcmp(optname, "font_size_menu")) {
		if (iflags.wc_fontsiz_menu) Sprintf(buf, "%d", iflags.wc_fontsiz_menu);
		else Strcpy(buf, defopt);
	}
	else if (!strcmp(optname, "font_size_text")) {
		if (iflags.wc_fontsiz_text) Sprintf(buf, "%d",iflags.wc_fontsiz_text);
		else Strcpy(buf, defopt);
	}
	else if (!strcmp(optname, "fruit"))
		Sprintf(buf, "%s", pl_fruit);
	else if (!strcmp(optname, "gender"))
		Sprintf(buf, "%s", rolestring(flags.initgend, genders, adj));
	else if (!strcmp(optname, "horsename"))
		Sprintf(buf, "%s", horsename[0] ? horsename : none);
	else if (!strcmp(optname, "map_mode"))
		Sprintf(buf, "%s",
			iflags.wc_map_mode == MAP_MODE_TILES      ? "tiles" :
			iflags.wc_map_mode == MAP_MODE_ASCII4x6   ? "ascii4x6" :
			iflags.wc_map_mode == MAP_MODE_ASCII6x8   ? "ascii6x8" :
			iflags.wc_map_mode == MAP_MODE_ASCII8x8   ? "ascii8x8" :
			iflags.wc_map_mode == MAP_MODE_ASCII16x8  ? "ascii16x8" :
			iflags.wc_map_mode == MAP_MODE_ASCII7x12  ? "ascii7x12" :
			iflags.wc_map_mode == MAP_MODE_ASCII8x12  ? "ascii8x12" :
			iflags.wc_map_mode == MAP_MODE_ASCII16x12 ? "ascii16x12" :
			iflags.wc_map_mode == MAP_MODE_ASCII12x16 ? "ascii12x16" :
			iflags.wc_map_mode == MAP_MODE_ASCII10x18 ? "ascii10x18" :
			iflags.wc_map_mode == MAP_MODE_ASCII_FIT_TO_SCREEN ?
			"fit_to_screen" : defopt);
	else if (!strcmp(optname, "menustyle"))
		Sprintf(buf, "%s", menutype[(int)flags.menu_style] );
	else if (!strcmp(optname, "menu_deselect_all"))
		Sprintf(buf, "%s", to_be_done);
	else if (!strcmp(optname, "menu_deselect_page"))
		Sprintf(buf, "%s", to_be_done);
	else if (!strcmp(optname, "menu_first_page"))
		Sprintf(buf, "%s", to_be_done);
	else if (!strcmp(optname, "menu_invert_all"))
		Sprintf(buf, "%s", to_be_done);
	else if (!strcmp(optname, "menu_headings")) {
		Sprintf(buf, "%s", (iflags.menu_headings == ATR_BOLD) ?
			"bold" :   (iflags.menu_headings == ATR_INVERSE) ?
			"inverse" :   (iflags.menu_headings == ATR_ULINE) ?
			"underline" : "unknown");
	}
	else if (!strcmp(optname, "menu_invert_page"))
		Sprintf(buf, "%s", to_be_done);
	else if (!strcmp(optname, "menu_last_page"))
		Sprintf(buf, "%s", to_be_done);
	else if (!strcmp(optname, "menu_next_page"))
		Sprintf(buf, "%s", to_be_done);
	else if (!strcmp(optname, "menu_previous_page"))
		Sprintf(buf, "%s", to_be_done);
	else if (!strcmp(optname, "menu_search"))
		Sprintf(buf, "%s", to_be_done);
	else if (!strcmp(optname, "menu_select_all"))
		Sprintf(buf, "%s", to_be_done);
	else if (!strcmp(optname, "menu_select_page"))
		Sprintf(buf, "%s", to_be_done);
	else if (!strcmp(optname, "monsters"))
		Sprintf(buf, "%s", to_be_done);
	else if (!strcmp(optname, "msghistory"))
		Sprintf(buf, "%u", iflags.msg_history);
#ifdef TTY_GRAPHICS
	else if (!strcmp(optname, "msg_window"))
		Sprintf(buf, "%s", (iflags.prevmsg_window=='s') ? "single" :
					(iflags.prevmsg_window=='c') ? "combination" :
					(iflags.prevmsg_window=='f') ? "full" : "reversed");
#endif
	else if (!strcmp(optname, "name"))
		Sprintf(buf, "%s", plname);
	else if (!strcmp(optname, "number_pad")) {
		static const char *numpadmodes[] = {
		    "0=off", "1=on",
		    "2=on, MSDOS compatible", "3=on, phone-style layout",
		    "4=on, phone layout, MSDOS compatible",
		    "-1=off, y & z swapped", /*[5]*/
		};
		int indx = Cmd.num_pad ? 
			    (Cmd.phone_layout ? (Cmd.pcHack_compat ? 4 : 3) :
						(Cmd.pcHack_compat ? 2 : 1)) :
			    Cmd.swap_yz ? 5 : 0;

		Strcpy(buf, numpadmodes[indx]);
	} else if (!strcmp(optname, "objects"))
		Sprintf(buf, "%s", to_be_done);
	else if (!strcmp(optname, "packorder")) {
		oc_to_str(flags.inv_order, ocl);
		Sprintf(buf, "%s", ocl);
	     }
#ifdef CHANGE_COLOR
	else if (!strcmp(optname, "palette"))
		Sprintf(buf, "%s", get_color_string());
#endif
	else if (!strcmp(optname, "paranoid_confirmation")) {
		char tmpbuf[QBUFSZ];

		tmpbuf[0] = '\0';
		if (ParanoidConfirm) Strcat(tmpbuf, " Confirm");
		if (ParanoidQuit)    Strcat(tmpbuf, " quit");
		if (ParanoidDie)     Strcat(tmpbuf, " die");
		if (ParanoidBones)   Strcat(tmpbuf, " bones");
		if (ParanoidHit)     Strcat(tmpbuf, " attack");
		if (ParanoidPray)    Strcat(tmpbuf, " pray");
		if (ParanoidRemove)  Strcat(tmpbuf, " Remove");
		Strcpy(buf, tmpbuf[0] ? &tmpbuf[1] : "none");
	} else if (!strcmp(optname, "pettype"))
		Sprintf(buf, "%s", (preferred_pet == 'c') ? "cat" :
				(preferred_pet == 'd') ? "dog" :
				(preferred_pet == 'h') ? "horse" :
				(preferred_pet == 'n') ? "none" : "random");
	else if (!strcmp(optname, "pickup_burden"))
		Sprintf(buf, "%s", burdentype[flags.pickup_burden] );
	else if (!strcmp(optname, "pickup_types")) {
		oc_to_str(flags.pickup_types, ocl);
		Sprintf(buf, "%s", ocl[0] ? ocl : "all" );
	}
	else if (!strcmp(optname, "pile_limit"))
		Sprintf(buf, "%d", flags.pile_limit);
	else if (!strcmp(optname, "playmode")) {
		Strcpy(buf,
		       wizard ? "debug" : discover ? "explore" : "normal");
	}
	else if (!strcmp(optname, "race"))
		Sprintf(buf, "%s", rolestring(flags.initrace, races, noun));
#ifdef REINCARNATION
	else if (!strcmp(optname, "roguesymset")) {
		Sprintf(buf, "%s",
# ifdef LOADSYMSETS
			symset[ROGUESET].name ?
			symset[ROGUESET].name :
# endif
			"default");
		if (currentgraphics == ROGUESET && symset[ROGUESET].name)
			Strcat(buf, ", active");
#endif
	}
	else if (!strcmp(optname, "role"))
		Sprintf(buf, "%s", rolestring(flags.initrole, roles, name.m));
	else if (!strcmp(optname, "runmode"))
		Sprintf(buf, "%s", runmodes[flags.runmode]);
	else if (!strcmp(optname, "scores")) {
		Sprintf(buf, "%d top/%d around%s", flags.end_top,
				flags.end_around, flags.end_own ? "/own" : "");
	}
	else if (!strcmp(optname, "scroll_amount")) {
		if (iflags.wc_scroll_amount) Sprintf(buf, "%d",iflags.wc_scroll_amount);
		else Strcpy(buf, defopt);
	}
	else if (!strcmp(optname, "scroll_margin")) {
		if (iflags.wc_scroll_margin) Sprintf(buf, "%d",iflags.wc_scroll_margin);
		else Strcpy(buf, defopt);
	}
	else if (!strcmp(optname, "player_selection"))
		Sprintf(buf, "%s", iflags.wc_player_selection ? "prompts" : "dialog");
#ifdef MSDOS
	else if (!strcmp(optname, "soundcard"))
		Sprintf(buf, "%s", to_be_done);
#endif
	else if (!strcmp(optname, "suppress_alert")) {
	    if (flags.suppress_alert == 0L)
		Strcpy(buf, none);
	    else
		Sprintf(buf, "%lu.%lu.%lu",
			FEATURE_NOTICE_VER_MAJ,
			FEATURE_NOTICE_VER_MIN,
			FEATURE_NOTICE_VER_PATCH);
	}
	else if (!strcmp(optname, "symset")) {
		Sprintf(buf, "%s",
#ifdef LOADSYMSETS
			symset[PRIMARY].name ?
			symset[PRIMARY].name :
#endif
			"default");
		if (currentgraphics == PRIMARY && symset[PRIMARY].name)
			Strcat(buf, ", active");
	}
	else if (!strcmp(optname, "tile_file"))
		Sprintf(buf, "%s", iflags.wc_tile_file ? iflags.wc_tile_file : defopt);
	else if (!strcmp(optname, "tile_height")) {
		if (iflags.wc_tile_height) Sprintf(buf, "%d",iflags.wc_tile_height);
		else Strcpy(buf, defopt);
	}
	else if (!strcmp(optname, "tile_width")) {
		if (iflags.wc_tile_width) Sprintf(buf, "%d",iflags.wc_tile_width);
		else Strcpy(buf, defopt);
	}
	else if (!strcmp(optname, "traps"))
		Sprintf(buf, "%s", to_be_done);
	else if (!strcmp(optname, "vary_msgcount")) {
		if (iflags.wc_vary_msgcount) Sprintf(buf, "%d",iflags.wc_vary_msgcount);
		else Strcpy(buf, defopt);
	}
#ifdef MSDOS
	else if (!strcmp(optname, "video"))
		Sprintf(buf, "%s", to_be_done);
#endif
#ifdef VIDEOSHADES
	else if (!strcmp(optname, "videoshades"))
		Sprintf(buf, "%s-%s-%s", shade[0],shade[1],shade[2]);
	else if (!strcmp(optname, "videocolors"))
		Sprintf(buf, "%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d",
			ttycolors[CLR_RED], ttycolors[CLR_GREEN],
			ttycolors[CLR_BROWN], ttycolors[CLR_BLUE],
			ttycolors[CLR_MAGENTA], ttycolors[CLR_CYAN],
			ttycolors[CLR_ORANGE], ttycolors[CLR_BRIGHT_GREEN],
			ttycolors[CLR_YELLOW], ttycolors[CLR_BRIGHT_BLUE],
			ttycolors[CLR_BRIGHT_MAGENTA],
			ttycolors[CLR_BRIGHT_CYAN]);
#endif /* VIDEOSHADES */
	else if (!strcmp(optname, "windowtype"))
		Sprintf(buf, "%s", windowprocs.name);
	else if (!strcmp(optname, "windowcolors"))
		Sprintf(buf, "%s/%s %s/%s %s/%s %s/%s",
			iflags.wc_foregrnd_menu    ? iflags.wc_foregrnd_menu : defbrief,
			iflags.wc_backgrnd_menu    ? iflags.wc_backgrnd_menu : defbrief,
			iflags.wc_foregrnd_message ? iflags.wc_foregrnd_message : defbrief,
			iflags.wc_backgrnd_message ? iflags.wc_backgrnd_message : defbrief,
			iflags.wc_foregrnd_status  ? iflags.wc_foregrnd_status : defbrief,
			iflags.wc_backgrnd_status  ? iflags.wc_backgrnd_status : defbrief,
			iflags.wc_foregrnd_text    ? iflags.wc_foregrnd_text : defbrief,
			iflags.wc_backgrnd_text    ? iflags.wc_backgrnd_text : defbrief);
#ifdef PREFIXES_IN_USE
	else {
	    for (i = 0; i < PREFIX_COUNT; ++i)
		if (!strcmp(optname, fqn_prefix_names[i]) && fqn_prefix[i])
			Sprintf(buf, "%s", fqn_prefix[i]);
	}
#endif

	if (buf[0]) return buf;
	else return "unknown";
}