bool get_header_info(rzip_control *control, int fd_in, uchar *ctype, i64 *c_len,
		     i64 *u_len, i64 *last_head, int chunk_bytes)
{
	if (unlikely(read(fd_in, ctype, 1) != 1))
		fatal_return(("Failed to read in get_header_info\n"), false);

	*c_len = *u_len = *last_head = 0;
	if (control->major_version == 0 && control->minor_version < 4) {
		u32 c_len32, u_len32, last_head32;

		if (unlikely(read(fd_in, &c_len32, 4) != 4))
			fatal_return(("Failed to read in get_header_info"), false);
		if (unlikely(read(fd_in, &u_len32, 4) != 4))
			fatal_return(("Failed to read in get_header_info"), false);
		if (unlikely(read(fd_in, &last_head32, 4) != 4))
			fatal_return(("Failed to read in get_header_info"), false);
		c_len32 = le32toh(c_len32);
		u_len32 = le32toh(u_len32);
		last_head32 = le32toh(last_head32);
		*c_len = c_len32;
		*u_len = u_len32;
		*last_head = last_head32;
	} else {
		int read_len;

		if (control->major_version == 0 && control->minor_version == 5)
			read_len = 8;
		else
			read_len = chunk_bytes;
		if (unlikely(read(fd_in, c_len, read_len) != read_len))
			fatal_return(("Failed to read in get_header_info"), false);
		if (unlikely(read(fd_in, u_len, read_len) != read_len))
			fatal_return(("Failed to read in get_header_info"), false);
		if (unlikely(read(fd_in, last_head, read_len) != read_len))
			fatal_return(("Failed to read_i64 in get_header_info"), false);
		*c_len = le64toh(*c_len);
		*u_len = le64toh(*u_len);
		*last_head = le64toh(*last_head);
	}
	return true;
}