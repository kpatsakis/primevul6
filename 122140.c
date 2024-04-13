R_API ut8 *r_bin_java_cp_get_fref_bytes(RBinJavaObj *bin, ut32 *out_sz, ut8 tag, ut16 cn_idx, ut16 fn_idx, ut16 ft_idx) {
	ut8 *bytes = NULL, *fnt_bytes = NULL;
	RBinJavaCPTypeObj *ref_cp_obj = NULL;
	ut16 fnt_idx = 0, cref_idx = 0;
	ut32 fnt_len = 0;
	ut16 ref_cp_obj_idx = r_bin_java_find_cp_class_ref_from_name_idx (bin, cn_idx);
	if (!ref_cp_obj_idx) {
		return NULL;
	}
	ref_cp_obj = r_bin_java_get_item_from_bin_cp_list (bin, ref_cp_obj_idx);
	if (ref_cp_obj) {
		cref_idx = ref_cp_obj->idx;
	}
	ref_cp_obj = r_bin_java_find_cp_name_and_type_info (bin, fn_idx, ft_idx);
	if (ref_cp_obj) {
		fnt_idx = ref_cp_obj->idx;
	} else {
		fnt_bytes = r_bin_java_cp_get_name_type (bin, &fnt_len, fn_idx, ft_idx);
		fnt_idx = bin->cp_idx + 1;
	}
	if (cref_idx && fnt_idx) {
		bytes = r_bin_java_cp_get_fm_ref (bin, out_sz, tag, cref_idx, fnt_idx);
		if (fnt_bytes) {
			ut8 *tbuf = malloc (fnt_len + *out_sz);
			if (!tbuf) {
				free (bytes);
				free (fnt_bytes);
				return NULL;
			}
			// copy the bytes to the new buffer
			memcpy (tbuf, fnt_bytes, fnt_len);
			memcpy (tbuf + fnt_len, bytes, *out_sz);
			// update the values free old buffer
			*out_sz += fnt_len;
			free (bytes);
			bytes = tbuf;
		}
	}
	free (fnt_bytes);
	return bytes;
}