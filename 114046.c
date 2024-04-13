static inline int32_t htx_get_prev(const struct htx *htx, uint32_t pos)
{
	if (htx->head == -1 || pos == htx->head)
		return -1;
	return (pos - 1);
}