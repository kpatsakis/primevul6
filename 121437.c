SendLocatorPosition(XtermWidget xw, XButtonEvent *event)
{
    ANSI reply;
    TScreen *screen = TScreenOf(xw);
    int row, col;
    Bool oor;
    int button;
    unsigned state;

    /* Make sure the event is an appropriate type */
    if (IsBtnEvent(event)) {
	if (OverrideButton(event))
	    return (False);
    } else {
	if (!screen->loc_filter)
	    return (False);
    }

    if ((event->type == ButtonPress &&
	 !(screen->locator_events & LOC_BTNS_DN)) ||
	(event->type == ButtonRelease &&
	 !(screen->locator_events & LOC_BTNS_UP)))
	return (True);

    if (event->type == MotionNotify) {
	CheckLocatorPosition(xw, event);
	return (True);
    }

    /* get button # */
    button = (int) event->button - 1;

    LocatorCoords(row, col, event->x, event->y, oor);

    /*
     * DECterm mouse:
     *
     * ESCAPE '[' event ; mask ; row ; column '&' 'w'
     */
    memset(&reply, 0, sizeof(reply));
    reply.a_type = ANSI_CSI;

    if (oor) {
	reply.a_nparam = 1;
	reply.a_param[0] = 0;	/* Event - 0 = locator unavailable */
	reply.a_inters = '&';
	reply.a_final = 'w';
	unparseseq(xw, &reply);

	if (screen->locator_reset) {
	    MotionOff(screen, xw);
	    screen->send_mouse_pos = MOUSE_OFF;
	}
	return (True);
    }

    /*
     * event:
     *        1       no buttons
     *        2       left button down
     *        3       left button up
     *        4       middle button down
     *        5       middle button up
     *        6       right button down
     *        7       right button up
     *        8       M4 down
     *        9       M4 up
     */
    reply.a_nparam = 4;
    switch (event->type) {
    case ButtonPress:
	reply.a_param[0] = (ParmType) (2 + (button << 1));
	break;
    case ButtonRelease:
	reply.a_param[0] = (ParmType) (3 + (button << 1));
	break;
    default:
	return (True);
    }
    /*
     * mask:
     * bit7   bit6   bit5   bit4   bit3     bit2       bit1         bit0
     *                             M4 down  left down  middle down  right down
     *
     * Notice that Button1 (left) and Button3 (right) are swapped in the mask.
     * Also, mask should be the state after the button press/release,
     * X provides the state not including the button press/release.
     */
    state = (event->state
	     & (Button1Mask | Button2Mask | Button3Mask | Button4Mask)) >> 8;
    /* update mask to "after" state */
    state ^= ((unsigned) (1 << button));
    /* swap Button1 & Button3 */
    state = ((state & (unsigned) ~(4 | 1))
	     | ((state & 1) ? 4 : 0)
	     | ((state & 4) ? 1 : 0));

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

    /*
     * DECterm turns the Locator off if a button is pressed while a filter
     * rectangle is active.  This might be a bug, but I don't know, so I'll
     * emulate it anyway.
     */
    if (screen->loc_filter) {
	screen->send_mouse_pos = MOUSE_OFF;
	screen->loc_filter = False;
	screen->locator_events = 0;
	MotionOff(screen, xw);
    }

    return (True);
}