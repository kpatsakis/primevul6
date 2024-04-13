static int r_bin_dyldcache_apply_patch (struct r_buf_t* buf, ut32 data, ut64 offset) {
	return r_buf_write_at (buf, offset, (ut8*)&data, sizeof (data));
}