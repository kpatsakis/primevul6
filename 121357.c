LastTextCol(TScreen *screen, CLineData *ld, int row)
{
    int i = -1;

    if (ld != 0) {
	if (okScrnRow(screen, row)) {
	    const IAttr *ch;
	    for (i = screen->max_col,
		 ch = ld->attribs + i;
		 i >= 0 && !(*ch & CHARDRAWN);
		 ch--, i--) {
		;
	    }
#if OPT_DEC_CHRSET
	    if (CSET_DOUBLE(GetLineDblCS(ld))) {
		i *= 2;
	    }
#endif
	}
    }
    return (i);
}