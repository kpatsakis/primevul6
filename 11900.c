PutRange(NCURSES_SP_DCLx
	 const NCURSES_CH_T *otext,
	 const NCURSES_CH_T *ntext,
	 int row,
	 int first, int last)
{
    int rc;

    TR(TRACE_CHARPUT, ("PutRange(%p, %p, %p, %d, %d, %d)",
		       (void *) SP_PARM,
		       (const void *) otext,
		       (const void *) ntext,
		       row, first, last));

    if (otext != ntext
	&& (last - first + 1) > SP_PARM->_inline_cost) {
	int i, j, same;

	for (j = first, same = 0; j <= last; j++) {
	    if (!same && isWidecExt(otext[j]))
		continue;
	    if (CharEq(otext[j], ntext[j])) {
		same++;
	    } else {
		if (same > SP_PARM->_inline_cost) {
		    EmitRange(NCURSES_SP_ARGx ntext + first, j - same - first);
		    GoTo(NCURSES_SP_ARGx row, first = j);
		}
		same = 0;
	    }
	}
	i = EmitRange(NCURSES_SP_ARGx ntext + first, j - same - first);
	/*
	 * Always return 1 for the next GoTo() after a PutRange() if we found
	 * identical characters at end of interval
	 */
	rc = (same == 0 ? i : 1);
    } else {
	rc = EmitRange(NCURSES_SP_ARGx ntext + first, last - first + 1);
    }
    return rc;
}