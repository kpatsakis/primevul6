load_dummy_buffer(
    char_u	*fname,
    char_u	*dirname_start,  // in: old directory
    char_u	*resulting_dir)  // out: new directory
{
    buf_T	*newbuf;
    bufref_T	newbufref;
    bufref_T	newbuf_to_wipe;
    int		failed = TRUE;
    aco_save_T	aco;
    int		readfile_result;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(NULL, NULL, (linenr_T)1, BLN_DUMMY);
    if (newbuf == NULL)
	return NULL;
    set_bufref(&newbufref, newbuf);

    // Init the options.
    buf_copy_options(newbuf, BCO_ENTER | BCO_NOHELP);

    // need to open the memfile before putting the buffer in a window
    if (ml_open(newbuf) == OK)
    {
	// Make sure this buffer isn't wiped out by autocommands.
	++newbuf->b_locked;

	// set curwin/curbuf to buf and save a few things
	aucmd_prepbuf(&aco, newbuf);

	// Need to set the filename for autocommands.
	(void)setfname(curbuf, fname, NULL, FALSE);

	// Create swap file now to avoid the ATTENTION message.
	check_need_swap(TRUE);

	// Remove the "dummy" flag, otherwise autocommands may not
	// work.
	curbuf->b_flags &= ~BF_DUMMY;

	newbuf_to_wipe.br_buf = NULL;
	readfile_result = readfile(fname, NULL,
		    (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
		    NULL, READ_NEW | READ_DUMMY);
	--newbuf->b_locked;
	if (readfile_result == OK
		&& !got_int
		&& !(curbuf->b_flags & BF_NEW))
	{
	    failed = FALSE;
	    if (curbuf != newbuf)
	    {
		// Bloody autocommands changed the buffer!  Can happen when
		// using netrw and editing a remote file.  Use the current
		// buffer instead, delete the dummy one after restoring the
		// window stuff.
		set_bufref(&newbuf_to_wipe, newbuf);
		newbuf = curbuf;
	    }
	}

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
	if (newbuf_to_wipe.br_buf != NULL && bufref_valid(&newbuf_to_wipe))
	    wipe_buffer(newbuf_to_wipe.br_buf, FALSE);

	// Add back the "dummy" flag, otherwise buflist_findname_stat() won't
	// skip it.
	newbuf->b_flags |= BF_DUMMY;
    }

    // When autocommands/'autochdir' option changed directory: go back.
    // Let the caller know what the resulting dir was first, in case it is
    // important.
    mch_dirname(resulting_dir, MAXPATHL);
    restore_start_dir(dirname_start);

    if (!bufref_valid(&newbufref))
	return NULL;
    if (failed)
    {
	wipe_dummy_buffer(newbuf, dirname_start);
	return NULL;
    }
    return newbuf;
}