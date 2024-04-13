R_API ut8 r_bin_java_quick_check(ut8 expected_tag, ut8 actual_tag, ut32 actual_len, const char *name) {
	ut8 res = 0;
	if (expected_tag > R_BIN_JAVA_CP_METAS_SZ) {
		eprintf ("Invalid tag '%d' expected 0x%02x for %s.\n", actual_tag, expected_tag, name);
		res = 1;
	} else if (expected_tag != actual_tag)  {
		eprintf ("Invalid tag '%d' expected 0x%02x for %s.\n", actual_tag, expected_tag, name);
		res = 1;
	} else if (actual_len < R_BIN_JAVA_CP_METAS[expected_tag].len)  {
		eprintf ("Unable to parse '%d' expected sz=0x%02x got 0x%02x for %s.\n",
			actual_tag, R_BIN_JAVA_CP_METAS[expected_tag].len, actual_len, name);
		res = 2;
	}
	return res;
}