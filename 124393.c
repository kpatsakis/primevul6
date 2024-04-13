static bool preserve_perms(rzip_control *control, int fd_in, int fd_out)
{
	struct stat st;

	if (unlikely(fstat(fd_in, &st)))
		fatal_return(("Failed to fstat input file\n"), false);
	if (unlikely(fchmod(fd_out, (st.st_mode & 0666))))
		print_verbose("Warning, unable to set permissions on %s\n", control->outfile);

	/* chown fail is not fatal_return(( */
	if (unlikely(fchown(fd_out, st.st_uid, st.st_gid)))
		print_verbose("Warning, unable to set owner on %s\n", control->outfile);
	return true;
}