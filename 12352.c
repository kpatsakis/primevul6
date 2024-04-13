n_swapchar(cmdarg_T *cap)
{
    long	n;
    pos_T	startpos;
    int		did_change = 0;
#ifdef FEAT_NETBEANS_INTG
    pos_T	pos;
    char_u	*ptr;
    int		count;
#endif

    if (checkclearopq(cap->oap))
	return;

    if (LINEEMPTY(curwin->w_cursor.lnum) && vim_strchr(p_ww, '~') == NULL)
    {
	clearopbeep(cap->oap);
	return;
    }

    prep_redo_cmd(cap);

    if (u_save_cursor() == FAIL)
	return;

    startpos = curwin->w_cursor;
#ifdef FEAT_NETBEANS_INTG
    pos = startpos;
#endif
    for (n = cap->count1; n > 0; --n)
    {
	did_change |= swapchar(cap->oap->op_type, &curwin->w_cursor);
	inc_cursor();
	if (gchar_cursor() == NUL)
	{
	    if (vim_strchr(p_ww, '~') != NULL
		    && curwin->w_cursor.lnum < curbuf->b_ml.ml_line_count)
	    {
#ifdef FEAT_NETBEANS_INTG
		if (netbeans_active())
		{
		    if (did_change)
		    {
			ptr = ml_get(pos.lnum);
			count = (int)STRLEN(ptr) - pos.col;
			netbeans_removed(curbuf, pos.lnum, pos.col,
								 (long)count);
			netbeans_inserted(curbuf, pos.lnum, pos.col,
							&ptr[pos.col], count);
		    }
		    pos.col = 0;
		    pos.lnum++;
		}
#endif
		++curwin->w_cursor.lnum;
		curwin->w_cursor.col = 0;
		if (n > 1)
		{
		    if (u_savesub(curwin->w_cursor.lnum) == FAIL)
			break;
		    u_clearline();
		}
	    }
	    else
		break;
	}
    }
#ifdef FEAT_NETBEANS_INTG
    if (did_change && netbeans_active())
    {
	ptr = ml_get(pos.lnum);
	count = curwin->w_cursor.col - pos.col;
	netbeans_removed(curbuf, pos.lnum, pos.col, (long)count);
	netbeans_inserted(curbuf, pos.lnum, pos.col, &ptr[pos.col], count);
    }
#endif


    check_cursor();
    curwin->w_set_curswant = TRUE;
    if (did_change)
    {
	changed_lines(startpos.lnum, startpos.col, curwin->w_cursor.lnum + 1,
									  0L);
	curbuf->b_op_start = startpos;
	curbuf->b_op_end = curwin->w_cursor;
	if (curbuf->b_op_end.col > 0)
	    --curbuf->b_op_end.col;
    }
}