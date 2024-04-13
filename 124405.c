static i64 fdout_seekto(rzip_control *control, i64 pos)
{
	if (TMP_OUTBUF) {
		pos -= control->out_relofs;
		control->out_ofs = pos;
		if (unlikely(pos > control->out_len || pos < 0)) {
			print_err("Trying to seek to %lld outside tmp outbuf in fdout_seekto\n", pos);
			return -1;
		}
		return 0;
	}
	return lseek(control->fd_out, pos, SEEK_SET);
}