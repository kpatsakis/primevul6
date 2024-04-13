int git_pkt_buffer_flush(git_buf *buf)
{
	return git_buf_put(buf, pkt_flush_str, strlen(pkt_flush_str));
}