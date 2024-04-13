columnToCell(TScreen *screen, int row, int col, CELL *cell)
{
    while (row < screen->max_row) {
	CLineData *ld = GET_LINEDATA(screen, row);
	int last = LastTextCol(screen, ld, row);

	/* TRACE(("last(%d) = %d, have %d\n", row, last, col)); */
	if (col <= last) {
	    break;
	}
	/*
	 * Stop if the current row does not wrap (does not continue the current
	 * line).
	 */
	if (!LineTstWrapped(ld)) {
	    col = last + 1;
	    break;
	}
	col -= (last + 1);
	++row;
    }
    if (col < 0)
	col = 0;
    cell->row = row;
    cell->col = col;
}