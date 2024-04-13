static bool preserve_times(rzip_control *control, int fd_in)
{
	struct utimbuf times;
	struct stat st;

	if (unlikely(fstat(fd_in, &st)))
		fatal_return(("Failed to fstat input file\n"), false);
	times.actime = 0;
	times.modtime = st.st_mtime;
	if (unlikely(utime(control->outfile, &times)))
		print_verbose("Warning, unable to set time on %s\n", control->outfile);

	return true;
}