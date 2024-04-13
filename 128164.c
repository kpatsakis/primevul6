msg_strtrunc(
    char_u	*s,
    int		force)	    // always truncate
{
    char_u	*buf = NULL;
    int		len;
    int		room;

    // May truncate message to avoid a hit-return prompt
    if ((!msg_scroll && !need_wait_return && shortmess(SHM_TRUNCALL)
			       && !exmode_active && msg_silent == 0) || force)
    {
	len = vim_strsize(s);
	if (msg_scrolled != 0)
	    // Use all the columns.
	    room = (int)(Rows - msg_row) * Columns - 1;
	else
	    // Use up to 'showcmd' column.
	    room = (int)(Rows - msg_row - 1) * Columns + sc_col - 1;
	if (len > room && room > 0)
	{
	    if (enc_utf8)
		// may have up to 18 bytes per cell (6 per char, up to two
		// composing chars)
		len = (room + 2) * 18;
	    else if (enc_dbcs == DBCS_JPNU)
		// may have up to 2 bytes per cell for euc-jp
		len = (room + 2) * 2;
	    else
		len = room + 2;
	    buf = alloc(len);
	    if (buf != NULL)
		trunc_string(s, buf, room, len);
	}
    }
    return buf;
}