msg_check(void)
{
    if (msg_row == Rows - 1 && msg_col >= sc_col)
    {
	need_wait_return = TRUE;
	redraw_cmdline = TRUE;
    }
}