ex_cwindow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    win_T	*win;

    if ((qi = qf_cmd_get_stack(eap, TRUE)) == NULL)
	return;

    qfl = qf_get_curlist(qi);

    // Look for an existing quickfix window.
    win = qf_find_win(qi);

    // If a quickfix window is open but we have no errors to display,
    // close the window.  If a quickfix window is not open, then open
    // it if we have errors; otherwise, leave it closed.
    if (qf_stack_empty(qi)
	    || qfl->qf_nonevalid
	    || qf_list_empty(qfl))
    {
	if (win != NULL)
	    ex_cclose(eap);
    }
    else if (win == NULL)
	ex_copen(eap);
}