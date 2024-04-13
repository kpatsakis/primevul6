TrackMouse(XtermWidget xw,
	   int func,
	   CELL *start,
	   int firstrow,
	   int lastrow)
{
    TScreen *screen = TScreenOf(xw);

    if (screen->waitingForTrackInfo) {	/* if Timed, ignore */
	screen->waitingForTrackInfo = False;

	if (func != 0) {
	    CELL first = *start;

	    boundsCheck(first.row);
	    boundsCheck(firstrow);
	    boundsCheck(lastrow);
	    screen->firstValidRow = firstrow;
	    screen->lastValidRow = lastrow;
	    screen->replyToEmacs = True;
	    StartSelect(xw, &first);
	}
    }
}