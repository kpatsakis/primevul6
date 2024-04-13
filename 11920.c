drv_rescol(TERMINAL_CONTROL_BLOCK * TCB)
{
    bool result = FALSE;
    SCREEN *sp;

    AssertTCB();
    SetSP();

    if (orig_pair != 0) {
	NCURSES_PUTP2("orig_pair", orig_pair);
	result = TRUE;
    }
    return result;
}