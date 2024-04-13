static inline int htx_almost_full(const struct htx *htx)
{
	if (!htx->size || htx_free_space(htx) < htx->size / 4)
		return 1;
	return 0;
}