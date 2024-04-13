EditorButton(XtermWidget xw, XButtonEvent *event)
{
    TScreen *screen = TScreenOf(xw);
    int pty = screen->respond;
    int mouse_limit = MouseLimit(screen);
    Char line[32];
    Char final = 'M';
    int row, col;
    int button;
    unsigned count = 0;
    Boolean changed = True;

    /* If button event, get button # adjusted for DEC compatibility */
    button = (int) (event->button - 1);
    if (button >= 3)
	button++;

    /* Ignore buttons that cannot be encoded */
    if (screen->send_mouse_pos == X10_MOUSE) {
	if (button > 3)
	    return;
    } else if (screen->extend_coords == SET_SGR_EXT_MODE_MOUSE
	       || screen->extend_coords == SET_URXVT_EXT_MODE_MOUSE
	       || screen->extend_coords == SET_PIXEL_POSITION_MOUSE) {
	if (button > 15) {
	    return;
	}
    } else {
	if (button > 11) {
	    return;
	}
    }

    if (screen->extend_coords == SET_PIXEL_POSITION_MOUSE) {
	row = event->y - OriginY(screen);
	col = event->x - OriginX(screen);
    } else {
	/* Compute character position of mouse pointer */
	row = (event->y - screen->border) / FontHeight(screen);
	col = (event->x - OriginX(screen)) / FontWidth(screen);

	/* Limit to screen dimensions */
	if (row < 0)
	    row = 0;
	else if (row > screen->max_row)
	    row = screen->max_row;

	if (col < 0)
	    col = 0;
	else if (col > screen->max_col)
	    col = screen->max_col;

	if (mouse_limit > 0) {
	    /* Limit to representable mouse dimensions */
	    if (row > mouse_limit)
		row = mouse_limit;
	    if (col > mouse_limit)
		col = mouse_limit;
	}
    }

    /* Build key sequence starting with \E[M */
    if (screen->control_eight_bits) {
	line[count++] = ANSI_CSI;
    } else {
	line[count++] = ANSI_ESC;
	line[count++] = '[';
    }
    switch (screen->extend_coords) {
    case 0:
    case SET_EXT_MODE_MOUSE:
#if OPT_SCO_FUNC_KEYS
	if (xw->keyboard.type == keyboardIsSCO) {
	    /*
	     * SCO function key F1 is \E[M, which would conflict with xterm's
	     * normal kmous.
	     */
	    line[count++] = '>';
	}
#endif
	line[count++] = final;
	break;
    case SET_SGR_EXT_MODE_MOUSE:
    case SET_PIXEL_POSITION_MOUSE:
	line[count++] = '<';
	break;
    }

    /* Add event code to key sequence */
    if (okSendMousePos(xw) == X10_MOUSE) {
	count = EMIT_BUTTON(button);
    } else {
	/* Button-Motion events */
	switch (event->type) {
	case ButtonPress:
	    screen->mouse_button |= ButtonBit(button);
	    count = EMIT_BUTTON(button);
	    break;
	case ButtonRelease:
	    /*
	     * The (vertical) wheel mouse interface generates release-events
	     * for buttons 4 and 5.
	     *
	     * The X10/X11 xterm protocol maps the release for buttons 1..3 to
	     * a -1, which will be later mapped into a "0" (some button was
	     * released),  At this point, buttons 1..3 are encoded 0..2 (the
	     * code 3 is unused).
	     *
	     * The SGR (extended) xterm mouse protocol keeps the button number
	     * and uses a "m" to indicate button release.
	     *
	     * The behavior for mice with more buttons is unclear, and may be
	     * revised -TD
	     */
	    screen->mouse_button &= ~ButtonBit(button);
	    if (button < 3 || button > 5) {
		switch (screen->extend_coords) {
		case SET_SGR_EXT_MODE_MOUSE:
		case SET_PIXEL_POSITION_MOUSE:
		    final = 'm';
		    break;
		default:
		    button = -1;
		    break;
		}
	    }
	    count = EMIT_BUTTON(button);
	    break;
	case MotionNotify:
	    /* BTN_EVENT_MOUSE and ANY_EVENT_MOUSE modes send motion
	     * events only if character cell has changed.
	     */
	    if ((row == screen->mouse_row)
		&& (col == screen->mouse_col)) {
		changed = False;
	    } else {
		count = EMIT_BUTTON(FirstBitN(screen->mouse_button));
	    }
	    break;
	default:
	    changed = False;
	    break;
	}
    }

    if (changed) {
	screen->mouse_row = row;
	screen->mouse_col = col;

	TRACE(("mouse at %d,%d button+mask = %#x\n", row, col, line[count - 1]));

	/* Add pointer position to key sequence */
	count = EmitMousePositionSeparator(screen, line, count);
	count = EmitMousePosition(screen, line, count, col);
	count = EmitMousePositionSeparator(screen, line, count);
	count = EmitMousePosition(screen, line, count, row);

	switch (screen->extend_coords) {
	case SET_SGR_EXT_MODE_MOUSE:
	case SET_URXVT_EXT_MODE_MOUSE:
	case SET_PIXEL_POSITION_MOUSE:
	    line[count++] = final;
	    break;
	}

	/* Transmit key sequence to process running under xterm */
	TRACE(("EditorButton -> %s\n", visibleChars(line, count)));
	v_write(pty, line, count);
    }
    return;
}