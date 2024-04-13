R_API void r_bin_java_element_pair_free(void /*RBinJavaElementValuePair*/ *e) {
	RBinJavaElementValuePair *ev_pair = e;
	if (ev_pair) {
		free (ev_pair->name);
		r_bin_java_element_value_free (ev_pair->value);
		free (ev_pair);
	}
	ev_pair = NULL;
}