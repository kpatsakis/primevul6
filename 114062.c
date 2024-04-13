static inline uint32_t htx_get_blk_pos(const struct htx *htx, const struct htx_blk *blk)
{
	return ((htx->blocks + htx->size - (char *)blk) / sizeof(struct htx_blk) - 1);
}