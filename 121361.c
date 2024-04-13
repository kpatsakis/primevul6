firstRowOfLine(TScreen *screen, int row, Bool visible)
{
    LineData *ld = 0;
    int limit = visible ? 0 : -screen->savedlines;

    while (row > limit &&
	   (ld = GET_LINEDATA(screen, row - 1)) != 0 &&
	   LineTstWrapped(ld)) {
	--row;
    }
    return row;
}