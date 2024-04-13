R_API ut64 r_bin_java_annotation_default_attr_calc_size(RBinJavaAttrInfo *attr) {
	ut64 size = 0;
	if (attr) {
		// attr = r_bin_java_default_attr_new (buffer, sz, buf_offset);
		size += 6;
		// attr->info.annotation_default_attr.default_value = r_bin_java_element_value_new (buffer+offset, sz-offset, buf_offset+offset);
		size += r_bin_java_element_value_calc_size (attr->info.annotation_default_attr.default_value);
	}
	return size;
}