verbose_open(void)
{
    if (verbose_fd == NULL && !verbose_did_open)
    {
	// Only give the error message once.
	verbose_did_open = TRUE;

	verbose_fd = mch_fopen((char *)p_vfile, "a");
	if (verbose_fd == NULL)
	{
	    semsg(_(e_cant_open_file_str), p_vfile);
	    return FAIL;
	}
    }
    return OK;
}