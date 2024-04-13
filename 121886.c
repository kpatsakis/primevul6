R_API ut8 *r_bin_java_cp_get_utf8(ut8 tag, ut32 *out_sz, const ut8 *buf, const ut64 len) {
	ut8 *buffer = NULL;
	ut16 sz = 0;
	ut16 t = (ut16) len;
	if (len > 0 && len > (ut16) - 1) {
		*out_sz = 0;
		return NULL;
	}
	sz = R_BIN_JAVA_USHORT (((ut8 *) (ut16 *) &t), 0);
	*out_sz = 3 + t; // tag + sz + bytes
	buffer = malloc (*out_sz + 3);
	if (!buffer) {
		return NULL;
	}
	// XXX - excess bytes are created to ensure null for string operations.
	memset (buffer, 0, *out_sz + 3);
	buffer[0] = tag;
	memcpy (buffer + 1, (const char *) &sz, 2);
	memcpy (buffer + 3, buf, *out_sz - 3);
	return buffer;
}