static inline enum htx_blk_type htx_get_head_type(const struct htx *htx)
{
	struct htx_blk *blk = htx_get_head_blk(htx);

	return (blk ? htx_get_blk_type(blk) : HTX_BLK_UNUSED);
}