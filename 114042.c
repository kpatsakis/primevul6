static inline void htx_dump(struct buffer *chunk, const struct htx *htx, int full)
{
	int32_t pos;

	chunk_appendf(chunk, " htx=%p(size=%u,data=%u,used=%u,wrap=%s,flags=0x%08x,extra=%llu,"
		      "first=%d,head=%d,tail=%d,tail_addr=%d,head_addr=%d,end_addr=%d)",
		      htx, htx->size, htx->data, htx_nbblks(htx), (!htx->head_addr) ? "NO" : "YES",
		      htx->flags, (unsigned long long)htx->extra, htx->first, htx->head, htx->tail,
		      htx->tail_addr, htx->head_addr, htx->end_addr);

	if (!full || !htx_nbblks(htx))
		return;
	chunk_memcat(chunk, "\n", 1);

	for (pos = htx_get_head(htx); pos != -1; pos = htx_get_next(htx, pos)) {
		struct htx_sl     *sl;
		struct htx_blk    *blk  = htx_get_blk(htx, pos);
		enum htx_blk_type  type = htx_get_blk_type(blk);
		uint32_t           sz   = htx_get_blksz(blk);
		struct ist         n, v;

		n = htx_get_blk_name(htx, blk);
		v = htx_get_blk_value(htx, blk);

		if (type == HTX_BLK_REQ_SL || type == HTX_BLK_RES_SL) {
			sl = htx_get_blk_ptr(htx, blk);
			chunk_appendf(chunk, "\t\t[%u] type=%-17s - size=%-6u - addr=%-6u\t%.*s %.*s %.*s\n",
				      pos, htx_blk_type_str(type), sz, blk->addr,
				      HTX_SL_P1_LEN(sl), HTX_SL_P1_PTR(sl),
				      HTX_SL_P2_LEN(sl), HTX_SL_P2_PTR(sl),
				      HTX_SL_P3_LEN(sl), HTX_SL_P3_PTR(sl));
		}
		else if (type == HTX_BLK_HDR || type == HTX_BLK_TLR)
			chunk_appendf(chunk, "\t\t[%u] type=%-17s - size=%-6u - addr=%-6u\t%.*s: %.*s\n",
				      pos, htx_blk_type_str(type), sz, blk->addr,
				      (int)MIN(n.len, 32), n.ptr,
				      (int)MIN(v.len, 64), v.ptr);
		else
			chunk_appendf(chunk, "\t\t[%u] type=%-17s - size=%-6u - addr=%-6u%s\n",
				      pos, htx_blk_type_str(type), sz, blk->addr,
				      (!v.len ? "\t<empty>" : ""));
	}
}