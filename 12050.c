qf_setup_state(
	qfstate_T	*pstate,
	char_u		*enc,
	char_u		*efile,
	typval_T	*tv,
	buf_T		*buf,
	linenr_T	lnumfirst,
	linenr_T	lnumlast)
{
    pstate->vc.vc_type = CONV_NONE;
    if (enc != NULL && *enc != NUL)
	convert_setup(&pstate->vc, enc, p_enc);

    if (efile != NULL && (pstate->fd = mch_fopen((char *)efile, "r")) == NULL)
    {
	semsg(_(e_cant_open_errorfile_str), efile);
	return FAIL;
    }

    if (tv != NULL)
    {
	if (tv->v_type == VAR_STRING)
	    pstate->p_str = tv->vval.v_string;
	else if (tv->v_type == VAR_LIST)
	    pstate->p_li = tv->vval.v_list->lv_first;
	pstate->tv = tv;
    }
    pstate->buf = buf;
    pstate->buflnum = lnumfirst;
    pstate->lnumlast = lnumlast;

    return OK;
}