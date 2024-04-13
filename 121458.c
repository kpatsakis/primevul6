ViButton(Widget w,
	 XEvent *event,		/* must be XButtonEvent */
	 String *params GCC_UNUSED,	/* selections */
	 Cardinal *num_params GCC_UNUSED)
{
    XtermWidget xw;

    if ((xw = getXtermWidget(w)) != 0) {
	TScreen *screen = TScreenOf(xw);
	int pty = screen->respond;

	if (IsBtnEvent(event)) {
	    int line;

	    line = screen->cur_row -
		((event->xbutton.y - screen->border) / FontHeight(screen));

	    if (line != 0) {
		Char Line[6];

		Line[0] = ANSI_ESC;	/* force an exit from insert-mode */
		v_write(pty, Line, 1);

		if (line < 0) {
		    line = -line;
		    Line[0] = CONTROL('n');
		} else {
		    Line[0] = CONTROL('p');
		}
		while (--line >= 0)
		    v_write(pty, Line, 1);
	    }
	}
    }
}