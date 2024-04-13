TrackText(XtermWidget xw,
	  const CELL *firstp,
	  const CELL *lastp)
{
    TScreen *screen = TScreenOf(xw);
    int from, to;
    CELL old_start, old_end;
    CELL first = *firstp;
    CELL last = *lastp;

    TRACE(("TrackText(first=%d,%d, last=%d,%d)\n",
	   first.row, first.col, last.row, last.col));

    old_start = screen->startH;
    old_end = screen->endH;
    TRACE(("...previous(first=%d,%d, last=%d,%d)\n",
	   old_start.row, old_start.col,
	   old_end.row, old_end.col));
    if (isSameCELL(&first, &old_start) &&
	isSameCELL(&last, &old_end)) {
	return;
    }

    screen->startH = first;
    screen->endH = last;
    from = Coordinate(screen, &screen->startH);
    to = Coordinate(screen, &screen->endH);
    if (to <= screen->startHCoord || from > screen->endHCoord) {
	/* No overlap whatsoever between old and new hilite */
	ReHiliteText(xw, &old_start, &old_end);
	ReHiliteText(xw, &first, &last);
    } else {
	if (from < screen->startHCoord) {
	    /* Extend left end */
	    ReHiliteText(xw, &first, &old_start);
	} else if (from > screen->startHCoord) {
	    /* Shorten left end */
	    ReHiliteText(xw, &old_start, &first);
	}
	if (to > screen->endHCoord) {
	    /* Extend right end */
	    ReHiliteText(xw, &old_end, &last);
	} else if (to < screen->endHCoord) {
	    /* Shorten right end */
	    ReHiliteText(xw, &last, &old_end);
	}
    }
    screen->startHCoord = from;
    screen->endHCoord = to;
}