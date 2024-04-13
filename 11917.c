main(int argc, char *argv[])
{
    int n;
    int r_run, t_run, n_run;
    char *old_term = getenv("TERM");
    int r_opt = 1;
    char *t_opt = 0;
    int len_names = 0;		/* cur # of items in all_names[] */
    int use_names = 10;		/* max # of items in all_names[] */
    char **all_names = typeCalloc(char *, use_names);
    int all_parms[10];		/* workspace for "-a" option */
    int len_terms = 0;		/* cur # of items in all_terms[] */
    int use_terms = 10;		/* max # of items in all_terms[] */
    char **all_terms = typeCalloc(char *, use_terms);
    int len_parms = 0;		/* cur # of items in num_parms[], str_parms[] */
    int use_parms = argc + 10;	/* max # of items in num_parms[], str_parms[] */
    int *num_parms = typeCalloc(int, use_parms);
    char **str_parms = typeCalloc(char *, use_parms);

    if (all_names == 0 || all_terms == 0 || num_parms == 0 || str_parms == 0)
	failed("no memory");

    while ((n = getopt(argc, argv, "T:ar:v")) != -1) {
	switch (n) {
	case 'T':
	    t_opt = optarg;
	    break;
	case 'a':
	    ++a_opt;
	    break;
	case 'r':
	    r_opt = atoi(optarg);
	    break;
	case 'v':
	    ++v_opt;
	    break;
	default:
	    usage();
	    break;
	}
    }

    /*
     * If there is a nonnumeric parameter after the options, use that as the
     * capability name.
     */
    if (optind < argc) {
	if (!isNumeric(argv[optind])) {
	    all_names[len_names++] = strdup(argv[optind++]);
	}
    }

    /*
     * Any remaining arguments must be possible parameter values.  If numeric,
     * and "-a" is not set, use those as the maximum values within which the
     * test parameters should vary.
     */
    while (optind < argc) {
	if (isNumeric(argv[optind])) {
	    char *dummy = 0;
	    long value = strtol(argv[optind], &dummy, 0);
	    num_parms[len_parms] = (int) value;
	}
	str_parms[len_parms] = argv[optind];
	++optind;
	++len_parms;
    }
    for (n = len_parms; n < use_parms; ++n) {
	static char dummy[1];
	str_parms[n] = dummy;
    }
    if (v_opt) {
	printf("%d parameter%s%s\n", PLURAL(len_parms), COLONS(len_parms));
	for (n = 0; n < len_parms; ++n) {
	    printf(" %d: %d (%s)\n", n + 1, num_parms[n], str_parms[n]);
	}
    }

    /*
     * Make a list of values for $TERM.  Accept "-" for standard input to
     * simplify scripting a check of the whole database.
     */
    old_term = strdup((old_term == 0) ? "unknown" : old_term);
    if (t_opt != 0) {
	if (!strcmp(t_opt, "-")) {
	    char buffer[BUFSIZ];
	    while (fgets(buffer, sizeof(buffer) - 1, stdin) != 0) {
		char *s = buffer;
		char *t;
		while (isspace(UChar(s[0])))
		    ++s;
		t = s + strlen(s);
		while (t != s && isspace(UChar(t[-1])))
		    *--t = '\0';
		s = strdup(s);
		if (len_terms + 2 >= use_terms) {
		    use_terms *= 2;
		    all_terms = typeRealloc(char *, use_terms, all_terms);
		    if (all_terms == 0)
			failed("no memory: all_terms");
		}
		all_terms[len_terms++] = s;
	    }
	} else {
	    char *s = t_opt;
	    char *t;
	    while ((t = strtok(s, ",")) != 0) {
		s = 0;
		if (len_terms + 2 >= use_terms) {
		    use_terms *= 2;
		    all_terms = typeRealloc(char *, use_terms, all_terms);
		    if (all_terms == 0)
			failed("no memory: all_terms");
		}
		all_terms[len_terms++] = strdup(t);
	    }
	}
    } else {
	all_terms[len_terms++] = strdup(old_term);
    }
    all_terms[len_terms] = 0;
    if (v_opt) {
	printf("%d term%s:\n", PLURAL(len_terms));
	for (n = 0; n < len_terms; ++n) {
	    printf(" %d: %s\n", n + 1, all_terms[n]);
	}
    }

    /*
     * If no capability name was selected, use the predefined list of string
     * capabilities.
     *
     * TODO: To address the "other" systems which do not follow SVr4,
     * just use the output from infocmp on $TERM.
     */
    if (len_names == 0) {
#if defined(HAVE_CURSES_DATA_BOOLNAMES) || defined(DECL_CURSES_DATA_BOOLNAMES)
	for (n = 0; strnames[n] != 0; ++n) {
	    if (len_names + 2 >= use_names) {
		use_names *= 2;
		all_names = typeRealloc(char *, use_names, all_names);
		if (all_names == 0) {
		    failed("no memory: all_names");
		}
	    }
	    all_names[len_names++] = strdup(strnames[n]);
	}
#else
	all_names[len_names++] = strdup("cup");
	all_names[len_names++] = strdup("sgr");
#endif
    }
    all_names[len_names] = 0;
    if (v_opt) {
	printf("%d name%s%s\n", PLURAL(len_names), COLONS(len_names));
	for (n = 0; n < len_names; ++n) {
	    printf(" %d: %s\n", n + 1, all_names[n]);
	}
    }

    if (r_opt <= 0)
	r_opt = 1;

    for (r_run = 0; r_run < r_opt; ++r_run) {
	for (t_run = 0; t_run < len_terms; ++t_run) {
	    int errs;

	    if (setupterm(all_terms[t_run], fileno(stdout), &errs) != OK) {
		printf("** skipping %s (errs:%d)\n", all_terms[t_run], errs);
	    }

	    if (v_opt)
		printf("** testing %s\n", all_terms[t_run]);
	    if (len_names == 1) {
		if (a_opt) {
		    /* for each combination of values */
		    memset(all_parms, 0, sizeof(all_parms));
		    do {
			test_tparm(all_names[0], all_parms);
		    }
		    while (increment(all_parms, num_parms, len_parms, 0));
		} else {
		    /* for the given values */
		    test_tparm(all_names[0], num_parms);
		}
	    } else {
		for (n_run = 0; n_run < len_names; ++n_run) {
		    test_tparm(all_names[n_run], num_parms);
		}
	    }
	    if (cur_term != 0) {
		del_curterm(cur_term);
	    } else {
		printf("? no cur_term\n");
	    }
	}
    }
#if NO_LEAKS
    for (n = 0; n < len_names; ++n) {
	free(all_names[n]);
    }
    free(all_names);
    free(old_term);
    for (n = 0; n < len_terms; ++n) {
	free(all_terms[n]);
    }
    free(all_terms);
    free(num_parms);
    free(str_parms);
#endif

    ExitProgram(EXIT_SUCCESS);
}