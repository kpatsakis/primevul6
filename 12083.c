ex_make(exarg_T *eap)
{
    char_u	*fname;
    char_u	*cmd;
    char_u	*enc = NULL;
    win_T	*wp = NULL;
    qf_info_T	*qi = &ql_info;
    int		res;
    char_u	*au_name = NULL;
    int_u	save_qfid;
    char_u	*errorformat = p_efm;
    int		newlist = TRUE;

    // Redirect ":grep" to ":vimgrep" if 'grepprg' is "internal".
    if (grep_internal(eap->cmdidx))
    {
	ex_vimgrep(eap);
	return;
    }

    au_name = make_get_auname(eap->cmdidx);
    if (au_name != NULL && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
#ifdef FEAT_EVAL
	if (aborting())
	    return;
#endif
    }
    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;

    if (is_loclist_cmd(eap->cmdidx))
	wp = curwin;

    autowrite_all();
    fname = get_mef_name();
    if (fname == NULL)
	return;
    mch_remove(fname);	    // in case it's not unique

    cmd = make_get_fullcmd(eap->arg, fname);
    if (cmd == NULL)
    {
	vim_free(fname);
	return;
    }

    // let the shell know if we are redirecting output or not
    do_shell(cmd, *p_sp != NUL ? SHELL_DOOUT : 0);

#ifdef AMIGA
    out_flush();
		// read window status report and redraw before message
    (void)char_avail();
#endif

    incr_quickfix_busy();

    if (eap->cmdidx != CMD_make && eap->cmdidx != CMD_lmake)
	errorformat = p_gefm;
    if (eap->cmdidx == CMD_grepadd || eap->cmdidx == CMD_lgrepadd)
	newlist = FALSE;

    res = qf_init(wp, fname, errorformat, newlist, qf_cmdtitle(*eap->cmdlinep),
									enc);
    if (wp != NULL)
    {
	qi = GET_LOC_LIST(wp);
	if (qi == NULL)
	    goto cleanup;
    }
    if (res >= 0)
	qf_list_changed(qf_get_curlist(qi));

    // Remember the current quickfix list identifier, so that we can
    // check for autocommands changing the current quickfix list.
    save_qfid = qf_get_curlist(qi)->qf_id;
    if (au_name != NULL)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name,
					       curbuf->b_fname, TRUE, curbuf);
    if (res > 0 && !eap->forceit && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_first(qi, save_qfid, FALSE);

cleanup:
    decr_quickfix_busy();
    mch_remove(fname);
    vim_free(fname);
    vim_free(cmd);
}