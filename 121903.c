R_API ut32 r_bin_java_cp_get_size(RBinJavaObj *bin, ut16 idx) {
	RBinJavaCPTypeObj *cp_obj = r_bin_java_get_item_from_bin_cp_list (bin, idx);
	switch (cp_obj->tag) {
	case R_BIN_JAVA_CP_INTEGER:
	case R_BIN_JAVA_CP_FLOAT:
		return 1 + 4;
	case R_BIN_JAVA_CP_LONG:
	case R_BIN_JAVA_CP_DOUBLE:
		return 1 + 8;
	case R_BIN_JAVA_CP_UTF8:
		return 1 + 2 + cp_obj->info.cp_utf8.length;
	}
	return 0;
}