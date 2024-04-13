parse_efm_option(char_u *efm)
{
    efm_T	*fmt_ptr = NULL;
    efm_T	*fmt_first = NULL;
    efm_T	*fmt_last = NULL;
    char_u	*fmtstr = NULL;
    int		len;
    int		sz;

    // Each part of the format string is copied and modified from errorformat
    // to regex prog.  Only a few % characters are allowed.

    // Get some space to modify the format string into.
    sz = efm_regpat_bufsz(efm);
    if ((fmtstr = alloc_id(sz, aid_qf_efm_fmtstr)) == NULL)
	goto parse_efm_error;

    while (efm[0] != NUL)
    {
	// Allocate a new eformat structure and put it at the end of the list
	fmt_ptr = ALLOC_CLEAR_ONE_ID(efm_T, aid_qf_efm_fmtpart);
	if (fmt_ptr == NULL)
	    goto parse_efm_error;
	if (fmt_first == NULL)	    // first one
	    fmt_first = fmt_ptr;
	else
	    fmt_last->next = fmt_ptr;
	fmt_last = fmt_ptr;

	// Isolate one part in the 'errorformat' option
	len = efm_option_part_len(efm);

	if (efm_to_regpat(efm, len, fmt_ptr, fmtstr) == FAIL)
	    goto parse_efm_error;
	if ((fmt_ptr->prog = vim_regcomp(fmtstr, RE_MAGIC + RE_STRING)) == NULL)
	    goto parse_efm_error;
	// Advance to next part
	efm = skip_to_option_part(efm + len);	// skip comma and spaces
    }

    if (fmt_first == NULL)	// nothing found
	emsg(_(e_errorformat_contains_no_pattern));

    goto parse_efm_end;

parse_efm_error:
    free_efm_list(&fmt_first);

parse_efm_end:
    vim_free(fmtstr);

    return fmt_first;
}