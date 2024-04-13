drv_size(TERMINAL_CONTROL_BLOCK * TCB, int *linep, int *colp)
{
    SCREEN *sp;
    bool useEnv = TRUE;
    bool useTioctl = TRUE;

    AssertTCB();
    sp = TCB->csp;		/* can be null here */

    if (sp) {
	useEnv = sp->_use_env;
	useTioctl = sp->use_tioctl;
    } else {
	useEnv = _nc_prescreen.use_env;
	useTioctl = _nc_prescreen.use_tioctl;
    }

    /* figure out the size of the screen */
    T(("screen size: terminfo lines = %d columns = %d", lines, columns));

    *linep = (int) lines;
    *colp = (int) columns;

    if (useEnv || useTioctl) {
	int value;

#ifdef __EMX__
	{
	    int screendata[2];
	    _scrsize(screendata);
	    *colp = screendata[0];
	    *linep = ((sp != 0 && sp->_filtered)
		      ? 1
		      : screendata[1]);
	    T(("EMX screen size: environment LINES = %d COLUMNS = %d",
	       *linep, *colp));
	}
#endif
#if HAVE_SIZECHANGE
	/* try asking the OS */
	{
	    TERMINAL *termp = (TERMINAL *) TCB;
	    if (NC_ISATTY(termp->Filedes)) {
		STRUCT_WINSIZE size;

		errno = 0;
		do {
		    if (ioctl(termp->Filedes, IOCTL_WINSIZE, &size) >= 0) {
			*linep = ((sp != 0 && sp->_filtered)
				  ? 1
				  : WINSIZE_ROWS(size));
			*colp = WINSIZE_COLS(size);
			T(("SYS screen size: environment LINES = %d COLUMNS = %d",
			   *linep, *colp));
			break;
		    }
		} while
		    (errno == EINTR);
	    }
	}
#endif /* HAVE_SIZECHANGE */

	if (useEnv) {
	    if (useTioctl) {
		/*
		 * If environment variables are used, update them.
		 */
		if ((sp == 0 || !sp->_filtered) && _nc_getenv_num("LINES") > 0) {
		    _nc_setenv_num("LINES", *linep);
		}
		if (_nc_getenv_num("COLUMNS") > 0) {
		    _nc_setenv_num("COLUMNS", *colp);
		}
	    }

	    /*
	     * Finally, look for environment variables.
	     *
	     * Solaris lets users override either dimension with an environment
	     * variable.
	     */
	    if ((value = _nc_getenv_num("LINES")) > 0) {
		*linep = value;
		T(("screen size: environment LINES = %d", *linep));
	    }
	    if ((value = _nc_getenv_num("COLUMNS")) > 0) {
		*colp = value;
		T(("screen size: environment COLUMNS = %d", *colp));
	    }
	}

	/* if we can't get dynamic info about the size, use static */
	if (*linep <= 0) {
	    *linep = (int) lines;
	}
	if (*colp <= 0) {
	    *colp = (int) columns;
	}

	/* the ultimate fallback, assume fixed 24x80 size */
	if (*linep <= 0) {
	    *linep = 24;
	}
	if (*colp <= 0) {
	    *colp = 80;
	}

	/*
	 * Put the derived values back in the screen-size caps, so
	 * tigetnum() and tgetnum() will do the right thing.
	 */
	lines = (short) (*linep);
	columns = (short) (*colp);
    }

    T(("screen size is %dx%d", *linep, *colp));
    return OK;
}