store_sb_text(
    char_u	**sb_str,	// start of string
    char_u	*s,		// just after string
    int		attr,
    int		*sb_col,
    int		finish)		// line ends
{
    msgchunk_T	*mp;

    if (do_clear_sb_text == SB_CLEAR_ALL
	    || do_clear_sb_text == SB_CLEAR_CMDLINE_DONE)
    {
	clear_sb_text(do_clear_sb_text == SB_CLEAR_ALL);
	do_clear_sb_text = SB_CLEAR_NONE;
    }

    if (s > *sb_str)
    {
	mp = alloc(sizeof(msgchunk_T) + (s - *sb_str));
	if (mp != NULL)
	{
	    mp->sb_eol = finish;
	    mp->sb_msg_col = *sb_col;
	    mp->sb_attr = attr;
	    vim_strncpy(mp->sb_text, *sb_str, s - *sb_str);

	    if (last_msgchunk == NULL)
	    {
		last_msgchunk = mp;
		mp->sb_prev = NULL;
	    }
	    else
	    {
		mp->sb_prev = last_msgchunk;
		last_msgchunk->sb_next = mp;
		last_msgchunk = mp;
	    }
	    mp->sb_next = NULL;
	}
    }
    else if (finish && last_msgchunk != NULL)
	last_msgchunk->sb_eol = TRUE;

    *sb_str = s;
    *sb_col = 0;
}