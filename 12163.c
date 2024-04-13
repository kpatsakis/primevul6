vgr_process_args(
	exarg_T		*eap,
	vgr_args_T	*args)
{
    char_u	*p;

    vim_memset(args, 0, sizeof(*args));

    args->regmatch.regprog = NULL;
    args->qf_title = vim_strsave(qf_cmdtitle(*eap->cmdlinep));

    if (eap->addr_count > 0)
	args->tomatch = eap->line2;
    else
	args->tomatch = MAXLNUM;

    // Get the search pattern: either white-separated or enclosed in //
    p = skip_vimgrep_pat(eap->arg, &args->spat, &args->flags);
    if (p == NULL)
    {
	emsg(_(e_invalid_search_pattern_or_delimiter));
	return FAIL;
    }

    vgr_init_regmatch(&args->regmatch, args->spat);
    if (args->regmatch.regprog == NULL)
	return FAIL;

    p = skipwhite(p);
    if (*p == NUL)
    {
	emsg(_(e_file_name_missing_or_invalid_pattern));
	return FAIL;
    }

    // Parse the list of arguments, wildcards have already been expanded.
    if ((get_arglist_exp(p, &args->fcount, &args->fnames, TRUE) == FAIL) ||
	args->fcount == 0)
    {
	emsg(_(e_no_match));
	return FAIL;
    }

    return OK;
}