CheckLocatorPosition(XtermWidget xw, XButtonEvent *event)
{
    ANSI reply;
    TScreen *screen = TScreenOf(xw);
    int row, col;
    Bool oor;

    LocatorCoords(row, col, event->x, event->y, oor);

    /*
     * Send report if the pointer left the filter rectangle, if
     * the pointer left the window, or if the filter rectangle
     * had no coordinates and the pointer re-entered the window.
     */
    if (oor || (screen->loc_filter_top == LOC_FILTER_POS) ||
	(col < screen->loc_filter_left) ||
	(col > screen->loc_filter_right) ||
	(row < screen->loc_filter_top) ||
	(row > screen->loc_filter_bottom)) {
	/* Filter triggered - disable it */
	screen->loc_filter = False;
	MotionOff(screen, xw);

	memset(&reply, 0, sizeof(reply));
	reply.a_type = ANSI_CSI;
	if (oor) {
	    reply.a_nparam = 1;
	    reply.a_param[0] = 0;	/* Event - 0 = locator unavailable */
	} else {
	    int state;

	    ButtonState(state, event->state);

	    reply.a_nparam = 4;
	    reply.a_param[0] = 10;	/* Event - 10 = locator outside filter */
	    reply.a_param[1] = (ParmType) state;
	    reply.a_param[2] = (ParmType) row;
	    reply.a_param[3] = (ParmType) col;
	}

	reply.a_inters = '&';
	reply.a_final = 'w';
	unparseseq(xw, &reply);

	if (screen->locator_reset) {
	    MotionOff(screen, xw);
	    screen->send_mouse_pos = MOUSE_OFF;
	}
    }
}