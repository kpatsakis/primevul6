disp_sb_line(int row, msgchunk_T *smp)
{
    msgchunk_T	*mp = smp;
    char_u	*p;

    for (;;)
    {
	msg_row = row;
	msg_col = mp->sb_msg_col;
	p = mp->sb_text;
	if (*p == '\n')	    // don't display the line break
	    ++p;
	msg_puts_display(p, -1, mp->sb_attr, TRUE);
	if (mp->sb_eol || mp->sb_next == NULL)
	    break;
	mp = mp->sb_next;
    }
    return mp->sb_next;
}