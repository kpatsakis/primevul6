decode_xterm_SGR1006(SCREEN *sp, MEVENT * eventp)
{
    SGR_DATA data;
    bool result = FALSE;
    if (read_SGR(sp, &data)) {
	int b = data.params[0];
	int b3 = 1 + (b & 3);
	int wheel = ((b & 64) == 64);

	if (b >= 132) {
	    b3 = MAX_BUTTONS + 1;
	} else if (b >= 128) {
	    b3 = (b - 120);	/* buttons 8-11 */
	} else if (b >= 64) {
	    b3 = (b - 60);	/* buttons 6-7 */
	}

	eventp->id = NORMAL_EVENT;
	if (data.final == 'M') {
	    (void) handle_wheel(sp, eventp, b, wheel);
	} else if (b3 > MAX_BUTTONS) {
	    eventp->bstate = REPORT_MOUSE_POSITION;
	} else {
	    mmask_t pressed = (mmask_t) NCURSES_MOUSE_MASK(b3, NCURSES_BUTTON_PRESSED);
	    mmask_t release = (mmask_t) NCURSES_MOUSE_MASK(b3, NCURSES_BUTTON_RELEASED);
	    if (sp->_mouse_bstate & pressed) {
		eventp->bstate = release;
		sp->_mouse_bstate &= ~pressed;
	    } else {
		eventp->bstate = REPORT_MOUSE_POSITION;
	    }
	}
	if (b & 4) {
	    eventp->bstate |= BUTTON_SHIFT;
	}
	if (b & 8) {
	    eventp->bstate |= BUTTON_ALT;
	}
	if (b & 16) {
	    eventp->bstate |= BUTTON_CTRL;
	}
	result = (eventp->bstate & REPORT_MOUSE_POSITION) ? TRUE : FALSE;
	eventp->x = (data.params[1] ? (data.params[1] - 1) : 0);
	eventp->y = (data.params[2] ? (data.params[2] - 1) : 0);
    }
    return result;
}