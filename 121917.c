R_API ut8 *r_bin_java_cp_get_2_ut16(RBinJavaObj *bin, ut32 *out_sz, ut8 tag, ut16 ut16_one, ut16 ut16_two) {
	ut8 *bytes = malloc (7);
	if (!bytes) {
		return NULL;
	}
	ut8 *idx_addr = NULL;
	bytes[*out_sz] = tag;
	*out_sz += 1;
	idx_addr = (ut8 *) &ut16_one;
	bytes[*out_sz + 1] = idx_addr[1];
	bytes[*out_sz + 2] = idx_addr[0];
	*out_sz += 3;
	idx_addr = (ut8 *) &ut16_two;
	bytes[*out_sz + 1] = idx_addr[1];
	bytes[*out_sz + 2] = idx_addr[0];
	*out_sz += 3;
	return bytes;
}