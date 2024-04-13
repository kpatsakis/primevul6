static inline struct htx_blk *htx_get_head_blk(const struct htx *htx)
{
	int32_t head = htx_get_head(htx);

	return ((head == -1) ? NULL : htx_get_blk(htx, head));
}