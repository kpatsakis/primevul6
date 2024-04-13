static int r_cmd_java_print_import_definitions ( RBinJavaObj *obj ) {
	RList * the_list = r_bin_java_get_import_definitions (obj);
	char * str = NULL;
	RListIter *iter;
	r_list_foreach (the_list, iter, str) {
		r_cons_printf("import %s;\n", str);
	}
	r_list_free (the_list);
	return true;
}