ClrUpdate(NCURSES_SP_DCL0)
{
    TR(TRACE_UPDATE, (T_CALLED("ClrUpdate")));
    if (0 != SP_PARM) {
	int i;
	NCURSES_CH_T blank = ClrBlank(NCURSES_SP_ARGx StdScreen(SP_PARM));
	int nonempty = min(screen_lines(SP_PARM),
			   NewScreen(SP_PARM)->_maxy + 1);

	ClearScreen(NCURSES_SP_ARGx blank);

	TR(TRACE_UPDATE, ("updating screen from scratch"));

	nonempty = ClrBottom(NCURSES_SP_ARGx nonempty);

	for (i = 0; i < nonempty; i++)
	    TransformLine(NCURSES_SP_ARGx i);
    }
    TR(TRACE_UPDATE, (T_RETURN("")));
}