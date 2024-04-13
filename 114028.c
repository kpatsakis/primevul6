static inline struct htx_blk *htx_get_tail_blk(const struct htx *htx)
{
	int32_t tail = htx_get_tail(htx);

	return ((tail == -1) ? NULL : htx_get_blk(htx, tail));
}