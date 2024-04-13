decode_X10_bstate(SCREEN *sp, MEVENT * eventp, unsigned intro)
{
    bool result;
    int button = 0;
    int wheel = (intro & 96) == 96;

    eventp->bstate = 0;

    if (intro >= 96) {
	if (intro >= 160) {
	    button = (int) (intro - 152);	/* buttons 8-11 */
	} else if (intro >= 96) {
	    button = (int) (intro - 92);	/* buttons 4-7 */
	}
    } else {
	button = (intro & 3);
    }

    if (button > MAX_BUTTONS) {
	eventp->bstate = REPORT_MOUSE_POSITION;
    } else if (!handle_wheel(sp, eventp, (int) intro, wheel)) {

	/*
	 * Release events aren't reported for individual buttons, just for
	 * the button set as a whole.  However, because there are normally
	 * no mouse events under xterm that intervene between press and
	 * release, we can infer the button actually released by looking at
	 * the previous event.
	 */
	if (sp->_mouse_bstate & BUTTON_PRESSED) {
	    int b;

	    eventp->bstate = BUTTON_RELEASED;
	    for (b = 1; b <= MAX_BUTTONS; ++b) {
		if (!(sp->_mouse_bstate & MASK_PRESS(b)))
		    eventp->bstate &= ~MASK_RELEASE(b);
	    }
	    sp->_mouse_bstate = 0;
	} else {
	    /*
	     * xterm will return a stream of release-events to let the
	     * application know where the mouse is going, if private mode
	     * 1002 or 1003 is enabled.
	     */
	    eventp->bstate = REPORT_MOUSE_POSITION;
	}
    }

    if (intro & 4) {
	eventp->bstate |= BUTTON_SHIFT;
    }
    if (intro & 8) {
	eventp->bstate |= BUTTON_ALT;
    }
    if (intro & 16) {
	eventp->bstate |= BUTTON_CTRL;
    }
    result = (eventp->bstate & REPORT_MOUSE_POSITION) ? TRUE : FALSE;
    return result;
}