ex_cclose(exarg_T *eap)
{
    win_T	*win = NULL;
    qf_info_T	*qi;

    if ((qi = qf_cmd_get_stack(eap, FALSE)) == NULL)
	return;

    // Find existing quickfix window and close it.
    win = qf_find_win(qi);
    if (win != NULL)
	win_close(win, FALSE);
}