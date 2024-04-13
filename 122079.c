R_API RBinSymbol *r_bin_java_create_new_symbol_from_fm_type_meta(RBinJavaField *fm_type, ut64 baddr) {
	RBinSymbol *sym = R_NEW0 (RBinSymbol);
	if (!sym || !fm_type || !fm_type->field_ref_cp_obj || fm_type->field_ref_cp_obj == &R_BIN_JAVA_NULL_TYPE) {
		free (sym);
		return NULL;
	}
	// ut32 new_name_len = strlen (fm_type->name) + strlen ("_meta") + 1;
	// char *new_name = malloc (new_name_len);
	sym->name = r_str_newf ("meta_%s", fm_type->name);
	if (fm_type->type == R_BIN_JAVA_FIELD_TYPE_METHOD) {
		sym->type = r_str_const ("FUNC_META");
	} else {
		sym->type = r_str_const ("FIELD_META");
	}
	if (r_bin_java_is_fm_type_protected (fm_type)) {
		sym->bind = r_str_const (R_BIN_BIND_LOCAL_STR);
	} else if (r_bin_java_is_fm_type_private (fm_type)) {
		sym->bind = r_str_const (R_BIN_BIND_LOCAL_STR);
	} else if (r_bin_java_is_fm_type_protected (fm_type)) {
		sym->bind = r_str_const (R_BIN_BIND_GLOBAL_STR);
	}
	sym->forwarder = r_str_const ("NONE");
	if (fm_type->class_name) {
		sym->classname = strdup (fm_type->class_name);
	} else {
		sym->classname = strdup ("UNKNOWN");
	}
	sym->paddr = fm_type->file_offset;// r_bin_java_get_method_code_offset (fm_type);
	sym->vaddr = fm_type->file_offset + baddr;
	sym->ordinal = fm_type->metas->ord;
	sym->size = fm_type->size;
	sym->visibility = fm_type->flags;
	if (fm_type->flags_str) {
		sym->visibility_str = strdup (fm_type->flags_str);
	}
	return sym;
}