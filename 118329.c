static int r_bin_dyldcache_init(struct r_bin_dyldcache_obj_t* bin) {
	int len = r_buf_fread_at (bin->b, 0, (ut8*)&bin->hdr, "16c4i7l", 1);
	if (len == -1) {
		perror ("read (cache_header)");
		return false;
	}
	bin->nlibs = bin->hdr.numlibs;
	return true;
}