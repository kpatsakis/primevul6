Length(TScreen *screen,
       int row,
       int scol,
       int ecol)
{
    CLineData *ld = GET_LINEDATA(screen, row);
    const int lastcol = LastTextCol(screen, ld, row);

    if (ecol > lastcol)
	ecol = lastcol;
    return (ecol - scol + 1);
}