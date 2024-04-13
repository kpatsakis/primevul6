static inline void *htx_get_blk_ptr(const struct htx *htx, const struct htx_blk *blk)
{
	return ((void *)htx->blocks + blk->addr);
}