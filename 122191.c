R_API ut64 r_bin_java_get_class_entrypoint(RBinJavaObj *bin) {
	if (bin->cf2.this_class_entrypoint_code_attr) {
		return bin->cf2.this_class_entrypoint_code_attr->info.code_attr.code_offset;
	}
	return 0;
}