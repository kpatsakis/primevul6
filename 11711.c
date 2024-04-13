drv_rescolors(TERMINAL_CONTROL_BLOCK * TCB)
{
    int result = FALSE;
    SCREEN *sp;

    AssertTCB();
    SetSP();

    if (orig_colors != 0) {
	NCURSES_PUTP2("orig_colors", orig_colors);
	result = TRUE;
    }
    return result;
}