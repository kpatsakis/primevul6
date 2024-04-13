R_API RBinField *r_bin_java_create_new_rbinfield_from_field(RBinJavaField *fm_type, ut64 baddr) {
	RBinField *field = r_bin_java_allocate_rbinfield ();
	if (field) {
		field->name = strdup (fm_type->name);
		field->paddr = fm_type->file_offset + baddr;
		field->visibility = fm_type->flags;
	}
	return field;
}