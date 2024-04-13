mouse_server(unsigned long param)
#endif
{
    SCREEN *sp = (SCREEN *) param;
    unsigned short fWait = MOU_WAIT;
    /* NOPTRRECT mourt = { 0,0,24,79 }; */
    MOUEVENTINFO mouev;
    HMOU hmou;
    unsigned short mask = MOUSE_BN1_DOWN | MOUSE_BN2_DOWN | MOUSE_BN3_DOWN;
    int nbuttons = 3;
    int oldstate = 0;
    char err[80];
    unsigned long rc;

    /* open the handle for the mouse */
    if (MouOpen(NULL, &hmou) == 0) {
	rc = MouSetEventMask(&mask, hmou);
	if (rc) {		/* retry with 2 buttons */
	    mask = MOUSE_BN1_DOWN | MOUSE_BN2_DOWN;
	    rc = MouSetEventMask(&mask, hmou);
	    nbuttons = 2;
	}
	if (rc == 0 && MouDrawPtr(hmou) == 0) {
	    for (;;) {
		/* sit and wait on the event queue */
		rc = MouReadEventQue(&mouev, &fWait, hmou);
		if (rc) {
		    _nc_SPRINTF(err, _nc_SLIMIT(sizeof(err))
				"Error reading mouse queue, rc=%lu.\r\n", rc);
		    break;
		}
		if (!sp->_emxmouse_activated)
		    goto finish;

		/*
		 * OS/2 numbers a 3-button mouse inconsistently from other
		 * platforms:
		 *      1 = left
		 *      2 = right
		 *      3 = middle.
		 */
		if ((mouev.fs ^ oldstate) & MOUSE_BN1_DOWN)
		    write_event(sp, mouev.fs & MOUSE_BN1_DOWN,
				sp->_emxmouse_buttons[1], mouev.col, mouev.row);
		if ((mouev.fs ^ oldstate) & MOUSE_BN2_DOWN)
		    write_event(sp, mouev.fs & MOUSE_BN2_DOWN,
				sp->_emxmouse_buttons[3], mouev.col, mouev.row);
		if ((mouev.fs ^ oldstate) & MOUSE_BN3_DOWN)
		    write_event(sp, mouev.fs & MOUSE_BN3_DOWN,
				sp->_emxmouse_buttons[2], mouev.col, mouev.row);

	      finish:
		oldstate = mouev.fs;
	    }
	} else {
	    _nc_SPRINTF(err, _nc_SLIMIT(sizeof(err))
			"Error setting event mask, buttons=%d, rc=%lu.\r\n",
			nbuttons, rc);
	}

	DosWrite(2, err, strlen(err), &rc);
	MouClose(hmou);
    }
    DosExit(EXIT_THREAD, 0L);
}