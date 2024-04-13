static inline void htx_to_buf(struct htx *htx, struct buffer *buf)
{
	if ((htx->head == -1) &&
	    !(htx->flags & (HTX_FL_PARSING_ERROR|HTX_FL_PROCESSING_ERROR))) {
		htx_reset(htx);
		b_set_data(buf, 0);
	}
	else
		b_set_data(buf, b_size(buf));
}