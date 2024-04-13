ScrollSelection(TScreen *screen, int amount, Bool always)
{
    int minrow = INX2ROW(screen, -screen->savedlines);
    int maxrow = INX2ROW(screen, screen->max_row);
    int maxcol = screen->max_col;

#define scroll_update_one(cell) \
	(cell)->row += amount; \
	if ((cell)->row < minrow) { \
	    (cell)->row = minrow; \
	    (cell)->col = 0; \
	} \
	if ((cell)->row > maxrow) { \
	    (cell)->row = maxrow; \
	    (cell)->col = maxcol; \
	}

    scroll_update_one(&(screen->startRaw));
    scroll_update_one(&(screen->endRaw));
    scroll_update_one(&(screen->startSel));
    scroll_update_one(&(screen->endSel));

    scroll_update_one(&(screen->rawPos));

    /*
     * If we are told to scroll the selection but it lies outside the scrolling
     * margins, then that could cause the selection to move (bad).  It is not
     * simple to fix, because this function is called both for the scrollbar
     * actions as well as application scrolling.  The 'always' flag is set in
     * the former case.  The rest of the logic handles the latter.
     */
    if (ScrnHaveSelection(screen)) {
	int adjust;

	adjust = ROW2INX(screen, screen->startH.row);
	if (always
	    || !ScrnHaveRowMargins(screen)
	    || ScrnIsRowInMargins(screen, adjust)) {
	    scroll_update_one(&screen->startH);
	}
	adjust = ROW2INX(screen, screen->endH.row);
	if (always
	    || !ScrnHaveRowMargins(screen)
	    || ScrnIsRowInMargins(screen, adjust)) {
	    scroll_update_one(&screen->endH);
	}
    }

    screen->startHCoord = Coordinate(screen, &screen->startH);
    screen->endHCoord = Coordinate(screen, &screen->endH);
}