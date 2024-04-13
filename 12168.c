unload_dummy_buffer(buf_T *buf, char_u *dirname_start)
{
    if (curbuf != buf)		// safety check
    {
	close_buffer(NULL, buf, DOBUF_UNLOAD, FALSE, TRUE);

	// When autocommands/'autochdir' option changed directory: go back.
	restore_start_dir(dirname_start);
    }
}