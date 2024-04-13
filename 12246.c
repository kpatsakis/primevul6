ex_helpgrep(exarg_T *eap)
{
    regmatch_T	regmatch;
    char_u	*save_cpo;
    int		save_cpo_allocated;
    qf_info_T	*qi = &ql_info;
    int		new_qi = FALSE;
    char_u	*au_name =  NULL;
    char_u	*lang = NULL;
    int		updated = FALSE;

    switch (eap->cmdidx)
    {
	case CMD_helpgrep:  au_name = (char_u *)"helpgrep"; break;
	case CMD_lhelpgrep: au_name = (char_u *)"lhelpgrep"; break;
	default: break;
    }
    if (au_name != NULL && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
#ifdef FEAT_EVAL
	if (aborting())
	    return;
#endif
    }

    if (is_loclist_cmd(eap->cmdidx))
    {
	qi = hgr_get_ll(&new_qi);
	if (qi == NULL)
	    return;
    }

    // Make 'cpoptions' empty, the 'l' flag should not be used here.
    save_cpo = p_cpo;
    save_cpo_allocated = is_option_allocated("cpo");
    p_cpo = empty_option;

    incr_quickfix_busy();

#ifdef FEAT_MULTI_LANG
    // Check for a specified language
    lang = check_help_lang(eap->arg);
#endif
    regmatch.regprog = vim_regcomp(eap->arg, RE_MAGIC + RE_STRING);
    regmatch.rm_ic = FALSE;
    if (regmatch.regprog != NULL)
    {
	qf_list_T	*qfl;

	// create a new quickfix list
	qf_new_list(qi, qf_cmdtitle(*eap->cmdlinep));
	qfl = qf_get_curlist(qi);

	hgr_search_in_rtp(qfl, &regmatch, lang);

	vim_regfree(regmatch.regprog);

	qfl->qf_nonevalid = FALSE;
	qfl->qf_ptr = qfl->qf_start;
	qfl->qf_index = 1;
	qf_list_changed(qfl);
	updated = TRUE;
    }

    if (p_cpo == empty_option)
	p_cpo = save_cpo;
    else
    {
	// Darn, some plugin changed the value.  If it's still empty it was
	// changed and restored, need to restore in the complicated way.
	if (*p_cpo == NUL)
	    set_option_value_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	if (save_cpo_allocated)
	    free_string_option(save_cpo);
    }

    if (updated)
	// This may open a window and source scripts, do this after 'cpo' was
	// restored.
	qf_update_buffer(qi, NULL);

    if (au_name != NULL)
    {
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
	// When adding a location list to an existing location list stack,
	// if the autocmd made the stack invalid, then just return.
	if (!new_qi && IS_LL_STACK(qi) && qf_find_win_with_loclist(qi) == NULL)
	{
	    decr_quickfix_busy();
	    return;
	}
    }

    // Jump to first match.
    if (!qf_list_empty(qf_get_curlist(qi)))
	qf_jump(qi, 0, 0, FALSE);
    else
	semsg(_(e_no_match_str_2), eap->arg);

    decr_quickfix_busy();

    if (eap->cmdidx == CMD_lhelpgrep)
    {
	// If the help window is not opened or if it already points to the
	// correct location list, then free the new location list.
	if (!bt_help(curwin->w_buffer) || curwin->w_llist == qi)
	{
	    if (new_qi)
		ll_free_all(&qi);
	}
	else if (curwin->w_llist == NULL && new_qi)
	    // current window didn't have a location list associated with it
	    // before. Associate the new location list now.
	    curwin->w_llist = qi;
    }
}