wipe_dummy_buffer(buf_T *buf, char_u *dirname_start)
{
    // If any autocommand opened a window on the dummy buffer, close that
    // window.  If we can't close them all then give up.
    while (buf->b_nwindows > 0)
    {
	int	    did_one = FALSE;
	win_T	    *wp;

	if (firstwin->w_next != NULL)
	    FOR_ALL_WINDOWS(wp)
		if (wp->w_buffer == buf)
		{
		    if (win_close(wp, FALSE) == OK)
			did_one = TRUE;
		    break;
		}
	if (!did_one)
	    return;
    }

    if (curbuf != buf && buf->b_nwindows == 0)	// safety check
    {
#if defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that aborting()
	// returns FALSE when wiping out the buffer.  Otherwise it doesn't
	// work when got_int is set.
	enter_cleanup(&cs);
#endif

	wipe_buffer(buf, TRUE);

#if defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#endif
	// When autocommands/'autochdir' option changed directory: go back.
	restore_start_dir(dirname_start);
    }
}