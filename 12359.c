nv_bracket_block(cmdarg_T *cap, pos_T *old_pos)
{
    pos_T	new_pos = {0, 0, 0};
    pos_T	*pos = NULL;	    // init for GCC
    pos_T	prev_pos;
    long	n;
    int		findc;
    int		c;

    if (cap->nchar == '*')
	cap->nchar = '/';
    prev_pos.lnum = 0;
    if (cap->nchar == 'm' || cap->nchar == 'M')
    {
	if (cap->cmdchar == '[')
	    findc = '{';
	else
	    findc = '}';
	n = 9999;
    }
    else
    {
	findc = cap->nchar;
	n = cap->count1;
    }
    for ( ; n > 0; --n)
    {
	if ((pos = findmatchlimit(cap->oap, findc,
			(cap->cmdchar == '[') ? FM_BACKWARD : FM_FORWARD, 0)) == NULL)
	{
	    if (new_pos.lnum == 0)	// nothing found
	    {
		if (cap->nchar != 'm' && cap->nchar != 'M')
		    clearopbeep(cap->oap);
	    }
	    else
		pos = &new_pos;	// use last one found
	    break;
	}
	prev_pos = new_pos;
	curwin->w_cursor = *pos;
	new_pos = *pos;
    }
    curwin->w_cursor = *old_pos;

    // Handle "[m", "]m", "[M" and "[M".  The findmatchlimit() only
    // brought us to the match for "[m" and "]M" when inside a method.
    // Try finding the '{' or '}' we want to be at.
    // Also repeat for the given count.
    if (cap->nchar == 'm' || cap->nchar == 'M')
    {
	// norm is TRUE for "]M" and "[m"
	int	    norm = ((findc == '{') == (cap->nchar == 'm'));

	n = cap->count1;
	// found a match: we were inside a method
	if (prev_pos.lnum != 0)
	{
	    pos = &prev_pos;
	    curwin->w_cursor = prev_pos;
	    if (norm)
		--n;
	}
	else
	    pos = NULL;
	while (n > 0)
	{
	    for (;;)
	    {
		if ((findc == '{' ? dec_cursor() : inc_cursor()) < 0)
		{
		    // if not found anything, that's an error
		    if (pos == NULL)
			clearopbeep(cap->oap);
		    n = 0;
		    break;
		}
		c = gchar_cursor();
		if (c == '{' || c == '}')
		{
		    // Must have found end/start of class: use it.
		    // Or found the place to be at.
		    if ((c == findc && norm) || (n == 1 && !norm))
		    {
			new_pos = curwin->w_cursor;
			pos = &new_pos;
			n = 0;
		    }
		    // if no match found at all, we started outside of the
		    // class and we're inside now.  Just go on.
		    else if (new_pos.lnum == 0)
		    {
			new_pos = curwin->w_cursor;
			pos = &new_pos;
		    }
		    // found start/end of other method: go to match
		    else if ((pos = findmatchlimit(cap->oap, findc,
			      (cap->cmdchar == '[') ? FM_BACKWARD : FM_FORWARD,
								   0)) == NULL)
			n = 0;
		    else
			curwin->w_cursor = *pos;
		    break;
		}
	    }
	    --n;
	}
	curwin->w_cursor = *old_pos;
	if (pos == NULL && new_pos.lnum != 0)
	    clearopbeep(cap->oap);
    }
    if (pos != NULL)
    {
	setpcmark();
	curwin->w_cursor = *pos;
	curwin->w_set_curswant = TRUE;
#ifdef FEAT_FOLDING
	if ((fdo_flags & FDO_BLOCK) && KeyTyped
		&& cap->oap->op_type == OP_NOP)
	    foldOpenCursor();
#endif
    }
}