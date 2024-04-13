R_API ut8 *r_bin_java_cp_get_8bytes(ut8 tag, ut32 *out_sz, const ut8 *buf, const ut64 len) {
	ut8 *buffer = malloc (10);
	if (!buffer) {
		return NULL;
	}
	ut64 val = 0;
	if (len < 8) {
		*out_sz = 0;
		free (buffer);
		return NULL;
	}
	buffer[0] = tag;
	val = r_bin_java_raw_to_long (buf, 0);
	memcpy (buffer + 1, (const char *) &val, 8);
	*out_sz = 9;
	return buffer;
}