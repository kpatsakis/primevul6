R_API RList *r_bin_java_get_lib_names(RBinJavaObj *bin) {
	RList *lib_names = r_list_newf (free);
	RListIter *iter;
	RBinJavaCPTypeObj *cp_obj = NULL;
	if (!bin) {
		return lib_names;
	}
	r_list_foreach (bin->cp_list, iter, cp_obj) {
		if (cp_obj && cp_obj->tag == R_BIN_JAVA_CP_CLASS &&
		(bin->cf2.this_class != cp_obj->info.cp_class.name_idx || !is_class_interface (bin, cp_obj))) {
			char *name = r_bin_java_get_item_name_from_bin_cp_list (bin, cp_obj);
			r_list_append (lib_names, name);
		}
	}
	return lib_names;
}