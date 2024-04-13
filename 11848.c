_nc_change_pair(SCREEN *sp, int pair)
{
    int y, x;

    if (CurScreen(sp)->_clear)
	return;
#if NO_LEAKS
    if (_nc_globals.leak_checking)
	return;
#endif

    for (y = 0; y <= CurScreen(sp)->_maxy; y++) {
	struct ldat *ptr = &(CurScreen(sp)->_line[y]);
	bool changed = FALSE;
	for (x = 0; x <= CurScreen(sp)->_maxx; x++) {
	    if (GetPair(ptr->text[x]) == pair) {
		/* Set the old cell to zero to ensure it will be
		   updated on the next doupdate() */
		SetChar(ptr->text[x], 0, 0);
		CHANGED_CELL(ptr, x);
		changed = TRUE;
	    }
	}
	if (changed)
	    NCURSES_SP_NAME(_nc_make_oldhash) (NCURSES_SP_ARGx y);
    }
}