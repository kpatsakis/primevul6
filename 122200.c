R_API ut64 r_bin_java_element_pair_calc_size(RBinJavaElementValuePair *ev_pair) {
	ut64 sz = 0;
	if (ev_pair == NULL) {
		return sz;
	}
	// ev_pair->element_name_idx = r_bin_java_read_short(bin, bin->b->cur);
	sz += 2;
	// ev_pair->value = r_bin_java_element_value_new (bin, offset+2);
	if (ev_pair->value) {
		sz += r_bin_java_element_value_calc_size (ev_pair->value);
	}
	return sz;
}