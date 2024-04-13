drv_setfilter(TERMINAL_CONTROL_BLOCK * TCB)
{
    AssertTCB();

    /* *INDENT-EQLS* */
    clear_screen     = ABSENT_STRING;
    cursor_address   = ABSENT_STRING;
    cursor_down      = ABSENT_STRING;
    cursor_up        = ABSENT_STRING;
    parm_down_cursor = ABSENT_STRING;
    parm_up_cursor   = ABSENT_STRING;
    row_address      = ABSENT_STRING;
    cursor_home      = carriage_return;

    if (back_color_erase)
	clr_eos = ABSENT_STRING;
}