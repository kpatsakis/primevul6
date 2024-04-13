static inline void htx_skip_msg_payload(struct htx *htx)
{
	struct htx_blk *blk = htx_get_first_blk(htx);

	while (blk) {
		enum htx_blk_type type = htx_get_blk_type(blk);

		blk = ((type > HTX_BLK_EOH)
		       ? htx_remove_blk(htx, blk)
		       : htx_get_next_blk(htx, blk));
	}
}