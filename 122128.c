R_API ut32 r_bin_java_get_method_count(RBinJavaObj *bin_obj) {
	return r_list_length (bin_obj->methods_list);
}