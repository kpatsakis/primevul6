lengthOfLines(TScreen *screen, int firstRow, int lastRow)
{
    unsigned length = 0;
    int n;

    for (n = firstRow; n <= lastRow; ++n) {
	LineData *ld = GET_LINEDATA(screen, n);
	int value = LastTextCol(screen, ld, n);
	if (value >= 0)
	    length += (unsigned) (value + 1);
    }
    return length;
}