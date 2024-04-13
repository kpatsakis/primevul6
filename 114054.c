static inline void htx_set_blk_value_len(struct htx_blk *blk, uint32_t vlen)
{
	enum htx_blk_type type = htx_get_blk_type(blk);

	switch (type) {
		case HTX_BLK_HDR:
		case HTX_BLK_TLR:
			blk->info = (type << 28) + (vlen << 8) + (blk->info & 0xff);
			break;
		case HTX_BLK_REQ_SL:
		case HTX_BLK_RES_SL:
		case HTX_BLK_DATA:
			blk->info = (type << 28) + vlen;
			break;
		default:
			/* Unexpected case */
			break;
	}
}