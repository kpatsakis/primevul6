qf_get_fnum(qf_list_T *qfl, char_u *directory, char_u *fname)
{
    char_u	*ptr = NULL;
    buf_T	*buf;
    char_u	*bufname;

    if (fname == NULL || *fname == NUL)		// no file name
	return 0;

#ifdef VMS
    vms_remove_version(fname);
#endif
#ifdef BACKSLASH_IN_FILENAME
    if (directory != NULL)
	slash_adjust(directory);
    slash_adjust(fname);
#endif
    if (directory != NULL && !vim_isAbsName(fname)
	    && (ptr = concat_fnames(directory, fname, TRUE)) != NULL)
    {
	// Here we check if the file really exists.
	// This should normally be true, but if make works without
	// "leaving directory"-messages we might have missed a
	// directory change.
	if (mch_getperm(ptr) < 0)
	{
	    vim_free(ptr);
	    directory = qf_guess_filepath(qfl, fname);
	    if (directory)
		ptr = concat_fnames(directory, fname, TRUE);
	    else
		ptr = vim_strsave(fname);
	}
	// Use concatenated directory name and file name
	bufname = ptr;
    }
    else
	bufname = fname;

    if (qf_last_bufname != NULL && STRCMP(bufname, qf_last_bufname) == 0
	    && bufref_valid(&qf_last_bufref))
    {
	buf = qf_last_bufref.br_buf;
	vim_free(ptr);
    }
    else
    {
	vim_free(qf_last_bufname);
	buf = buflist_new(bufname, NULL, (linenr_T)0, BLN_NOOPT);
	if (bufname == ptr)
	    qf_last_bufname = bufname;
	else
	    qf_last_bufname = vim_strsave(bufname);
	set_bufref(&qf_last_bufref, buf);
    }
    if (buf == NULL)
	return 0;

    buf->b_has_qf_entry =
			IS_QF_LIST(qfl) ? BUF_HAS_QF_ENTRY : BUF_HAS_LL_ENTRY;
    return buf->b_fnum;
}