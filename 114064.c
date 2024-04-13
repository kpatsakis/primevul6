static inline struct htx_blk *htx_get_first_blk(const struct htx *htx)
{
	int32_t pos;

	pos = htx_get_first(htx);
	return ((pos == -1) ? NULL : htx_get_blk(htx, pos));
}