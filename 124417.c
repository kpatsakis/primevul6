int open_tmpinfile(rzip_control *control)
{
	int fd_in = -1;

	/* Use temporary directory if there is one */
	if (control->tmpdir) {
		control->infile = malloc(strlen(control->tmpdir) + 15);
		if (unlikely(!control->infile))
			fatal_return(("Failed to allocate infile name\n"), -1);
		strcpy(control->infile, control->tmpdir);
		strcat(control->infile, "lrzipin.XXXXXX");
		fd_in = mkstemp(control->infile);
	}

	/* Try the current directory */
	if (fd_in == -1) {
		dealloc(control->infile);
		control->infile = malloc(16);
		if (unlikely(!control->infile))
			fatal_return(("Failed to allocate infile name\n"), -1);
		strcpy(control->infile, "lrzipin.XXXXXX");
		fd_in = mkstemp(control->infile);
	}

	/* Use /tmp if nothing is writeable so far */
	if (fd_in == -1) {
		dealloc(control->infile);
		control->infile = malloc(20);
		if (unlikely(!control->infile))
			fatal_return(("Failed to allocate infile name\n"), -1);
		strcpy(control->infile, "/tmp/lrzipin.XXXXXX");
		fd_in = mkstemp(control->infile);
	}

	if (fd_in == -1) {
		print_progress("WARNING: Failed to create in tmpfile: %s, will fail if cannot perform %scompression entirely in ram\n",
			       control->infile, DECOMPRESS ? "de" : "");
	} else {
		register_infile(control, control->infile, (DECOMPRESS || TEST_ONLY) && STDIN);
		/* Unlink temporary file immediately to minimise chance of files left
		* lying around in cases of failure_return((. */
		if (unlikely(unlink(control->infile))) {
			fatal("Failed to unlink tmpfile: %s\n", control->infile);
			close(fd_in);
			return -1;
		}
	}
	return fd_in;
}