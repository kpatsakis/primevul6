bool dump_tmpoutfile(rzip_control *control, int fd_out)
{
	FILE *tmpoutfp;
	int tmpchar;

	if (unlikely(fd_out == -1))
		fatal_return(("Failed: No temporary outfile created, unable to do in ram\n"), false);
	/* flush anything not yet in the temporary file */
	fsync(fd_out);
	tmpoutfp = fdopen(fd_out, "r");
	if (unlikely(tmpoutfp == NULL))
		fatal_return(("Failed to fdopen out tmpfile\n"), false);
	rewind(tmpoutfp);

	if (!TEST_ONLY) {
		print_verbose("Dumping temporary file to control->outFILE.\n");
		while ((tmpchar = fgetc(tmpoutfp)) != EOF)
			putchar(tmpchar);
		fflush(control->outFILE);
		rewind(tmpoutfp);
	}

	if (unlikely(ftruncate(fd_out, 0)))
		fatal_return(("Failed to ftruncate fd_out in dump_tmpoutfile\n"), false);
	return true;
}