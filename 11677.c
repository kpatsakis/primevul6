scroll_csr_forward(NCURSES_SP_DCLx
		   int n,
		   int top,
		   int bot,
		   int miny,
		   int maxy,
		   NCURSES_CH_T blank)
{
    int i;

    if (n == 1 && scroll_forward && top == miny && bot == maxy) {
	GoTo(NCURSES_SP_ARGx bot, 0);
	UpdateAttrs(SP_PARM, blank);
	NCURSES_PUTP2("scroll_forward", scroll_forward);
    } else if (n == 1 && delete_line && bot == maxy) {
	GoTo(NCURSES_SP_ARGx top, 0);
	UpdateAttrs(SP_PARM, blank);
	NCURSES_PUTP2("delete_line", delete_line);
    } else if (parm_index && top == miny && bot == maxy) {
	GoTo(NCURSES_SP_ARGx bot, 0);
	UpdateAttrs(SP_PARM, blank);
	TPUTS_TRACE("parm_index");
	NCURSES_SP_NAME(tputs) (NCURSES_SP_ARGx
				TIPARM_1(parm_index, n),
				n,
				NCURSES_SP_NAME(_nc_outch));
    } else if (parm_delete_line && bot == maxy) {
	GoTo(NCURSES_SP_ARGx top, 0);
	UpdateAttrs(SP_PARM, blank);
	TPUTS_TRACE("parm_delete_line");
	NCURSES_SP_NAME(tputs) (NCURSES_SP_ARGx
				TIPARM_1(parm_delete_line, n),
				n,
				NCURSES_SP_NAME(_nc_outch));
    } else if (scroll_forward && top == miny && bot == maxy) {
	GoTo(NCURSES_SP_ARGx bot, 0);
	UpdateAttrs(SP_PARM, blank);
	for (i = 0; i < n; i++) {
	    NCURSES_PUTP2("scroll_forward", scroll_forward);
	}
    } else if (delete_line && bot == maxy) {
	GoTo(NCURSES_SP_ARGx top, 0);
	UpdateAttrs(SP_PARM, blank);
	for (i = 0; i < n; i++) {
	    NCURSES_PUTP2("delete_line", delete_line);
	}
    } else
	return ERR;

#if NCURSES_EXT_FUNCS
    if (FILL_BCE(SP_PARM)) {
	int j;
	for (i = 0; i < n; i++) {
	    GoTo(NCURSES_SP_ARGx bot - i, 0);
	    for (j = 0; j < screen_columns(SP_PARM); j++)
		PutChar(NCURSES_SP_ARGx CHREF(blank));
	}
    }
#endif
    return OK;
}