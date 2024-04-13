R_API void UNUSED_FUNCTION(r_bin_java_print_prototypes)(RBinJavaObj * bin) {
	RList *the_list = r_bin_java_get_method_definitions (bin);
	RListIter *iter;
	char *str;
	r_list_foreach (the_list, iter, str) {
		eprintf ("%s;\n", str);
	}
	r_list_free (the_list);
}