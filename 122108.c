R_API RList *r_bin_java_enum_class_fields(RBinJavaObj *bin, ut16 class_idx) {
	RList *fields = r_list_newf (free);
	RListIter *iter;
	RBinJavaField *fm_type;
	RBinField *field = NULL;
	r_list_foreach (bin->fields_list, iter, fm_type) {
		if (fm_type) {
			if (fm_type && fm_type->field_ref_cp_obj
			&& fm_type->field_ref_cp_obj->metas->ord == class_idx) {
				field = r_bin_java_create_new_rbinfield_from_field (fm_type, bin->loadaddr);
				if (field) {
					r_list_append (fields, field);
				}
			}
		}
	}
	return fields;
}