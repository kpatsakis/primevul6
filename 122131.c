R_API ut8 *U(r_bin_java_cp_append_method_ref)(RBinJavaObj * bin, ut32 * out_sz, ut16 cn_idx, ut16 fn_idx, ut16 ft_idx) {
	return r_bin_java_cp_get_fref_bytes (bin, out_sz, R_BIN_JAVA_CP_METHODREF, cn_idx, fn_idx, ft_idx);
}