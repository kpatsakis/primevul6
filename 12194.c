get_mef_name(void)
{
    char_u	*p;
    char_u	*name;
    static int	start = -1;
    static int	off = 0;
#ifdef HAVE_LSTAT
    stat_T	sb;
#endif

    if (*p_mef == NUL)
    {
	name = vim_tempname('e', FALSE);
	if (name == NULL)
	    emsg(_(e_cant_get_temp_file_name));
	return name;
    }

    for (p = p_mef; *p; ++p)
	if (p[0] == '#' && p[1] == '#')
	    break;

    if (*p == NUL)
	return vim_strsave(p_mef);

    // Keep trying until the name doesn't exist yet.
    for (;;)
    {
	if (start == -1)
	    start = mch_get_pid();
	else
	    off += 19;

	name = alloc_id(STRLEN(p_mef) + 30, aid_qf_mef_name);
	if (name == NULL)
	    break;
	STRCPY(name, p_mef);
	sprintf((char *)name + (p - p_mef), "%d%d", start, off);
	STRCAT(name, p + 2);
	if (mch_getperm(name) < 0
#ifdef HAVE_LSTAT
		    // Don't accept a symbolic link, it's a security risk.
		    && mch_lstat((char *)name, &sb) < 0
#endif
		)
	    break;
	vim_free(name);
    }
    return name;
}