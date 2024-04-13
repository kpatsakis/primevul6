msg_check_screen(void)
{
    if (!full_screen || !screen_valid(FALSE))
	return FALSE;

    if (msg_row >= Rows)
	msg_row = Rows - 1;
    if (msg_col >= Columns)
	msg_col = Columns - 1;
    return TRUE;
}