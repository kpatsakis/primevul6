bool read_magic(rzip_control *control, int fd_in, i64 *expected_size)
{
	char magic[MAGIC_LEN];

	memset(magic, 0, sizeof(magic));
	/* Initially read only <v0.6x header */
	if (unlikely(read(fd_in, magic, 24) != 24))
		fatal_return(("Failed to read magic header\n"), false);

	if (unlikely(!get_magic(control, magic)))
		return false;
	*expected_size = control->st_size;
	return true;
}