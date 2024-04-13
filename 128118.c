clear_sb_text(int all)
{
    msgchunk_T	*mp;
    msgchunk_T	**lastp;

    if (all)
	lastp = &last_msgchunk;
    else
    {
	if (last_msgchunk == NULL)
	    return;
	lastp = &last_msgchunk->sb_prev;
    }

    while (*lastp != NULL)
    {
	mp = (*lastp)->sb_prev;
	vim_free(*lastp);
	*lastp = mp;
    }
}