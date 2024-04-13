cellToColumn(TScreen *screen, CELL *cell)
{
    CLineData *ld = 0;
    int col = cell->col;
    int row = firstRowOfLine(screen, cell->row, False);
    while (row < cell->row) {
	ld = GET_LINEDATA(screen, row);
	col += LastTextCol(screen, ld, row++);
    }
#if OPT_DEC_CHRSET
    if (ld == 0)
	ld = GET_LINEDATA(screen, row);
    if (CSET_DOUBLE(GetLineDblCS(ld)))
	col /= 2;
#endif
    return col;
}