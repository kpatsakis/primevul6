msg_source(int attr)
{
    char_u	*p;
    static int	recursive = FALSE;

    // Bail out if something called here causes an error.
    if (recursive)
	return;
    recursive = TRUE;

    ++no_wait_return;
    p = get_emsg_source();
    if (p != NULL)
    {
	msg_attr((char *)p, attr);
	vim_free(p);
    }
    p = get_emsg_lnum();
    if (p != NULL)
    {
	msg_attr((char *)p, HL_ATTR(HLF_N));
	vim_free(p);
	last_sourcing_lnum = SOURCING_LNUM;  // only once for each line
    }

    // remember the last sourcing name printed, also when it's empty
    if (SOURCING_NAME == NULL || other_sourcing_name())
    {
	VIM_CLEAR(last_sourcing_name);
	if (SOURCING_NAME != NULL)
	    last_sourcing_name = vim_strsave(SOURCING_NAME);
    }
    --no_wait_return;

    recursive = FALSE;
}