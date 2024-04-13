ex_cbelow(exarg_T *eap)
{
    qf_info_T	*qi;
    qf_list_T	*qfl;
    int		dir;
    int		buf_has_flag;
    int		errornr = 0;
    pos_T	pos;

    if (eap->addr_count > 0 && eap->line2 <= 0)
    {
	emsg(_(e_invalid_range));
	return;
    }

    // Check whether the current buffer has any quickfix entries
    if (eap->cmdidx == CMD_cabove || eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_cbefore || eap->cmdidx == CMD_cafter)
	buf_has_flag = BUF_HAS_QF_ENTRY;
    else
	buf_has_flag = BUF_HAS_LL_ENTRY;
    if (!(curbuf->b_has_qf_entry & buf_has_flag))
    {
	emsg(_(e_no_errors));
	return;
    }

    if ((qi = qf_cmd_get_stack(eap, TRUE)) == NULL)
	return;

    qfl = qf_get_curlist(qi);
    // check if the list has valid errors
    if (!qf_list_has_valid_entries(qfl))
    {
	emsg(_(e_no_errors));
	return;
    }

    if (eap->cmdidx == CMD_cbelow
	    || eap->cmdidx == CMD_lbelow
	    || eap->cmdidx == CMD_cafter
	    || eap->cmdidx == CMD_lafter)
	// Forward motion commands
	dir = FORWARD;
    else
	dir = BACKWARD;

    pos = curwin->w_cursor;
    // A quickfix entry column number is 1 based whereas cursor column
    // number is 0 based. Adjust the column number.
    pos.col++;
    errornr = qf_find_nth_adj_entry(qfl, curbuf->b_fnum, &pos,
				eap->addr_count > 0 ? eap->line2 : 0, dir,
				eap->cmdidx == CMD_cbelow
					|| eap->cmdidx == CMD_lbelow
					|| eap->cmdidx == CMD_cabove
					|| eap->cmdidx == CMD_labove);

    if (errornr > 0)
	qf_jump(qi, 0, errornr, FALSE);
    else
	emsg(_(e_no_more_items));
}