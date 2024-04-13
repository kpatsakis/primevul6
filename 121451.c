ReadLineButton(Widget w,
	       XEvent *event,	/* must be XButtonEvent */
	       String *params,	/* selections */
	       Cardinal *num_params)
{
    XtermWidget xw;

    if ((xw = getXtermWidget(w)) != 0) {
	TScreen *screen = TScreenOf(xw);
	Char Line[6];
	int line, col, ldelta = 0;

	if (!IsBtnEvent(event)
	    || (okSendMousePos(xw) != MOUSE_OFF) || ExtendingSelection)
	    goto finish;
	if (event->type == ButtonRelease) {
	    int delta;

	    if (lastButtonDownTime == (Time) 0) {
		/* first time and once in a blue moon */
		delta = screen->multiClickTime + 1;
	    } else if (event->xbutton.time > lastButtonDownTime) {
		/* most of the time */
		delta = (int) (event->xbutton.time - lastButtonDownTime);
	    } else {
		/* time has rolled over since lastButtonUpTime */
		delta = (int) ((((Time) ~ 0) - lastButtonDownTime) + event->xbutton.time);
	    }
	    if (delta > screen->multiClickTime)
		goto finish;	/* All this work for this... */
	}
	line = (event->xbutton.y - screen->border) / FontHeight(screen);
	if (!rowOnCurrentLine(screen, line, &ldelta))
	    goto finish;
	/* Correct by half a width - we are acting on a boundary, not on a cell. */
	col = (event->xbutton.x - OriginX(screen) + (FontWidth(screen) - 1)
	       / 2)
	    / FontWidth(screen) - screen->cur_col + ldelta * MaxCols(screen);
	if (col == 0)
	    goto finish;
	Line[0] = ANSI_ESC;
	/* XXX: sometimes it is better to send '['? */
	Line[1] = 'O';
	Line[2] = CharOf(col > 0 ? 'C' : 'D');
	if (col < 0)
	    col = -col;
	while (col--)
	    v_write(screen->respond, Line, 3);
      finish:
	if (event->type == ButtonRelease)
	    do_select_end(xw, event, params, num_params, False);
    }
}