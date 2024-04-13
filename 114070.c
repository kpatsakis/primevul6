static inline int32_t htx_get_next(const struct htx *htx, uint32_t pos)
{
	if (htx->tail == -1 || pos == htx->tail)
		return -1;
	return (pos + 1);

}