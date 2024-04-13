R_API RList *r_bin_java_get_field_offsets(RBinJavaObj *bin) {
	RBinJavaField *fm_type = NULL;
	RList *the_list = r_list_new ();
	if (!the_list) {
		return NULL;
	}
	RListIter *iter = NULL;
	ut64 *paddr = NULL;
	if (!bin) {
		return the_list;
	}
	the_list->free = free;
	r_list_foreach (bin->fields_list, iter, fm_type) {
		paddr = malloc (sizeof(ut64));
		if (!paddr) {
			r_list_free (the_list);
			return NULL;
		}
		*paddr = fm_type->file_offset + bin->loadaddr;
		// eprintf ("Field def: %s, %s, %s, %s\n", fm_type->name, fm_type->descriptor, fm_type->flags_str, field_def);
		r_list_append (the_list, paddr);
	}
	return the_list;
}