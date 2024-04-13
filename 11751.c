drv_CanHandle(TERMINAL_CONTROL_BLOCK * TCB, const char *tname, int *errret)
{
    bool result = FALSE;
    int status;
    TERMINAL *termp;
    SCREEN *sp;

    START_TRACE();
    T((T_CALLED("tinfo::drv_CanHandle(%p)"), (void *) TCB));

    assert(TCB != 0 && tname != 0);
    termp = (TERMINAL *) TCB;
    sp = TCB->csp;
    TCB->magic = TCBMAGIC;

#if (NCURSES_USE_DATABASE || NCURSES_USE_TERMCAP)
    status = _nc_setup_tinfo(tname, &TerminalType(termp));
    T(("_nc_setup_tinfo returns %d", status));
#else
    T(("no database available"));
    status = TGETENT_NO;
#endif

    /* try fallback list if entry on disk */
    if (status != TGETENT_YES) {
	const TERMTYPE2 *fallback = _nc_fallback2(tname);

	if (fallback) {
	    T(("found fallback entry"));
	    TerminalType(termp) = *fallback;
	    status = TGETENT_YES;
	}
    }

    if (status != TGETENT_YES) {
	NCURSES_SP_NAME(del_curterm) (NCURSES_SP_ARGx termp);
	if (status == TGETENT_ERR) {
	    ret_error0(status, "terminals database is inaccessible\n");
	} else if (status == TGETENT_NO) {
	    ret_error1(status, "unknown terminal type.\n",
		       tname, NO_COPY);
	} else {
	    ret_error0(status, "unexpected return-code\n");
	}
    }
    result = TRUE;
#if NCURSES_EXT_NUMBERS
    _nc_export_termtype2(&termp->type, &TerminalType(termp));
#endif
#if !USE_REENTRANT
    save_ttytype(termp);
#endif

    if (command_character)
	_nc_tinfo_cmdch(termp, *command_character);

    /*
     * If an application calls setupterm() rather than initscr() or
     * newterm(), we will not have the def_prog_mode() call in
     * _nc_setupscreen().  Do it now anyway, so we can initialize the
     * baudrate.
     */
    if (sp == 0 && NC_ISATTY(termp->Filedes)) {
	get_baudrate(termp);
    }
#if NCURSES_EXT_NUMBERS
#define cleanup_termtype() \
    _nc_free_termtype2(&TerminalType(termp)); \
    _nc_free_termtype(&termp->type)
#else
#define cleanup_termtype() \
    _nc_free_termtype2(&TerminalType(termp))
#endif

    if (generic_type) {
	/*
	 * BSD 4.3's termcap contains mis-typed "gn" for wy99.  Do a sanity
	 * check before giving up.
	 */
	if ((VALID_STRING(cursor_address)
	     || (VALID_STRING(cursor_down) && VALID_STRING(cursor_home)))
	    && VALID_STRING(clear_screen)) {
	    cleanup_termtype();
	    ret_error1(TGETENT_YES, "terminal is not really generic.\n",
		       tname, NO_COPY);
	} else {
	    cleanup_termtype();
	    ret_error1(TGETENT_NO, "I need something more specific.\n",
		       tname, NO_COPY);
	}
    }
    if (hard_copy) {
	cleanup_termtype();
	ret_error1(TGETENT_YES, "I can't handle hardcopy terminals.\n",
		   tname, NO_COPY);
    }

    returnBool(result);
}