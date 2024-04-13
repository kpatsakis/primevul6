isClick1_clean(XtermWidget xw, XButtonEvent *event)
{
    TScreen *screen = TScreenOf(xw);
    int delta;

    /* Disable on Shift-Click-1, including the application-mouse modes */
    if (OverrideButton(event)
	|| (okSendMousePos(xw) != MOUSE_OFF)
	|| ExtendingSelection)	/* Was moved */
	return 0;

    if (event->type != ButtonRelease)
	return 0;

    if (lastButtonDownTime == (Time) 0) {
	/* first time or once in a blue moon */
	delta = screen->multiClickTime + 1;
    } else if (event->time > lastButtonDownTime) {
	/* most of the time */
	delta = (int) (event->time - lastButtonDownTime);
    } else {
	/* time has rolled over since lastButtonUpTime */
	delta = (int) ((((Time) ~ 0) - lastButtonDownTime) + event->time);
    }

    return delta <= screen->multiClickTime;
}