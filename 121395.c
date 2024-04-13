ReHiliteText(XtermWidget xw,
	     CELL *firstp,
	     CELL *lastp)
{
    TScreen *screen = TScreenOf(xw);
    CELL first = *firstp;
    CELL last = *lastp;

    TRACE(("ReHiliteText from %d.%d to %d.%d\n",
	   first.row, first.col, last.row, last.col));

    if (first.row < 0)
	first.row = first.col = 0;
    else if (first.row > screen->max_row)
	return;			/* nothing to do, since last.row >= first.row */

    if (last.row < 0)
	return;			/* nothing to do, since first.row <= last.row */
    else if (last.row > screen->max_row) {
	last.row = screen->max_row;
	last.col = MaxCols(screen);
    }
    if (isSameCELL(&first, &last))
	return;

    if (!isSameRow(&first, &last)) {	/* do multiple rows */
	int i;
	if ((i = screen->max_col - first.col + 1) > 0) {	/* first row */
	    ScrnRefresh(xw, first.row, first.col, 1, i, True);
	}
	if ((i = last.row - first.row - 1) > 0) {	/* middle rows */
	    ScrnRefresh(xw, first.row + 1, 0, i, MaxCols(screen), True);
	}
	if (last.col > 0 && last.row <= screen->max_row) {	/* last row */
	    ScrnRefresh(xw, last.row, 0, 1, last.col, True);
	}
    } else {			/* do single row */
	ScrnRefresh(xw, first.row, first.col, 1, last.col - first.col, True);
    }
}