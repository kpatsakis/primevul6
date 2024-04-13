ex_cfile(exarg_T *eap)
{
    char_u	*enc = NULL;
    win_T	*wp = NULL;
    qf_info_T	*qi = &ql_info;
    char_u	*au_name = NULL;
    int_u	save_qfid = 0;		// init for gcc
    int		res;

    au_name = cfile_get_auname(eap->cmdidx);
    if (au_name != NULL && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
							NULL, FALSE, curbuf))
    {
#ifdef FEAT_EVAL
	if (aborting())
	    return;
#endif
    }

    enc = (*curbuf->b_p_menc != NUL) ? curbuf->b_p_menc : p_menc;
#ifdef FEAT_BROWSE
    if (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *browse_file = do_browse(0, (char_u *)_("Error file"), eap->arg,
				   NULL, NULL,
				   (char_u *)_(BROWSE_FILTER_ALL_FILES), NULL);
	if (browse_file == NULL)
	    return;
	set_string_option_direct((char_u *)"ef", -1, browse_file, OPT_FREE, 0);
	vim_free(browse_file);
    }
    else
#endif
    if (*eap->arg != NUL)
	set_string_option_direct((char_u *)"ef", -1, eap->arg, OPT_FREE, 0);

    if (is_loclist_cmd(eap->cmdidx))
	wp = curwin;

    incr_quickfix_busy();

    // This function is used by the :cfile, :cgetfile and :caddfile
    // commands.
    // :cfile always creates a new quickfix list and jumps to the
    // first error.
    // :cgetfile creates a new quickfix list but doesn't jump to the
    // first error.
    // :caddfile adds to an existing quickfix list. If there is no
    // quickfix list then a new list is created.
    res = qf_init(wp, p_ef, p_efm, (eap->cmdidx != CMD_caddfile
			&& eap->cmdidx != CMD_laddfile),
			qf_cmdtitle(*eap->cmdlinep), enc);
    if (wp != NULL)
    {
	qi = GET_LOC_LIST(wp);
	if (qi == NULL)
	{
	    decr_quickfix_busy();
	    return;
	}
    }
    if (res >= 0)
	qf_list_changed(qf_get_curlist(qi));
    save_qfid = qf_get_curlist(qi)->qf_id;
    if (au_name != NULL)
	apply_autocmds(EVENT_QUICKFIXCMDPOST, au_name, NULL, FALSE, curbuf);

    // Jump to the first error for a new list and if autocmds didn't
    // free the list.
    if (res > 0 && (eap->cmdidx == CMD_cfile || eap->cmdidx == CMD_lfile)
	    && qflist_valid(wp, save_qfid))
	// display the first error
	qf_jump_first(qi, save_qfid, eap->forceit);

    decr_quickfix_busy();
}