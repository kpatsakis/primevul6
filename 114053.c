static inline void htx_change_blk_value_len(struct htx *htx, struct htx_blk *blk, uint32_t newlen)
{
	enum htx_blk_type type = htx_get_blk_type(blk);
	uint32_t oldlen, sz;
	int32_t delta;

	sz = htx_get_blksz(blk);
	switch (type) {
		case HTX_BLK_HDR:
		case HTX_BLK_TLR:
			oldlen = (blk->info >> 8) & 0xfffff;
			blk->info = (type << 28) + (newlen << 8) + (blk->info & 0xff);
			break;
		default:
			oldlen = blk->info & 0xfffffff;
			blk->info = (type << 28) + newlen;
			break;
	}

	/* Update HTTP message */
	delta = (newlen - oldlen);
	htx->data += delta;
	if (blk->addr+sz == htx->tail_addr)
		htx->tail_addr += delta;
	else if (blk->addr+sz == htx->head_addr)
		htx->head_addr += delta;
}