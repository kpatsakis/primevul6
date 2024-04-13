static inline struct htx_blk *htx_add_all_headers(struct htx *htx, const struct http_hdr *hdrs)
{
	int i;

	for (i = 0; hdrs[i].n.len; i++) {
		if (!htx_add_header(htx, hdrs[i].n, hdrs[i].v))
			return NULL;
	}
	return htx_add_endof(htx, HTX_BLK_EOH);
}