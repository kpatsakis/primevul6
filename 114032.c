static inline struct htx *htxbuf(const struct buffer *buf)
{
	struct htx *htx;

	if (b_is_null(buf))
		return &htx_empty;
	htx = ((struct htx *)(buf->area));
	if (!b_data(buf)) {
		htx->size = buf->size - sizeof(*htx);
		htx_reset(htx);
	}
	return htx;
}