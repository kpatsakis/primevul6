t_puts(
    int		*t_col,
    char_u	*t_s,
    char_u	*s,
    int		attr)
{
    // output postponed text
    msg_didout = TRUE;		// remember that line is not empty
    screen_puts_len(t_s, (int)(s - t_s), msg_row, msg_col, attr);
    msg_col += *t_col;
    *t_col = 0;
    // If the string starts with a composing character don't increment the
    // column position for it.
    if (enc_utf8 && utf_iscomposing(utf_ptr2char(t_s)))
	--msg_col;
    if (msg_col >= Columns)
    {
	msg_col = 0;
	++msg_row;
    }
}