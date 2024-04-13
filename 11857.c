ClrBottom(NCURSES_SP_DCLx int total)
{
    int top = total;
    int last = min(screen_columns(SP_PARM), NewScreen(SP_PARM)->_maxx + 1);
    NCURSES_CH_T blank = NewScreen(SP_PARM)->_line[total - 1].text[last - 1];

    if (clr_eos && can_clear_with(NCURSES_SP_ARGx CHREF(blank))) {
	int row;

	for (row = total - 1; row >= 0; row--) {
	    int col;
	    bool ok;

	    for (col = 0, ok = TRUE; ok && col < last; col++) {
		ok = (CharEq(NewScreen(SP_PARM)->_line[row].text[col], blank));
	    }
	    if (!ok)
		break;

	    for (col = 0; ok && col < last; col++) {
		ok = (CharEq(CurScreen(SP_PARM)->_line[row].text[col], blank));
	    }
	    if (!ok)
		top = row;
	}

	/* don't use clr_eos for just one line if clr_eol available */
	if (top < total) {
	    GoTo(NCURSES_SP_ARGx top, 0);
	    ClrToEOS(NCURSES_SP_ARGx blank);
	    if (SP_PARM->oldhash && SP_PARM->newhash) {
		for (row = top; row < screen_lines(SP_PARM); row++)
		    SP_PARM->oldhash[row] = SP_PARM->newhash[row];
	    }
	}
    }
    return top;
}