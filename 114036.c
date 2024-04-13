static inline uint32_t htx_meta_space(const struct htx *htx)
{
	if (htx->tail == -1)
		return 0;

	return ((htx->tail + 1 - htx->head) * sizeof(struct htx_blk));
}