R_API ut8 *U(r_bin_java_cp_append_ref_cname_fname_ftype)(RBinJavaObj * bin, ut32 * out_sz, ut8 tag, const char *cname, const ut32 c_len, const char *fname, const ut32 f_len, const char *tname, const ut32 t_len) {
	ut32 cn_len = 0, fn_len = 0, ft_len = 0, total_len;
	ut16 cn_idx = 0, fn_idx = 0, ft_idx = 0;
	ut8 *bytes = NULL, *cn_bytes = NULL, *fn_bytes = NULL, *ft_bytes = NULL, *cref_bytes = NULL, *fref_bytes = NULL, *fnt_bytes = NULL;
	*out_sz = 0;
	cn_bytes = r_bin_java_cp_get_utf8 (R_BIN_JAVA_CP_UTF8, &cn_len, (const ut8 *) cname, c_len);
	cn_idx = bin->cp_idx + 1;
	if (cn_bytes) {
		fn_bytes = r_bin_java_cp_get_utf8 (R_BIN_JAVA_CP_UTF8, &fn_len, (const ut8 *) fname, f_len);
		fn_idx = bin->cp_idx + 2;
	}
	if (fn_bytes) {
		ft_bytes = r_bin_java_cp_get_utf8 (R_BIN_JAVA_CP_UTF8, &ft_len, (const ut8 *) tname, t_len);
		ft_idx = bin->cp_idx + 3;
	}
	if (cn_bytes && fn_bytes && ft_bytes) {
		ut32 cref_len = 0, fnt_len = 0, fref_len = 0;
		ut32 cref_idx = 0, fnt_idx = 0;
		cref_bytes = r_bin_java_cp_get_classref (bin, &cref_len, NULL, 0, cn_idx);
		cref_idx = bin->cp_idx + 3;
		fnt_bytes = r_bin_java_cp_get_name_type (bin, &fnt_len, fn_idx, ft_idx);
		fnt_idx = bin->cp_idx + 4;
		fref_bytes = r_bin_java_cp_get_2_ut16 (bin, &fref_len, tag, cref_idx, fnt_idx);
		if (cref_bytes && fref_bytes && fnt_bytes) {
			total_len = cn_len + fn_len + ft_len + cref_len + fnt_len + fref_len + 2;
			if (total_len < cn_len) {
				goto beach;
			}
			bytes = calloc (1, total_len);
			// class name bytes
			if (*out_sz + cn_len >= total_len) {
				goto beach;
			}
			memcpy (bytes, cn_bytes + *out_sz, cn_len);
			*out_sz += cn_len;
			// field name bytes
			if (*out_sz + fn_len >= total_len) {
				goto beach;
			}
			memcpy (bytes, fn_bytes + *out_sz, fn_len);
			*out_sz += fn_len;
			// field type bytes
			if (*out_sz + ft_len >= total_len) {
				goto beach;
			}
			memcpy (bytes, ft_bytes + *out_sz, ft_len);
			*out_sz += ft_len;
			// class ref bytes
			if (*out_sz + cref_len >= total_len) {
				goto beach;
			}
			memcpy (bytes, cref_bytes + *out_sz, cref_len);
			*out_sz += fn_len;
			// field name and type bytes
			if (*out_sz + fnt_len >= total_len) {
				goto beach;
			}
			memcpy (bytes, fnt_bytes + *out_sz, fnt_len);
			*out_sz += fnt_len;
			// field ref bytes
			if (*out_sz + fref_len >= total_len) {
				goto beach;
			}
			memcpy (bytes, fref_bytes + *out_sz, fref_len);
			*out_sz += fref_len;
		}
	}
beach:
	free (cn_bytes);
	free (ft_bytes);
	free (fn_bytes);
	free (fnt_bytes);
	free (fref_bytes);
	free (cref_bytes);
	return bytes;
}