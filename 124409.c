static bool fwrite_stdout(rzip_control *control, void *buf, i64 len)
{
	uchar *offset_buf = buf;
	ssize_t ret;
	i64 total;

	total = 0;
	while (len > 0) {
		ssize_t wrote;

		if (len > one_g)
			ret = one_g;
		else
			ret = len;
		wrote = fwrite(offset_buf, 1, ret, control->outFILE);
		if (unlikely(wrote != ret))
			fatal_return(("Failed to fwrite in fwrite_stdout\n"), false);
		len -= ret;
		offset_buf += ret;
		total += ret;
	}
	fflush(control->outFILE);
	return true;
}