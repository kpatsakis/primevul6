main(int argc, char *argv[])
{
    char my_tmpname[PATH_MAX];
    char my_altfile[PATH_MAX];
    int v_opt = -1;
    int smart_defaults = TRUE;
    char *termcap;
    ENTRY *qp;

    int this_opt, last_opt = '?';

    int outform = F_TERMINFO;	/* output format */
    int sortmode = S_TERMINFO;	/* sort_mode */

    int width = 60;
    int height = 65535;
    bool formatted = FALSE;	/* reformat complex strings? */
    bool literal = FALSE;	/* suppress post-processing? */
    int numbers = 0;		/* format "%'char'" to/from "%{number}" */
    bool forceresolve = FALSE;	/* force resolution */
    bool limited = TRUE;
    char *tversion = (char *) NULL;
    const char *source_file = "terminfo";
    char *outdir = (char *) NULL;
    bool check_only = FALSE;
    bool suppress_untranslatable = FALSE;
    int quickdump = 0;
    bool quiet = FALSE;
    bool wrap_strings = FALSE;

    log_fp = stderr;

    _nc_progname = _nc_rootname(argv[0]);
    atexit(cleanup);

    if ((infodump = same_program(_nc_progname, PROG_CAPTOINFO)) != FALSE) {
	outform = F_TERMINFO;
	sortmode = S_TERMINFO;
    }
    if ((capdump = same_program(_nc_progname, PROG_INFOTOCAP)) != FALSE) {
	outform = F_TERMCAP;
	sortmode = S_TERMCAP;
    }
#if NCURSES_XNAMES
    use_extended_names(FALSE);
#endif
    _nc_strict_bsd = 0;

    /*
     * Processing arguments is a little complicated, since someone made a
     * design decision to allow the numeric values for -w, -v options to
     * be optional.
     */
    while ((this_opt = getopt(argc, argv,
			      "0123456789CDIKLNQR:TUVWace:fGgo:qrstvwx")) != -1) {
	if (isdigit(this_opt)) {
	    switch (last_opt) {
	    case 'Q':
		add_digit(&quickdump, this_opt);
		break;
	    case 'v':
		add_digit(&v_opt, this_opt);
		break;
	    case 'w':
		add_digit(&width, this_opt);
		break;
	    default:
		switch (this_opt) {
		case '0':
		    last_opt = this_opt;
		    width = 65535;
		    height = 1;
		    break;
		case '1':
		    last_opt = this_opt;
		    width = 0;
		    break;
		default:
		    usage();
		}
	    }
	    continue;
	}
	switch (this_opt) {
	case 'K':
	    _nc_strict_bsd = 1;
	    /* the initial version of -K in 20110730 fell-thru here, but the
	     * same flag is useful when reading sources -TD
	     */
	    break;
	case 'C':
	    capdump = TRUE;
	    outform = F_TERMCAP;
	    sortmode = S_TERMCAP;
	    break;
	case 'D':
	    debug_level = VtoTrace(v_opt);
	    set_trace_level(debug_level);
	    show_databases(outdir);
	    ExitProgram(EXIT_SUCCESS);
	    break;
	case 'I':
	    infodump = TRUE;
	    outform = F_TERMINFO;
	    sortmode = S_TERMINFO;
	    break;
	case 'L':
	    infodump = TRUE;
	    outform = F_VARIABLE;
	    sortmode = S_VARIABLE;
	    break;
	case 'N':
	    smart_defaults = FALSE;
	    literal = TRUE;
	    break;
	case 'Q':
	    quickdump = 0;
	    break;
	case 'R':
	    tversion = optarg;
	    break;
	case 'T':
	    limited = FALSE;
	    break;
	case 'U':
	    literal = TRUE;
	    break;
	case 'V':
	    puts(curses_version());
	    ExitProgram(EXIT_SUCCESS);
	case 'W':
	    wrap_strings = TRUE;
	    break;
	case 'c':
	    check_only = TRUE;
	    break;
	case 'e':
	    namelst = make_namelist(optarg);
	    break;
	case 'f':
	    formatted = TRUE;
	    break;
	case 'G':
	    numbers = 1;
	    break;
	case 'g':
	    numbers = -1;
	    break;
	case 'o':
	    outdir = optarg;
	    break;
	case 'q':
	    quiet = TRUE;
	    break;
	case 'r':
	    forceresolve = TRUE;
	    break;
	case 's':
	    showsummary = TRUE;
	    break;
	case 'v':
	    v_opt = 0;
	    break;
	case 'w':
	    width = 0;
	    break;
#if NCURSES_XNAMES
	case 't':
	    _nc_disable_period = FALSE;
	    suppress_untranslatable = TRUE;
	    break;
	case 'a':
	    _nc_disable_period = TRUE;
	    /* FALLTHRU */
	case 'x':
	    use_extended_names(TRUE);
	    using_extensions = TRUE;
	    break;
#endif
	default:
	    usage();
	}
	last_opt = this_opt;
    }

    debug_level = VtoTrace(v_opt);
    set_trace_level(debug_level);

    if (_nc_tracing) {
	save_check_termtype = _nc_check_termtype2;
	_nc_check_termtype2 = check_termtype;
    }
#if !HAVE_BIG_CORE
    /*
     * Aaargh! immedhook seriously hoses us!
     *
     * One problem with immedhook is it means we can't do -e.  Problem
     * is that we can't guarantee that for each terminal listed, all the
     * terminals it depends on will have been kept in core for reference
     * resolution -- in fact it's certain the primitive types at the end
     * of reference chains *won't* be in core unless they were explicitly
     * in the select list themselves.
     */
    if (namelst && (!infodump && !capdump)) {
	(void) fprintf(stderr,
		       "%s: Sorry, -e can't be used without -I or -C\n",
		       _nc_progname);
	ExitProgram(EXIT_FAILURE);
    }
#endif /* HAVE_BIG_CORE */

    if (optind < argc) {
	source_file = argv[optind++];
	if (optind < argc) {
	    fprintf(stderr,
		    "%s: Too many file names.  Usage:\n\t%s %s",
		    _nc_progname,
		    _nc_progname,
		    usage_string);
	    ExitProgram(EXIT_FAILURE);
	}
    } else {
	if (infodump == TRUE) {
	    /* captoinfo's no-argument case */
	    source_file = "/etc/termcap";
	    if ((termcap = getenv("TERMCAP")) != 0
		&& (namelst = make_namelist(getenv("TERM"))) != 0) {
		if (access(termcap, F_OK) == 0) {
		    /* file exists */
		    source_file = termcap;
		} else {
		    if ((tmp_fp = open_tempfile(my_tmpname)) != 0) {
			source_file = my_tmpname;
			fprintf(tmp_fp, "%s\n", termcap);
			fclose(tmp_fp);
			tmp_fp = open_input(source_file, (char *) 0);
			to_remove = source_file;
		    } else {
			failed("tmpnam");
		    }
		}
	    }
	} else {
	    /* tic */
	    fprintf(stderr,
		    "%s: File name needed.  Usage:\n\t%s %s",
		    _nc_progname,
		    _nc_progname,
		    usage_string);
	    ExitProgram(EXIT_FAILURE);
	}
    }

    if (tmp_fp == 0) {
	tmp_fp = open_input(source_file, my_altfile);
	if (!strcmp(source_file, "-")) {
	    source_file = STDIN_NAME;
	}
    }

    if (infodump || check_only) {
	dump_init(tversion,
		  (smart_defaults
		   ? outform
		   : F_LITERAL),
		  sortmode,
		  wrap_strings, width, height,
		  debug_level, formatted || check_only, check_only, quickdump);
    } else if (capdump) {
	dump_init(tversion,
		  outform,
		  sortmode,
		  wrap_strings, width, height,
		  debug_level, FALSE, FALSE, FALSE);
    }

    /* parse entries out of the source file */
    _nc_set_source(source_file);
#if !HAVE_BIG_CORE
    if (!(check_only || infodump || capdump))
	_nc_set_writedir(outdir);
#endif /* HAVE_BIG_CORE */
    _nc_read_entry_source(tmp_fp, (char *) NULL,
			  !smart_defaults || literal, FALSE,
			  ((check_only || infodump || capdump)
			   ? NULLHOOK
			   : immedhook));

    /* do use resolution */
    if (check_only || (!infodump && !capdump) || forceresolve) {
	if (!_nc_resolve_uses2(TRUE, literal) && !check_only) {
	    ExitProgram(EXIT_FAILURE);
	}
    }

    /* length check */
    if (check_only && limited && (capdump || infodump)) {
	for_entry_list(qp) {
	    if (matches(namelst, qp->tterm.term_names)) {
		int len = fmt_entry(&qp->tterm, NULL, FALSE, TRUE, infodump, numbers);

		if (len > (infodump ? MAX_TERMINFO_LENGTH : MAX_TERMCAP_LENGTH))
		    (void) fprintf(stderr,
				   "%s: resolved %s entry is %d bytes long\n",
				   _nc_progname,
				   _nc_first_name(qp->tterm.term_names),
				   len);
	    }
	}
    }

    /* write or dump all entries */
    if (check_only) {
	/* this is in case infotocap() generates warnings */
	_nc_curr_col = _nc_curr_line = -1;

	for_entry_list(qp) {
	    if (matches(namelst, qp->tterm.term_names)) {
		/* this is in case infotocap() generates warnings */
		_nc_set_type(_nc_first_name(qp->tterm.term_names));
		_nc_curr_line = (int) qp->startline;
		repair_acsc(&qp->tterm);
		dump_entry(&qp->tterm, suppress_untranslatable,
			   limited, numbers, NULL);
	    }
	}
    } else {
	if (!infodump && !capdump) {
	    _nc_set_writedir(outdir);
	    for_entry_list(qp) {
		if (matches(namelst, qp->tterm.term_names))
		    write_it(qp);
	    }
	} else {
	    /* this is in case infotocap() generates warnings */
	    _nc_curr_col = _nc_curr_line = -1;

	    for_entry_list(qp) {
		if (matches(namelst, qp->tterm.term_names)) {
		    long j = qp->cend - qp->cstart;
		    int len = 0;

		    /* this is in case infotocap() generates warnings */
		    _nc_set_type(_nc_first_name(qp->tterm.term_names));

		    if (!quiet) {
			(void) fseek(tmp_fp, qp->cstart, SEEK_SET);
			while (j-- > 0) {
			    int ch = fgetc(tmp_fp);
			    if (ch == EOF || ferror(tmp_fp)) {
				break;
			    } else if (infodump) {
				(void) putchar(ch);
			    } else {
				put_translate(ch);
			    }
			}
		    }

		    repair_acsc(&qp->tterm);
		    dump_entry(&qp->tterm, suppress_untranslatable,
			       limited, numbers, NULL);
		    for (j = 0; j < (long) qp->nuses; j++)
			dump_uses(qp->uses[j].name, !capdump);
		    len = show_entry();
		    if (debug_level != 0 && !limited)
			printf("# length=%d\n", len);
		}
	    }
	    if (!namelst && _nc_tail && !quiet) {
		int c, oldc = '\0';
		bool in_comment = FALSE;
		bool trailing_comment = FALSE;

		(void) fseek(tmp_fp, _nc_tail->cend, SEEK_SET);
		while ((c = fgetc(tmp_fp)) != EOF) {
		    if (oldc == '\n') {
			if (c == '#') {
			    trailing_comment = TRUE;
			    in_comment = TRUE;
			} else {
			    in_comment = FALSE;
			}
		    }
		    if (trailing_comment
			&& (in_comment || (oldc == '\n' && c == '\n')))
			putchar(c);
		    oldc = c;
		}
	    }
	}
    }

    /* Show the directory into which entries were written, and the total
     * number of entries
     */
    if (showsummary
	&& (!(check_only || infodump || capdump))) {
	int total = _nc_tic_written();
	if (total != 0)
	    fprintf(log_fp, "%d entries written to %s\n",
		    total,
		    _nc_tic_dir((char *) 0));
	else
	    fprintf(log_fp, "No entries written\n");
    }
    ExitProgram(EXIT_SUCCESS);
}