msg_may_trunc(int force, char_u *s)
{
    int		n;
    int		room;

    // If something unexpected happened "room" may be negative, check for that
    // just in case.
    room = (int)(Rows - cmdline_row - 1) * Columns + sc_col - 1;
    if (room > 0 && (force || (shortmess(SHM_TRUNC) && !exmode_active))
	    && (n = (int)STRLEN(s) - room) > 0)
    {
	if (has_mbyte)
	{
	    int	size = vim_strsize(s);

	    // There may be room anyway when there are multibyte chars.
	    if (size <= room)
		return s;

	    for (n = 0; size >= room; )
	    {
		size -= (*mb_ptr2cells)(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    --n;
	}
	s += n;
	*s = '<';
    }
    return s;
}