do_select_start(XtermWidget xw,
		XEvent *event,	/* must be XButtonEvent* */
		CELL *cell)
{
    TScreen *screen = TScreenOf(xw);

    if (SendMousePosition(xw, event))
	return;
    screen->selectUnit = EvalSelectUnit(xw,
					event->xbutton.time,
					Select_CHAR,
					event->xbutton.button);
    screen->replyToEmacs = False;

#if OPT_READLINE
    lastButtonDownTime = event->xbutton.time;
#endif

    StartSelect(xw, cell);
}