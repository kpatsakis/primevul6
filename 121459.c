trimLastLine(TScreen *screen,
	     LineData **ld,
	     CELL *last)
{
    if (screen->cutNewline && last->row < screen->max_row) {
	last->col = 0;
	*ld = GET_LINEDATA(screen, ++last->row);
    } else {
	last->col = LastTextCol(screen, *ld, last->row) + 1;
    }
}