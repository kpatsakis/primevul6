static bool open_tmpoutbuf(rzip_control *control)
{
	i64 maxlen = control->maxram;
	void *buf;

	while (42) {
		round_to_page(&maxlen);
		buf = malloc(maxlen);
		if (buf) {
			print_maxverbose("Malloced %"PRId64" for tmp_outbuf\n", maxlen);
			break;
		}
		maxlen = maxlen / 3 * 2;
		if (maxlen < 100000000)
			fatal_return(("Unable to even malloc 100MB for tmp_outbuf\n"), false);
	}
	control->flags |= FLAG_TMP_OUTBUF;
	/* Allocate slightly more so we can cope when the buffer overflows and
	 * fall back to a real temporary file */
	control->out_maxlen = maxlen - control->page_size;
	control->tmp_outbuf = buf;
	if (!DECOMPRESS && !TEST_ONLY)
		control->out_ofs = control->out_len = MAGIC_LEN;\
	return true;
}