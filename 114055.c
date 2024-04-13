static inline uint32_t htx_get_max_blksz(const struct htx *htx, int32_t max)
{
	uint32_t free = htx_free_space(htx);

	if (max != -1 && free > max)
		free = max;
	if (free < sizeof(struct htx_blk))
		return 0;
	return (free - sizeof(struct htx_blk));
}