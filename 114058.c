static inline struct ist htx_get_blk_value(const struct htx *htx, const struct htx_blk *blk)
{
	enum htx_blk_type type = htx_get_blk_type(blk);
	struct ist ret;

	switch (type) {
		case HTX_BLK_HDR:
		case HTX_BLK_TLR:
			ret.ptr = htx_get_blk_ptr(htx, blk) + (blk->info & 0xff);
			ret.len = (blk->info >> 8) & 0xfffff;
			break;

		case HTX_BLK_REQ_SL:
		case HTX_BLK_RES_SL:
		case HTX_BLK_DATA:
			ret.ptr = htx_get_blk_ptr(htx, blk);
			ret.len = blk->info & 0xfffffff;
			break;

		default:
			return ist("");
	}
	return ret;
}