ex_copen(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    int		height;
    int		status = FAIL;
    int		lnum;

    if ((qi = qf_cmd_get_stack(eap, TRUE)) == NULL)
	return;

    incr_quickfix_busy();

    if (eap->addr_count != 0)
	height = eap->line2;
    else
	height = QF_WINHEIGHT;

    reset_VIsual_and_resel();			// stop Visual mode
#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    // Find an existing quickfix window, or open a new one.
    if (cmdmod.cmod_tab == 0)
	status = qf_goto_cwindow(qi, eap->addr_count != 0, height,
						cmdmod.cmod_split & WSP_VERT);
    if (status == FAIL)
	if (qf_open_new_cwindow(qi, height) == FAIL)
	{
	    decr_quickfix_busy();
	    return;
	}

    qfl = qf_get_curlist(qi);
    qf_set_title_var(qfl);
    // Save the current index here, as updating the quickfix buffer may free
    // the quickfix list
    lnum = qfl->qf_index;

    // Fill the buffer with the quickfix list.
    qf_fill_buffer(qfl, curbuf, NULL, curwin->w_id);

    decr_quickfix_busy();

    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col = 0;
    check_cursor();
    update_topline();		// scroll to show the line
}