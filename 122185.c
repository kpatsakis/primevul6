R_API RList *r_bin_java_enum_class_methods(RBinJavaObj *bin, ut16 class_idx) {
	RList *methods = r_list_newf (free);
	RListIter *iter;
	RBinJavaField *field;
	r_list_foreach (bin->methods_list, iter, field) {
		if (field->field_ref_cp_obj && 0) {
			if ((field && field->field_ref_cp_obj->metas->ord == class_idx)) {
				RBinSymbol *sym = r_bin_java_create_new_symbol_from_ref (
						field->field_ref_cp_obj, bin->loadaddr);
				if (sym) {
					r_list_append (methods, sym);
				}
			}
		} else {
			RBinSymbol *sym = R_NEW0 (RBinSymbol);
			sym->name = strdup (field->name);
			// func defintion
			// sym->paddr = field->file_offset + bin->loadaddr;
			// code implementation
			sym->paddr = r_bin_java_get_method_code_offset (field);
			sym->vaddr = sym->paddr; // + bin->loadaddr;
			r_list_append (methods, sym);
		}
	}
	return methods;
}