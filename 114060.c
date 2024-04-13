static inline uint32_t htx_pos_to_addr(const struct htx *htx, uint32_t pos)
{
	return htx->size - (pos + 1) * sizeof(struct htx_blk);
}