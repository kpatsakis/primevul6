R_API void r_bin_java_print_element_pair_summary(RBinJavaElementValuePair *ev_pair) {
	if (!ev_pair) {
		eprintf ("Attempting to print an invalid RBinJavaElementValuePair *pair.\n");
		return;
	}
	Eprintf ("Element Value Pair information:\n");
	Eprintf ("  EV Pair File Offset: 0x%08"PFMT64x "\n", ev_pair->file_offset);
	Eprintf ("  EV Pair Element Name index: 0x%02x\n", ev_pair->element_name_idx);
	Eprintf ("  EV Pair Element Name: %s\n", ev_pair->name);
	Eprintf ("  EV Pair Element Value:\n");
	r_bin_java_print_element_value_summary (ev_pair->value);
}