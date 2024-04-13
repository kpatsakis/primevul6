R_API ut8 *r_bin_java_cp_get_4bytes(ut8 tag, ut32 *out_sz, const ut8 *buf, const ut64 len) {
	ut8 *buffer = malloc (5);
	if (!buffer) {
		return NULL;
	}
	ut32 val = 0;
	if (!buffer || len < 4) {
		if (out_sz) {
			*out_sz = 0;
		}
		free (buffer);
		return NULL;
	}
	buffer[0] = tag;
	val = R_BIN_JAVA_UINT (buf, 0);
	memcpy (buffer + 1, (const char *) &val, 4);
	*out_sz = 5;
	return buffer;
}