qf_view_result(int split)
{
    qf_info_T   *qi = &ql_info;

    if (IS_LL_WINDOW(curwin))
	qi = GET_LOC_LIST(curwin);

    if (qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	return;
    }

    if (split)
    {
	// Open the selected entry in a new window
	qf_jump_newwin(qi, 0, (long)curwin->w_cursor.lnum, FALSE, TRUE);
	do_cmdline_cmd((char_u *) "clearjumps");
	return;
    }

    do_cmdline_cmd((char_u *)(IS_LL_WINDOW(curwin) ? ".ll" : ".cc"));
}