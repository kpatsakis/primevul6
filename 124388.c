bool read_tmpinfile(rzip_control *control, int fd_in)
{
	FILE *tmpinfp;
	int tmpchar;

	if (fd_in == -1)
		return false;
	if (control->flags & FLAG_SHOW_PROGRESS)
		fprintf(control->msgout, "Copying from stdin.\n");
	tmpinfp = fdopen(fd_in, "w+");
	if (unlikely(tmpinfp == NULL))
		fatal_return(("Failed to fdopen in tmpfile\n"), false);

	while ((tmpchar = getchar()) != EOF)
		fputc(tmpchar, tmpinfp);

	fflush(tmpinfp);
	rewind(tmpinfp);
	return true;
}