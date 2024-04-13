bool write_fdout(rzip_control *control, void *buf, i64 len)
{
	uchar *offset_buf = buf;
	ssize_t ret;

	while (len > 0) {
		ret = MIN(len, one_g);
		ret = write(control->fd_out, offset_buf, (size_t)ret);
		if (unlikely(ret <= 0))
			fatal_return(("Failed to write to fd_out in write_fdout\n"), false);
		len -= ret;
		offset_buf += ret;
	}
	return true;
}