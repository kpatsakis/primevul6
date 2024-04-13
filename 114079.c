static inline struct htx_sl *htx_add_stline(struct htx *htx, enum htx_blk_type type, unsigned int flags,
					    const struct ist p1, const struct ist p2, const struct ist p3)
{
	struct htx_blk *blk;
	struct htx_sl  *sl;
	uint32_t size;

	if (type != HTX_BLK_REQ_SL && type != HTX_BLK_RES_SL)
		return NULL;

	size = sizeof(*sl) + p1.len + p2.len + p3.len;

	blk = htx_add_blk(htx, type, size);
	if (!blk)
		return NULL;
	blk->info += size;

	sl = htx_get_blk_ptr(htx, blk);
	sl->flags = flags;

	HTX_SL_P1_LEN(sl) = p1.len;
	HTX_SL_P2_LEN(sl) = p2.len;
	HTX_SL_P3_LEN(sl) = p3.len;

	memcpy(HTX_SL_P1_PTR(sl), p1.ptr, p1.len);
	memcpy(HTX_SL_P2_PTR(sl), p2.ptr, p2.len);
	memcpy(HTX_SL_P3_PTR(sl), p3.ptr, p3.len);

	return sl;
}