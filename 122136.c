R_API ut64 r_bin_java_signature_attr_calc_size(RBinJavaAttrInfo *attr) {
	ut64 size = 0;
	if (attr == NULL) {
		// TODO eprintf allocation fail
		return size;
	}
	size += 6;
	// attr->info.source_file_attr.sourcefile_idx = R_BIN_JAVA_USHORT (buffer, offset);
	size += 2;
	// attr->info.signature_attr.signature_idx = R_BIN_JAVA_USHORT (buffer, offset);
	size += 2;
	return size;
}