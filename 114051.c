static inline struct htx_blk *htx_get_blk(const struct htx *htx, uint32_t pos)
{
	return (struct htx_blk *)(htx->blocks + htx_pos_to_addr(htx, pos));
}