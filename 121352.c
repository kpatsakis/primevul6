rowOnCurrentLine(TScreen *screen,
		 int line,
		 int *deltap)	/* must be XButtonEvent */
{
    int result = 1;

    *deltap = 0;

    if (line != screen->cur_row) {
	int l1, l2;

	if (line < screen->cur_row) {
	    l1 = line;
	    l2 = screen->cur_row;
	} else {
	    l2 = line;
	    l1 = screen->cur_row;
	}
	l1--;
	while (++l1 < l2) {
	    LineData *ld = GET_LINEDATA(screen, l1);
	    if (!LineTstWrapped(ld)) {
		result = 0;
		break;
	    }
	}
	if (result) {
	    /* Everything is on one "wrapped line" now */
	    *deltap = line - screen->cur_row;
	}
    }
    return result;
}