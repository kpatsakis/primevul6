static inline enum htx_blk_type htx_get_blk_type(const struct htx_blk *blk)
{
	return (blk->info >> 28);
}