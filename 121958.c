R_API ut8 *r_bin_java_cp_get_fm_ref(RBinJavaObj *bin, ut32 *out_sz, ut8 tag, ut16 class_idx, ut16 name_and_type_idx) {
	return r_bin_java_cp_get_2_ut16 (bin, out_sz, tag, class_idx, name_and_type_idx);
}