static inline uint32_t htx_free_data_space(const struct htx *htx)
{
	uint32_t free = htx_free_space(htx);

	if (free < sizeof(struct htx_blk))
		return 0;
	return (free - sizeof(struct htx_blk));
}