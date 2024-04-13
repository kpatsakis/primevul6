free_buffer(isc_mem_t *mctx, isc_buffer_t *buffer) {
	isc_region_t r;

	isc_buffer_region(buffer, &r);
	if (r.base != NULL) {
		isc_mem_put(mctx, r.base, r.length);
	}
}