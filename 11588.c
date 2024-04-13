_nc_mouse_inline(SCREEN *sp)
/* mouse report received in the keyboard stream -- parse its info */
{
    bool result = FALSE;
    MEVENT *eventp = sp->_mouse_eventp;

    TR(MY_TRACE, ("_nc_mouse_inline() called"));

    if (sp->_mouse_type == M_XTERM) {
	switch (sp->_mouse_format) {
	case MF_X10:
	    result = decode_xterm_X10(sp, eventp);
	    break;
	case MF_SGR1006:
	    result = decode_xterm_SGR1006(sp, eventp);
	    break;
#ifdef EXP_XTERM_1005
	case MF_XTERM_1005:
	    result = decode_xterm_1005(sp, eventp);
	    break;
#endif
	}

	TR(MY_TRACE,
	   ("_nc_mouse_inline: primitive mouse-event %s has slot %ld",
	    _nc_tracemouse(sp, eventp),
	    (long) IndexEV(sp, eventp)));

	/* bump the next-free pointer into the circular list */
	sp->_mouse_eventp = NEXT(eventp);

	if (!result) {
	    /* If this event is from a wheel-mouse, treat it like position
	     * reports and avoid waiting for the release-events which will
	     * never come.
	     */
	    if (eventp->bstate & BUTTON_PRESSED) {
		int b;

		for (b = 4; b <= MAX_BUTTONS; ++b) {
		    if ((eventp->bstate & MASK_PRESS(b))) {
			result = TRUE;
			break;
		    }
		}
	    }
	}
    }

    return (result);
}