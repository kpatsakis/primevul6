static int r_cmd_java_print_class_definitions( RBinJavaObj *obj ) {
	RList * the_fields = r_bin_java_get_field_definitions (obj),
			* the_methods = r_bin_java_get_method_definitions (obj),
			* the_imports = r_bin_java_get_import_definitions (obj),
			* the_moffsets = r_bin_java_get_method_offsets (obj),
			* the_foffsets = r_bin_java_get_field_offsets (obj);

	char * class_name = r_bin_java_get_this_class_name(obj);
	char * str = NULL;

	r_cmd_java_print_import_definitions (obj);
	r_cons_printf ("\nclass %s { // @0x%04"PFMT64x"\n", class_name, obj->loadaddr);

	if (the_fields && the_foffsets && r_list_length (the_fields) > 0) {
		r_cons_printf ("\n  // Fields defined in the class\n");
		ut32 idx = 0, end = r_list_length (the_fields);

		while (idx < end) {
			ut64 *addr = r_list_get_n (the_foffsets, idx);
			str = r_list_get_n (the_fields, idx);
			r_cons_printf("  %s; // @0x%04"PFMT64x"\n", str, *addr);
			idx++;
		}
	}

	if (the_methods && the_moffsets && r_list_length (the_methods) > 0) {
		r_cons_printf ("\n  // Methods defined in the class\n");
		ut32 idx = 0, end = r_list_length (the_methods);

		while (idx < end) {
			ut64 *addr = r_list_get_n (the_moffsets, idx);
			str = r_list_get_n (the_methods, idx);
			r_cons_printf ("  %s; // @0x%04"PFMT64x"\n", str, *addr);
			idx++;
		}
	}
	r_cons_printf ("}\n");

	r_list_free (the_imports);
	r_list_free (the_fields);
	r_list_free (the_methods);
	r_list_free (the_foffsets);
	r_list_free (the_moffsets);

	free(class_name);
	return true;
}