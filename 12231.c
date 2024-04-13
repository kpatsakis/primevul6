qf_list(exarg_T *eap)
{
    qf_list_T	*qfl;
    qfline_T	*qfp;
    int		i;
    int		idx1 = 1;
    int		idx2 = -1;
    char_u	*arg = eap->arg;
    int		plus = FALSE;
    int		all = eap->forceit;	// if not :cl!, only show
					// recognised errors
    qf_info_T	*qi;

    if ((qi = qf_cmd_get_stack(eap, TRUE)) == NULL)
	return;

    if (qf_stack_empty(qi) || qf_list_empty(qf_get_curlist(qi)))
    {
	emsg(_(e_no_errors));
	return;
    }
    if (*arg == '+')
    {
	++arg;
	plus = TRUE;
    }
    if (!get_list_range(&arg, &idx1, &idx2) || *arg != NUL)
    {
	semsg(_(e_trailing_characters_str), arg);
	return;
    }
    qfl = qf_get_curlist(qi);
    if (plus)
    {
	i = qfl->qf_index;
	idx2 = i + idx1;
	idx1 = i;
    }
    else
    {
	i = qfl->qf_count;
	if (idx1 < 0)
	    idx1 = (-idx1 > i) ? 0 : idx1 + i + 1;
	if (idx2 < 0)
	    idx2 = (-idx2 > i) ? 0 : idx2 + i + 1;
    }

    // Shorten all the file names, so that it is easy to read
    shorten_fnames(FALSE);

    // Get the attributes for the different quickfix highlight items.  Note
    // that this depends on syntax items defined in the qf.vim syntax file
    qfFileAttr = syn_name2attr((char_u *)"qfFileName");
    if (qfFileAttr == 0)
	qfFileAttr = HL_ATTR(HLF_D);
    qfSepAttr = syn_name2attr((char_u *)"qfSeparator");
    if (qfSepAttr == 0)
	qfSepAttr = HL_ATTR(HLF_D);
    qfLineAttr = syn_name2attr((char_u *)"qfLineNr");
    if (qfLineAttr == 0)
	qfLineAttr = HL_ATTR(HLF_N);

    if (qfl->qf_nonevalid)
	all = TRUE;
    FOR_ALL_QFL_ITEMS(qfl, qfp, i)
    {
	if ((qfp->qf_valid || all) && idx1 <= i && i <= idx2)
	    qf_list_entry(qfp, i, i == qfl->qf_index);

	ui_breakcheck();
    }
}