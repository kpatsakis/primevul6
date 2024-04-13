struct r_bin_dyldcache_obj_t* r_bin_dyldcache_new(const char* file) {
	struct r_bin_dyldcache_obj_t *bin;
	ut8 *buf;
	if (!(bin = R_NEW0 (struct r_bin_dyldcache_obj_t))) {
		return NULL;
	}
	bin->file = file;
	if (!(buf = (ut8*)r_file_slurp (file, &bin->size))) {
		return r_bin_dyldcache_free (bin);
	}
	bin->b = r_buf_new ();
	if (!r_buf_set_bytes (bin->b, buf, bin->size)) {
		free (buf);
		return r_bin_dyldcache_free (bin);
	}
	free (buf);
	if (!r_bin_dyldcache_init (bin)) {
		return r_bin_dyldcache_free (bin);
	}
	return bin;
}