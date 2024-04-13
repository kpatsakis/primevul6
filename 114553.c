fix_current_dir(void)
{
#ifdef FEAT_AUTOCHDIR
    if (p_acd)
	do_autochdir();
    else
#endif
    if (curwin->w_localdir != NULL || curtab->tp_localdir != NULL)
    {
	char_u	*dirname;

	// Window or tab has a local directory: Save current directory as
	// global directory (unless that was done already) and change to the
	// local directory.
	if (globaldir == NULL)
	{
	    char_u	cwd[MAXPATHL];

	    if (mch_dirname(cwd, MAXPATHL) == OK)
		globaldir = vim_strsave(cwd);
	}
	if (curwin->w_localdir != NULL)
	    dirname = curwin->w_localdir;
	else
	    dirname = curtab->tp_localdir;

	if (mch_chdir((char *)dirname) == 0)
	{
	    last_chdir_reason = NULL;
	    shorten_fnames(TRUE);
	}
    }
    else if (globaldir != NULL)
    {
	// Window doesn't have a local directory and we are not in the global
	// directory: Change to the global directory.
	vim_ignored = mch_chdir((char *)globaldir);
	VIM_CLEAR(globaldir);
	last_chdir_reason = NULL;
	shorten_fnames(TRUE);
    }
}