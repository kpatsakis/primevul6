_WriteSelectionData(XtermWidget xw, Char *line, size_t length)
{
    /* Write data to pty a line at a time. */
    /* Doing this one line at a time may no longer be necessary
       because v_write has been re-written. */

#if OPT_PASTE64
    TScreen *screen = TScreenOf(xw);
#endif
    Char *lag, *end;

    /* in the VMS version, if tt_pasting isn't set to True then qio
       reads aren't blocked and an infinite loop is entered, where the
       pasted text shows up as new input, goes in again, shows up
       again, ad nauseum. */
#ifdef VMS
    tt_pasting = True;
#endif

    end = &line[length];
    lag = line;

#if OPT_PASTE64
    if (screen->base64_paste) {
	_qWriteSelectionData(xw, lag, (unsigned) (end - lag));
	base64_flush(screen);
    } else
#endif
    {
	if (!SCREEN_FLAG(screen, paste_literal_nl)) {
	    Char *cp;
	    for (cp = line; cp != end; cp++) {
		if (*cp == '\n') {
		    *cp = '\r';
		    _qWriteSelectionData(xw, lag, (unsigned) (cp - lag + 1));
		    lag = cp + 1;
		}
	    }
	}

	if (lag != end) {
	    _qWriteSelectionData(xw, lag, (unsigned) (end - lag));
	}
    }
#ifdef VMS
    tt_pasting = False;
    tt_start_read();		/* reenable reads or a character may be lost */
#endif
}