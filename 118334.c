void* r_bin_dyldcache_free(struct r_bin_dyldcache_obj_t* bin) {
	if (!bin) {
		return NULL;
	}
	r_buf_free (bin->b);
	free (bin);
	return NULL;
}