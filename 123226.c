static int r_cmd_java_print_all_definitions( RAnal *anal ) {
	RList * obj_list  = r_cmd_java_get_bin_obj_list (anal);
	RListIter *iter;
	RBinJavaObj *obj;

	if (!obj_list) {
		return 1;
	}
	r_list_foreach (obj_list, iter, obj) {
		r_cmd_java_print_class_definitions (obj);
	}
	return true;
}