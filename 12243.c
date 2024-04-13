trigger_cexpr_autocmd(int cmdidx)
{
    char_u	*au_name = cexpr_get_auname(cmdidx);

    if (au_name != NULL && apply_autocmds(EVENT_QUICKFIXCMDPRE, au_name,
					       curbuf->b_fname, TRUE, curbuf))
    {
	if (aborting())
	    return FAIL;
    }
    return OK;
}