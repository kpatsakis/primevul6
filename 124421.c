int open_tmpoutfile(rzip_control *control)
{
	int fd_out;

	if (STDOUT && !TEST_ONLY)
		print_verbose("Outputting to stdout.\n");
	if (control->tmpdir) {
		control->outfile = realloc(NULL, strlen(control->tmpdir) + 16);
		if (unlikely(!control->outfile))
			fatal_return(("Failed to allocate outfile name\n"), -1);
		strcpy(control->outfile, control->tmpdir);
		strcat(control->outfile, "lrzipout.XXXXXX");
	}

	fd_out = mkstemp(control->outfile);
	if (fd_out == -1) {
		print_progress("WARNING: Failed to create out tmpfile: %s, will fail if cannot perform %scompression entirely in ram\n",
			       control->outfile, DECOMPRESS ? "de" : "");
	} else
		register_outfile(control, control->outfile, TEST_ONLY || STDOUT || !KEEP_BROKEN);
	return fd_out;
}