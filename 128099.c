inc_msg_scrolled(void)
{
#ifdef FEAT_EVAL
    if (*get_vim_var_str(VV_SCROLLSTART) == NUL)
    {
	char_u	    *p = SOURCING_NAME;
	char_u	    *tofree = NULL;
	int	    len;

	// v:scrollstart is empty, set it to the script/function name and line
	// number
	if (p == NULL)
	    p = (char_u *)_("Unknown");
	else
	{
	    len = (int)STRLEN(p) + 40;
	    tofree = alloc(len);
	    if (tofree != NULL)
	    {
		vim_snprintf((char *)tofree, len, _("%s line %ld"),
						      p, (long)SOURCING_LNUM);
		p = tofree;
	    }
	}
	set_vim_var_string(VV_SCROLLSTART, p, -1);
	vim_free(tofree);
    }
#endif
    ++msg_scrolled;
    if (must_redraw < VALID)
	must_redraw = VALID;
}