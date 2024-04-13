_nc_mouse_event(SCREEN *sp)
{
    MEVENT *eventp = sp->_mouse_eventp;
    bool result = FALSE;

    (void) eventp;

    switch (sp->_mouse_type) {
    case M_XTERM:
	/* xterm: never have to query, mouse events are in the keyboard stream */
#if USE_EMX_MOUSE
	{
	    char kbuf[3];

	    int i, res = read(M_FD(sp), &kbuf, 3);	/* Eat the prefix */
	    if (res != 3)
		printf("Got %d chars instead of 3 for prefix.\n", res);
	    for (i = 0; i < res; i++) {
		if (kbuf[i] != key_mouse[i])
		    printf("Got char %d instead of %d for prefix.\n",
			   (int) kbuf[i], (int) key_mouse[i]);
	    }
	    result = TRUE;
	}
#endif /* USE_EMX_MOUSE */
	break;

#if USE_GPM_SUPPORT
    case M_GPM:
	if (sp->_mouse_fd >= 0) {
	    /* query server for event, return TRUE if we find one */
	    Gpm_Event ev;

	    switch (my_Gpm_GetEvent(&ev)) {
	    case 0:
		/* Connection closed, drop the mouse. */
		sp->_mouse_fd = -1;
		break;
	    case 1:
		/* there's only one mouse... */
		eventp->id = NORMAL_EVENT;

		eventp->bstate = 0;
		switch (ev.type & 0x0f) {
		case (GPM_DOWN):
		    if (ev.buttons & GPM_B_LEFT)
			eventp->bstate |= BUTTON1_PRESSED;
		    if (ev.buttons & GPM_B_MIDDLE)
			eventp->bstate |= BUTTON2_PRESSED;
		    if (ev.buttons & GPM_B_RIGHT)
			eventp->bstate |= BUTTON3_PRESSED;
		    break;
		case (GPM_UP):
		    if (ev.buttons & GPM_B_LEFT)
			eventp->bstate |= BUTTON1_RELEASED;
		    if (ev.buttons & GPM_B_MIDDLE)
			eventp->bstate |= BUTTON2_RELEASED;
		    if (ev.buttons & GPM_B_RIGHT)
			eventp->bstate |= BUTTON3_RELEASED;
		    break;
		default:
		    eventp->bstate |= REPORT_MOUSE_POSITION;
		    break;
		}

		eventp->x = ev.x - 1;
		eventp->y = ev.y - 1;
		eventp->z = 0;

		/* bump the next-free pointer into the circular list */
		sp->_mouse_eventp = NEXT(eventp);
		result = TRUE;
		break;
	    }
	}
	break;
#endif

#if USE_SYSMOUSE
    case M_SYSMOUSE:
	if (sp->_sysmouse_head < sp->_sysmouse_tail) {
	    *eventp = sp->_sysmouse_fifo[sp->_sysmouse_head];

	    /*
	     * Point the fifo-head to the next possible location.  If there
	     * are none, reset the indices.  This may be interrupted by the
	     * signal handler, doing essentially the same reset.
	     */
	    sp->_sysmouse_head += 1;
	    if (sp->_sysmouse_head == sp->_sysmouse_tail) {
		sp->_sysmouse_tail = 0;
		sp->_sysmouse_head = 0;
	    }

	    /* bump the next-free pointer into the circular list */
	    sp->_mouse_eventp = eventp = NEXT(eventp);
	    result = TRUE;
	}
	break;
#endif /* USE_SYSMOUSE */

#ifdef USE_TERM_DRIVER
    case M_TERM_DRIVER:
	while (sp->_drv_mouse_head < sp->_drv_mouse_tail) {
	    *eventp = sp->_drv_mouse_fifo[sp->_drv_mouse_head];

	    /*
	     * Point the fifo-head to the next possible location.  If there
	     * are none, reset the indices.
	     */
	    sp->_drv_mouse_head += 1;
	    if (sp->_drv_mouse_head == sp->_drv_mouse_tail) {
		sp->_drv_mouse_tail = 0;
		sp->_drv_mouse_head = 0;
	    }

	    /* bump the next-free pointer into the circular list */
	    sp->_mouse_eventp = eventp = NEXT(eventp);
	    result = TRUE;
	}
	break;
#endif

    case M_NONE:
	break;
    }

    return result;		/* true if we found an event */
}