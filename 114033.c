static inline uint32_t htx_free_space(const struct htx *htx)
{
	return (htx->size - htx_used_space(htx));
}