static inline uint32_t htx_used_space(const struct htx *htx)
{
	return (htx->data + htx_meta_space(htx));
}