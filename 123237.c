static int r_cmd_java_print_method_definitions ( RBinJavaObj *obj ) {
	RList * the_list = r_bin_java_get_method_definitions (obj),
			* off_list = r_bin_java_get_method_offsets (obj);
	char * str = NULL;
	ut32 idx = 0, end = r_list_length (the_list);

	while (idx < end) {
		ut64 *addr = r_list_get_n (off_list, idx);
		str = r_list_get_n (the_list, idx);
		r_cons_printf("%s; // @0x%04"PFMT64x"\n", str, *addr);
		idx++;
	}

	r_list_free (the_list);
	r_list_free (off_list);
	return true;
}