R_API RBinJavaCPTypeObj *r_bin_java_find_cp_ref_info(RBinJavaObj *bin, ut16 name_and_type_idx) {
	RListIter *iter, *iter_tmp;
	RBinJavaCPTypeObj *res = NULL, *obj = NULL;
	r_list_foreach_safe (bin->cp_list, iter, iter_tmp, obj) {
		if (obj->tag == R_BIN_JAVA_CP_FIELDREF &&
		obj->info.cp_field.name_and_type_idx == name_and_type_idx) {
			res = obj;
			break;
		} else if (obj->tag == R_BIN_JAVA_CP_METHODREF &&
		obj->info.cp_method.name_and_type_idx == name_and_type_idx) {
			res = obj;
			break;
		}
	}
	return res;
}