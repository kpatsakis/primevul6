R_API ut64 r_bin_java_parse_attrs(RBinJavaObj *bin, const ut64 offset, const ut8 *buf, const ut64 len) {
	int i = 0;
	ut64 adv = 0;
	const ut8 *a_buf = buf + offset;
	if (offset + 2 >= len) {
		// Check if we can read that USHORT
		return UT64_MAX;
	}
	r_list_free (bin->attrs_list);
	bin->attrs_list = r_list_newf (r_bin_java_attribute_free);
	bin->attrs_offset = offset;
	bin->attrs_count = R_BIN_JAVA_USHORT (a_buf, adv);
	adv += 2;
	if (bin->attrs_count > 0) {
		for (i = 0; i < bin->attrs_count; i++, bin->attr_idx++) {
			RBinJavaAttrInfo *attr = r_bin_java_read_next_attr (bin, offset + adv, buf, len);
			if (!attr) {
				// eprintf ("[X] r_bin_java: Error unable to parse remainder of classfile after Attribute: %d.\n", i);
				break;
			}
			r_list_append (bin->attrs_list, attr);
			adv += attr->size;
			if (adv + offset >= len) {
				// eprintf ("[X] r_bin_java: Error unable to parse remainder of classfile after Attribute: %d.\n", i);
				break;
			}
		}
	}
	bin->attrs_size = adv;
	return adv;
}