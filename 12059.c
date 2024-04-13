qf_jump_to_buffer(
	qf_info_T	*qi,
	int		qf_index,
	qfline_T	*qf_ptr,
	int		forceit,
	int		prev_winid,
	int		*opened_window,
	int		openfold,
	int		print_message)
{
    buf_T	*old_curbuf;
    linenr_T	old_lnum;
    int		retval = OK;

    // If there is a file name, read the wanted file if needed, and check
    // autowrite etc.
    old_curbuf = curbuf;
    old_lnum = curwin->w_cursor.lnum;

    if (qf_ptr->qf_fnum != 0)
    {
	retval = qf_jump_edit_buffer(qi, qf_ptr, forceit, prev_winid,
						opened_window);
	if (retval != OK)
	    return retval;
    }

    // When not switched to another buffer, still need to set pc mark
    if (curbuf == old_curbuf)
	setpcmark();

    qf_jump_goto_line(qf_ptr->qf_lnum, qf_ptr->qf_col, qf_ptr->qf_viscol,
	    qf_ptr->qf_pattern);

#ifdef FEAT_FOLDING
    if ((fdo_flags & FDO_QUICKFIX) && openfold)
	foldOpenCursor();
#endif
    if (print_message)
	qf_jump_print_msg(qi, qf_index, qf_ptr, old_curbuf, old_lnum);

    return retval;
}