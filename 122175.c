R_API ut64 r_bin_java_get_main(RBinJavaObj *bin) {
	if (bin->main_code_attr) {
		return bin->main_code_attr->info.code_attr.code_offset + bin->loadaddr;
	}
	return 0;
}