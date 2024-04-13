R_API ut16 r_bin_java_find_cp_class_ref_from_name_idx(RBinJavaObj *bin, ut16 name_idx) {
	ut16 pos, len = (ut16) r_list_length (bin->cp_list);
	RBinJavaCPTypeObj *item;
	for (pos = 0; pos < len; pos++) {
		item = (RBinJavaCPTypeObj *) r_list_get_n (bin->cp_list, pos);
		if (item && item->tag == R_BIN_JAVA_CP_CLASS && item->info.cp_class.name_idx == name_idx) {
			break;
		}
	}
	return (pos != len) ? pos : 0;
}