static inline struct htx_blk *htx_add_endof(struct htx *htx, enum htx_blk_type type)
{
	struct htx_blk *blk;

	blk = htx_add_blk(htx, type, 1);
	if (!blk)
		return NULL;

	blk->info += 1;
	return blk;
}