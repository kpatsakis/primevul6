static inline uint32_t htx_get_blksz(const struct htx_blk *blk)
{
	enum htx_blk_type type = htx_get_blk_type(blk);

	switch (type) {
		case HTX_BLK_HDR:
		case HTX_BLK_TLR:
			/*       name.length       +        value.length        */
			return ((blk->info & 0xff) + ((blk->info >> 8) & 0xfffff));
		default:
			/*         value.length      */
			return (blk->info & 0xfffffff);
	}
}