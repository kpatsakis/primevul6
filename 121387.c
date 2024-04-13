GetLocatorPosition(XtermWidget xw)
{
    ANSI reply;
    TScreen *screen = TScreenOf(xw);
    Window root, child;
    int rx, ry, x, y;
    unsigned int mask = 0;
    int row = 0, col = 0;
    Bool oor = False;
    Bool ret = False;
    int state;

    /*
     * DECterm turns the Locator off if the position is requested while a
     * filter rectangle is active.  This might be a bug, but I don't know, so
     * I'll emulate it anyways.
     */
    if (screen->loc_filter) {
	screen->send_mouse_pos = MOUSE_OFF;
	screen->loc_filter = False;
	screen->locator_events = 0;
	MotionOff(screen, xw);
    }

    memset(&reply, 0, sizeof(reply));
    reply.a_type = ANSI_CSI;

    if (okSendMousePos(xw) == DEC_LOCATOR) {
	ret = XQueryPointer(screen->display, VWindow(screen), &root,
			    &child, &rx, &ry, &x, &y, &mask);
	if (ret) {
	    LocatorCoords(row, col, x, y, oor);
	}
    }
    if (ret == False || oor) {
	reply.a_nparam = 1;
	reply.a_param[0] = 0;	/* Event - 0 = locator unavailable */
	reply.a_inters = '&';
	reply.a_final = 'w';
	unparseseq(xw, &reply);

	if (screen->locator_reset) {
	    MotionOff(screen, xw);
	    screen->send_mouse_pos = MOUSE_OFF;
	}
	return;
    }

    ButtonState(state, mask);

    reply.a_nparam = 4;
    reply.a_param[0] = 1;	/* Event - 1 = response to locator request */
    reply.a_param[1] = (ParmType) state;
    reply.a_param[2] = (ParmType) row;
    reply.a_param[3] = (ParmType) col;
    reply.a_inters = '&';
    reply.a_final = 'w';
    unparseseq(xw, &reply);

    if (screen->locator_reset) {
	MotionOff(screen, xw);
	screen->send_mouse_pos = MOUSE_OFF;
    }
}