static int UNUSED_FUNCTION(r_cmd_java_print_field_count) (RBinJavaObj *obj) {
	ut32 res = r_bin_java_get_field_count (obj);
	r_cons_printf ("%d\n", res);
	r_cons_flush();
	return true;
}