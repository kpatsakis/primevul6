static inline void htx_cut_data_blk(struct htx *htx, struct htx_blk *blk, uint32_t n)
{
	if (blk->addr == htx->end_addr)
		htx->end_addr += n;
	blk->addr += n;
	blk->info -= n;
	htx->data -= n;
}