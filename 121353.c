lastRowOfLine(TScreen *screen, int row)
{
    LineData *ld;

    while (row < screen->max_row &&
	   (ld = GET_LINEDATA(screen, row)) != 0 &&
	   LineTstWrapped(ld)) {
	++row;
    }
    return row;
}