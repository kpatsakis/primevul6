isDoubleClick3(XtermWidget xw, TScreen *screen, XButtonEvent *event)
{
    int delta;

    if (event->type != ButtonRelease
	|| OverrideButton(event)
	|| event->button != Button3) {
	lastButton3UpTime = 0;	/* Disable the cached info */
	return 0;
    }
    /* Process Btn3Release. */
    if (lastButton3DoubleDownTime == (Time) 0) {
	/* No previous click or once in a blue moon */
	delta = screen->multiClickTime + 1;
    } else if (event->time > lastButton3DoubleDownTime) {
	/* most of the time */
	delta = (int) (event->time - lastButton3DoubleDownTime);
    } else {
	/* time has rolled over since lastButton3DoubleDownTime */
	delta = (int) ((((Time) ~ 0) - lastButton3DoubleDownTime) + event->time);
    }
    if (delta <= screen->multiClickTime) {
	/* Double click */
	CELL cell;

	/* Cannot check ExtendingSelection, since mouse-3 always sets it */
	PointToCELL(screen, event->y, event->x, &cell);
	if (isSameCELL(&cell, &lastButton3)) {
	    lastButton3DoubleDownTime = 0;	/* Disable the third click */
	    return 1;
	}
    }
    /* Not a double click, memorize for future check. */
    lastButton3UpTime = event->time;
    PointToCELL(screen, event->y, event->x, &lastButton3);
    return 0;
}