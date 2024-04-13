bool write_fdin(rzip_control *control)
{
	uchar *offset_buf = control->tmp_inbuf;
	i64 len = control->in_len;
	ssize_t ret;

	while (len > 0) {
		ret = MIN(len, one_g);
		ret = write(control->fd_in, offset_buf, (size_t)ret);
		if (unlikely(ret <= 0))
			fatal_return(("Failed to write to fd_in in write_fdin\n"), false);
		len -= ret;
		offset_buf += ret;
	}
	return true;
}