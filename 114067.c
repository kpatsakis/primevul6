static inline struct htx *htx_from_buf(struct buffer *buf)
{
	struct htx *htx = htxbuf(buf);

	b_set_data(buf, b_size(buf));
	return htx;
}