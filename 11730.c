main(int argc, char **argv)
{
    char *term;
    int errret;
    bool cmdline = TRUE;
    int c;
    char buf[BUFSIZ];
    int result = 0;
    int fd;
    TTY tty_settings;
    bool opt_x = FALSE;		/* clear scrollback if possible */
    bool is_alias;
    bool need_tty;

    prg_name = check_aliases(_nc_rootname(argv[0]), TRUE);

    term = getenv("TERM");

    while ((c = getopt(argc, argv, "ST:Vx")) != -1) {
	switch (c) {
	case 'S':
	    cmdline = FALSE;
	    break;
	case 'T':
	    use_env(FALSE);
	    use_tioctl(TRUE);
	    term = optarg;
	    break;
	case 'V':
	    puts(curses_version());
	    ExitProgram(EXIT_SUCCESS);
	case 'x':		/* do not try to clear scrollback */
	    opt_x = TRUE;
	    break;
	default:
	    usage();
	    /* NOTREACHED */
	}
    }

    is_alias = (is_clear || is_reset || is_init);
    need_tty = ((is_reset || is_init) ||
		(optind < argc &&
		 (!strcmp(argv[optind], "reset") ||
		  !strcmp(argv[optind], "init"))));

    /*
     * Modify the argument list to omit the options we processed.
     */
    if (is_alias) {
	if (optind-- < argc) {
	    argc -= optind;
	    argv += optind;
	}
	argv[0] = prg_name;
    } else {
	argc -= optind;
	argv += optind;
    }

    if (term == 0 || *term == '\0')
	quit(ErrUsage, "No value for $TERM and no -T specified");

    fd = save_tty_settings(&tty_settings, need_tty);

    if (setupterm(term, fd, &errret) != OK && errret <= 0)
	quit(ErrTermType, "unknown terminal \"%s\"", term);

    if (cmdline) {
	if ((argc <= 0) && !is_alias)
	    usage();
	ExitProgram(tput_cmd(fd, &tty_settings, opt_x, argc, argv));
    }

    while (fgets(buf, sizeof(buf), stdin) != 0) {
	char *argvec[16];	/* command, 9 parms, null, & slop */
	int argnum = 0;
	char *cp;

	/* crack the argument list into a dope vector */
	for (cp = buf; *cp; cp++) {
	    if (isspace(UChar(*cp))) {
		*cp = '\0';
	    } else if (cp == buf || cp[-1] == 0) {
		argvec[argnum++] = cp;
		if (argnum >= (int) SIZEOF(argvec) - 1)
		    break;
	    }
	}
	argvec[argnum] = 0;

	if (argnum != 0
	    && tput_cmd(fd, &tty_settings, opt_x, argnum, argvec) != 0) {
	    if (result == 0)
		result = ErrSystem(0);	/* will return value >4 */
	    ++result;
	}
    }

    ExitProgram(result);
}