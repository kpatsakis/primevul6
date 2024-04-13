nv_gi_cmd(cmdarg_T *cap)
{
    int		i;

    if (curbuf->b_last_insert.lnum != 0)
    {
	curwin->w_cursor = curbuf->b_last_insert;
	check_cursor_lnum();
	i = (int)STRLEN(ml_get_curline());
	if (curwin->w_cursor.col > (colnr_T)i)
	{
	    if (virtual_active())
		curwin->w_cursor.coladd += curwin->w_cursor.col - i;
	    curwin->w_cursor.col = i;
	}
    }
    cap->cmdchar = 'i';
    nv_edit(cap);
}