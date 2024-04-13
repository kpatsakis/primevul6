okPosition(TScreen *screen,
	   LineData **ld,
	   CELL *cell)
{
    Boolean result = True;

    if (cell->row > screen->max_row) {
	result = False;
	TRACE(("okPosition cell row %d > screen max %d\n", cell->row, screen->max_row));
    } else if (cell->col > (LastTextCol(screen, *ld, cell->row) + 1)) {
	TRACE(("okPosition cell col %d > screen max %d\n", cell->col,
	       (LastTextCol(screen, *ld, cell->row) + 1)));
	if (cell->row < screen->max_row) {
	    TRACE(("okPosition cell row %d < screen max %d\n", cell->row, screen->max_row));
	    cell->col = 0;
	    *ld = GET_LINEDATA(screen, ++cell->row);
	    result = False;
	}
    }
    return result;
}