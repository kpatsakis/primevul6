static inline int htx_is_not_empty(const struct htx *htx)
{
	return (htx->head != -1);
}