v_swap_corners(int cmdchar)
{
    pos_T	old_cursor;
    colnr_T	left, right;

    if (cmdchar == 'O' && VIsual_mode == Ctrl_V)
    {
	old_cursor = curwin->w_cursor;
	getvcols(curwin, &old_cursor, &VIsual, &left, &right);
	curwin->w_cursor.lnum = VIsual.lnum;
	coladvance(left);
	VIsual = curwin->w_cursor;

	curwin->w_cursor.lnum = old_cursor.lnum;
	curwin->w_curswant = right;
	// 'selection "exclusive" and cursor at right-bottom corner: move it
	// right one column
	if (old_cursor.lnum >= VIsual.lnum && *p_sel == 'e')
	    ++curwin->w_curswant;
	coladvance(curwin->w_curswant);
	if (curwin->w_cursor.col == old_cursor.col
		&& (!virtual_active()
		    || curwin->w_cursor.coladd == old_cursor.coladd))
	{
	    curwin->w_cursor.lnum = VIsual.lnum;
	    if (old_cursor.lnum <= VIsual.lnum && *p_sel == 'e')
		++right;
	    coladvance(right);
	    VIsual = curwin->w_cursor;

	    curwin->w_cursor.lnum = old_cursor.lnum;
	    coladvance(left);
	    curwin->w_curswant = left;
	}
    }
    else
    {
	old_cursor = curwin->w_cursor;
	curwin->w_cursor = VIsual;
	VIsual = old_cursor;
	curwin->w_set_curswant = TRUE;
    }
}