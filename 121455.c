CheckSecondPress3(XtermWidget xw, TScreen *screen, XEvent *event)
{
    int delta;

    if (event->type != ButtonPress
	|| OverrideEvent(event)
	|| event->xbutton.button != Button3) {
	lastButton3DoubleDownTime = 0;	/* Disable the cached info */
	return 0;
    }
    /* Process Btn3Press. */
    if (lastButton3UpTime == (Time) 0) {
	/* No previous click or once in a blue moon */
	delta = screen->multiClickTime + 1;
    } else if (event->xbutton.time > lastButton3UpTime) {
	/* most of the time */
	delta = (int) (event->xbutton.time - lastButton3UpTime);
    } else {
	/* time has rolled over since lastButton3UpTime */
	delta = (int) ((((Time) ~ 0) - lastButton3UpTime) + event->xbutton.time);
    }
    if (delta <= screen->multiClickTime) {
	CELL cell;

	PointToCELL(screen, event->xbutton.y, event->xbutton.x, &cell);
	if (isSameCELL(&cell, &lastButton3)) {
	    /* A candidate for a double-click */
	    lastButton3DoubleDownTime = event->xbutton.time;
	    PointToCELL(screen, event->xbutton.y, event->xbutton.x, &lastButton3);
	    return 1;
	}
	lastButton3UpTime = 0;	/* Disable the info about the previous click */
    }
    /* Either too long, or moved, disable. */
    lastButton3DoubleDownTime = 0;
    return 0;
}