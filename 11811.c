main(int argc, char *argv[])
{
    int rc = EXIT_FAILURE;
    bool debug = FALSE;
    bool no_op = FALSE;
    int n, ch;
    NCURSES_CONST char *term_name = 0;
    char *append = 0;
    const char *tab_list = 0;
    TTY tty_settings;
    int fd;

    _nc_progname = _nc_rootname(argv[0]);

    fd = save_tty_settings(&tty_settings, FALSE);

    if ((term_name = getenv("TERM")) == 0)
	term_name = "ansi+tabs";

    /* cannot use getopt, since some options are two-character */
    for (n = 1; n < argc; ++n) {
	char *option = argv[n];
	switch (option[0]) {
	case '-':
	    while ((ch = *++option) != '\0') {
		switch (ch) {
		case 'a':
		    switch (*++option) {
		    default:
		    case '\0':
			tab_list = "1,10,16,36,72";
			option--;
			/* Assembler, IBM S/370, first format */
			break;
		    case '2':
			tab_list = "1,10,16,40,72";
			/* Assembler, IBM S/370, second format */
			break;
		    }
		    break;
		case 'c':
		    switch (*++option) {
		    default:
		    case '\0':
			tab_list = "1,8,12,16,20,55";
			option--;
			/* COBOL, normal format */
			break;
		    case '2':
			tab_list = "1,6,10,14,49";
			/* COBOL compact format */
			break;
		    case '3':
			tab_list = "1,6,10,14,18,22,26,30,34,38,42,46,50,54,58,62,67";
			/* COBOL compact format extended */
			break;
		    }
		    break;
		case 'd':	/* ncurses extension */
		    debug = TRUE;
		    break;
		case 'f':
		    tab_list = "1,7,11,15,19,23";
		    /* FORTRAN */
		    break;
		case 'n':	/* ncurses extension */
		    no_op = TRUE;
		    break;
		case 'p':
		    tab_list = "1,5,9,13,17,21,25,29,33,37,41,45,49,53,57,61";
		    /* PL/I */
		    break;
		case 's':
		    tab_list = "1,10,55";
		    /* SNOBOL */
		    break;
		case 'u':
		    tab_list = "1,12,20,44";
		    /* UNIVAC 1100 Assembler */
		    break;
		case 'T':
		    ++n;
		    if (*++option != '\0') {
			term_name = option;
		    } else {
			term_name = argv[n];
			option--;
		    }
		    option += ((int) strlen(option)) - 1;
		    continue;
		case 'V':
		    puts(curses_version());
		    ExitProgram(EXIT_SUCCESS);
		default:
		    if (isdigit(UChar(*option))) {
			char *copy = strdup(option);
			*skip_list(copy) = '\0';
			tab_list = copy;
			option = skip_list(option) - 1;
		    } else {
			usage();
		    }
		    break;
		}
	    }
	    break;
	case '+':
	    while ((ch = *++option) != '\0') {
		switch (ch) {
		case 'm':
		    /*
		     * The "+mXXX" option is unimplemented because only the long-obsolete
		     * att510d implements smgl, which is needed to support
		     * this option.
		     */
		    break;
		default:
		    /* special case of relative stops separated by spaces? */
		    if (option == argv[n] + 1) {
			tab_list = add_to_tab_list(&append, argv[n]);
		    }
		    break;
		}
	    }
	    break;
	default:
	    if (append != 0) {
		if (tab_list != (const char *) append) {
		    /* one of the predefined options was used */
		    free(append);
		    append = 0;
		}
	    }
	    tab_list = add_to_tab_list(&append, option);
	    break;
	}
    }

    setupterm(term_name, fd, (int *) 0);

    max_cols = (columns > 0) ? columns : 80;

    if (!VALID_STRING(clear_all_tabs)) {
	fprintf(stderr,
		"%s: terminal type '%s' cannot reset tabs\n",
		_nc_progname, term_name);
    } else if (!VALID_STRING(set_tab)) {
	fprintf(stderr,
		"%s: terminal type '%s' cannot set tabs\n",
		_nc_progname, term_name);
    } else if (legal_tab_list(tab_list)) {
	int *list = decode_tabs(tab_list);

	if (!no_op)
	    tputs(clear_all_tabs, 1, putch);

	if (list != 0) {
	    if (!no_op)
		do_tabs(list);
	    if (debug) {
		fflush(stderr);
		printf("tabs %s\n", tab_list);
		print_ruler(list);
		write_tabs(list);
	    }
	    free(list);
	} else if (debug) {
	    fflush(stderr);
	    printf("tabs %s\n", tab_list);
	}
	rc = EXIT_SUCCESS;
    }
    if (append != 0)
	free(append);
    ExitProgram(rc);
}