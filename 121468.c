EndExtend(XtermWidget xw,
	  XEvent *event,	/* must be XButtonEvent */
	  String *params,	/* selections */
	  Cardinal num_params,
	  Bool use_cursor_loc)
{
    CELL cell;
    TScreen *screen = TScreenOf(xw);

    TRACE_EVENT("EndExtend", event, params, &num_params);
    if (use_cursor_loc) {
	cell = screen->cursorp;
    } else {
	PointToCELL(screen, event->xbutton.y, event->xbutton.x, &cell);
    }
    ExtendExtend(xw, &cell);

    screen->lastButtonUpTime = event->xbutton.time;
    screen->lastButton = event->xbutton.button;

    if (!isSameCELL(&(screen->startSel), &(screen->endSel))) {
	if (screen->replyToEmacs) {
	    Char line[64];
	    unsigned count = 0;

	    if (screen->control_eight_bits) {
		line[count++] = ANSI_CSI;
	    } else {
		line[count++] = ANSI_ESC;
		line[count++] = '[';
	    }
	    if (isSameCELL(&(screen->rawPos), &(screen->startSel))
		&& isSameCELL(&cell, &(screen->endSel))) {
		/* Use short-form emacs select */

		switch (screen->extend_coords) {
		case 0:
		case SET_EXT_MODE_MOUSE:
		    line[count++] = 't';
		    break;
		case SET_SGR_EXT_MODE_MOUSE:
		case SET_PIXEL_POSITION_MOUSE:
		    line[count++] = '<';
		    break;
		}

		count = EmitMousePosition(screen, line, count, screen->endSel.col);
		count = EmitMousePositionSeparator(screen, line, count);
		count = EmitMousePosition(screen, line, count, screen->endSel.row);

		switch (screen->extend_coords) {
		case SET_SGR_EXT_MODE_MOUSE:
		case SET_URXVT_EXT_MODE_MOUSE:
		case SET_PIXEL_POSITION_MOUSE:
		    line[count++] = 't';
		    break;
		}
	    } else {
		/* long-form, specify everything */

		switch (screen->extend_coords) {
		case 0:
		case SET_EXT_MODE_MOUSE:
		    line[count++] = 'T';
		    break;
		case SET_SGR_EXT_MODE_MOUSE:
		case SET_PIXEL_POSITION_MOUSE:
		    line[count++] = '<';
		    break;
		}

		count = EmitMousePosition(screen, line, count, screen->startSel.col);
		count = EmitMousePositionSeparator(screen, line, count);
		count = EmitMousePosition(screen, line, count, screen->startSel.row);
		count = EmitMousePositionSeparator(screen, line, count);
		count = EmitMousePosition(screen, line, count, screen->endSel.col);
		count = EmitMousePositionSeparator(screen, line, count);
		count = EmitMousePosition(screen, line, count, screen->endSel.row);
		count = EmitMousePositionSeparator(screen, line, count);
		count = EmitMousePosition(screen, line, count, cell.col);
		count = EmitMousePositionSeparator(screen, line, count);
		count = EmitMousePosition(screen, line, count, cell.row);

		switch (screen->extend_coords) {
		case SET_SGR_EXT_MODE_MOUSE:
		case SET_URXVT_EXT_MODE_MOUSE:
		case SET_PIXEL_POSITION_MOUSE:
		    line[count++] = 'T';
		    break;
		}
	    }
	    v_write(screen->respond, line, count);
	    UnHiliteText(xw);
	}
    }
    SelectSet(xw, event, params, num_params);
    screen->eventMode = NORMAL;
}