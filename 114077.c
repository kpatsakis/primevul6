static inline int htx_is_unique_blk(const struct htx *htx,
				    const struct htx_blk *blk)
{
	return (htx_get_blksz(blk) == htx->data);
}