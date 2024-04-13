static inline int htx_nbblks(const struct htx *htx)
{
	return ((htx->head != -1) ? (htx->tail + 1 - htx->head) : 0);
}