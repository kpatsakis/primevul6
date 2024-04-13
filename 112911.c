int git_pkt_buffer_done(git_buf *buf)
{
	return git_buf_puts(buf, pkt_done_str);
}