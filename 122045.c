R_API ut8 *r_bin_java_cp_get_name_type(RBinJavaObj *bin, ut32 *out_sz, ut16 name_idx, ut16 type_idx) {
	return r_bin_java_cp_get_2_ut16 (bin, out_sz, R_BIN_JAVA_CP_NAMEANDTYPE, name_idx, type_idx);
}