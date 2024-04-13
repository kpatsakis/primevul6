R_API RList *r_bin_java_get_method_offsets(RBinJavaObj *bin) {
	RBinJavaField *fm_type = NULL;
	RList *the_list = r_list_new ();
	RListIter *iter = NULL;
	ut64 *paddr = NULL;
	if (!bin) {
		return the_list;
	}
	the_list->free = free;
	r_list_foreach (bin->methods_list, iter, fm_type) {
		paddr = R_NEW0 (ut64);
		*paddr = fm_type->file_offset + bin->loadaddr;
		r_list_append (the_list, paddr);
	}
	return the_list;
}