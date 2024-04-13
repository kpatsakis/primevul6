static inline struct htx_blk *htx_get_prev_blk(const struct htx *htx,
					       const struct htx_blk *blk)
{
	int32_t pos;

	pos = htx_get_prev(htx, htx_get_blk_pos(htx, blk));
	return ((pos == -1) ? NULL : htx_get_blk(htx, pos));
}