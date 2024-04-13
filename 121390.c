DiredButton(Widget w,
	    XEvent *event,	/* must be XButtonEvent */
	    String *params GCC_UNUSED,	/* selections */
	    Cardinal *num_params GCC_UNUSED)
{
    XtermWidget xw;

    if ((xw = getXtermWidget(w)) != 0) {
	TScreen *screen = TScreenOf(xw);

	if (IsBtnEvent(event)
	    && (event->xbutton.y >= screen->border)
	    && (event->xbutton.x >= OriginX(screen))) {
	    Char Line[6];
	    unsigned line, col;

	    line = (unsigned) ((event->xbutton.y - screen->border)
			       / FontHeight(screen));
	    col = (unsigned) ((event->xbutton.x - OriginX(screen))
			      / FontWidth(screen));
	    Line[0] = CONTROL('X');
	    Line[1] = ANSI_ESC;
	    Line[2] = 'G';
	    Line[3] = CharOf(' ' + col);
	    Line[4] = CharOf(' ' + line);
	    v_write(screen->respond, Line, 5);
	}
    }
}