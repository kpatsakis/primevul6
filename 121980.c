R_API RBinJavaCPTypeObj *r_bin_java_find_cp_ref_info_from_name_and_type(RBinJavaObj *bin, ut16 name_idx, ut16 descriptor_idx) {
	RBinJavaCPTypeObj *obj = r_bin_java_find_cp_name_and_type_info (bin, name_idx, descriptor_idx);
	if (obj) {
		return r_bin_java_find_cp_ref_info (bin, obj->metas->ord);
	}
	return NULL;
}