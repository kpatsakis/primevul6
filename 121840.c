R_API RBinJavaElementValuePair *r_bin_java_element_pair_new(ut8 *buffer, ut64 sz, ut64 buf_offset) {
	RBinJavaElementValuePair *ev_pair = NULL;
	ut64 offset = 0;
	if (!buffer) {
		return NULL;
	}
	ev_pair = R_NEW0 (RBinJavaElementValuePair);
	if (!ev_pair) {
		// TODO eprintf ev_pair failed to allocate
		return NULL;
	}
	// TODO: What is the signifigance of ev_pair element
	ev_pair->element_name_idx = R_BIN_JAVA_USHORT (buffer, offset);
	offset += 2;
	ev_pair->file_offset = buf_offset;
	ev_pair->name = r_bin_java_get_utf8_from_bin_cp_list (R_BIN_JAVA_GLOBAL_BIN, ev_pair->element_name_idx);
	if (ev_pair->name == NULL) {
		// TODO: eprintf unable to find the name for the given index
		eprintf ("ElementValue Name is invalid.\n");
		ev_pair->name = strdup ("UNKNOWN");
	}
	ev_pair->value = r_bin_java_element_value_new (buffer + offset, sz - offset, buf_offset + offset);
	offset += ev_pair->value->size;
	ev_pair->size = offset;
	return ev_pair;
}