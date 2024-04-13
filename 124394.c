bool clear_tmpinfile(rzip_control *control)
{
	if (unlikely(lseek(control->fd_in, 0, SEEK_SET)))
		fatal_return(("Failed to lseek on fd_in in clear_tmpinfile\n"), false);
	if (unlikely(ftruncate(control->fd_in, 0)))
		fatal_return(("Failed to truncate fd_in in clear_tmpinfile\n"), false);
	return true;
}