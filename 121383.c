make_indexed_text(TScreen *screen, int row, unsigned length, int *indexed)
{
    Char *result = 0;
    size_t need = (length + 1);

    /*
     * Get a quick upper bound to the number of bytes needed, if the whole
     * string were UTF-8.
     */
    if_OPT_WIDE_CHARS(screen, {
	need *= ((screen->lineExtra + 1) * 6);
    });

    if ((result = TypeCallocN(Char, need + 1)) != 0) {
	LineData *ld = GET_LINEDATA(screen, row);
	unsigned used = 0;
	Char *last = result;

	do {
	    int col = 0;
	    int limit = LastTextCol(screen, ld, row);

	    while (col <= limit) {
		Char *next = last;
		unsigned data = ld->charData[col];

		assert(col < (int) ld->lineSize);
		/* some internal points may not be drawn */
		if (data == 0)
		    data = ' ';

		if_WIDE_OR_NARROW(screen, {
		    next = convertToUTF8(last, data);
		}
		, {
		    *next++ = CharOf(data);
		});

		if_OPT_WIDE_CHARS(screen, {
		    size_t off;
		    for_each_combData(off, ld) {
			data = ld->combData[off][col];
			if (data == 0)
			    break;
			next = convertToUTF8(next, data);
		    }
		});

		indexed[used] = (int) (last - result);
		*next = 0;
		/* TRACE(("index[%d.%d] %d:%s\n", row, used, indexed[used], last)); */
		last = next;
		++used;
		++col;
		indexed[used] = (int) (next - result);
	    }
	} while (used < length &&
		 LineTstWrapped(ld) &&
		 (ld = GET_LINEDATA(screen, ++row)) != 0 &&
		 row < screen->max_row);
    }
    /* TRACE(("result:%s\n", result)); */
    return (char *) result;
}