R_API ut8 *U(r_bin_java_cp_get_method_ref)(RBinJavaObj * bin, ut32 * out_sz, ut16 class_idx, ut16 name_and_type_idx) {
	return r_bin_java_cp_get_fm_ref (bin, out_sz, R_BIN_JAVA_CP_METHODREF, class_idx, name_and_type_idx);
}