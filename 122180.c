R_API RList *r_bin_java_get_sections(RBinJavaObj *bin) {
	RBinSection *section = NULL;
	RList *sections = r_list_newf (free);
	ut64 baddr = bin->loadaddr;
	RBinJavaField *fm_type;
	RListIter *iter = NULL;
	if (bin->cp_count > 0) {
		section = R_NEW0 (RBinSection);
		if (section) {
			strcpy (section->name, "constant_pool");
			section->size = bin->cp_size;
			section->paddr = bin->cp_offset + baddr;
			section->srwx = R_BIN_SCN_READABLE;
			section->add = true;
			r_list_append (sections, section);
		}
		section = NULL;
	}
	if (bin->fields_count > 0) {
		section = R_NEW0 (RBinSection);
		if (section) {
			strcpy (section->name, "fields");
			section->size = bin->fields_size;
			section->paddr = bin->fields_offset + baddr;
			section->srwx = R_BIN_SCN_READABLE;
			section->add = true;
			r_list_append (sections, section);
			section = NULL;
			r_list_foreach (bin->fields_list, iter, fm_type) {
				if (fm_type->attr_offset == 0) {
					continue;
				}
				section = R_NEW0 (RBinSection);
				if (section) {
					snprintf (section->name, R_BIN_SIZEOF_STRINGS, "attrs.%s", fm_type->name);
					section->size = fm_type->size - (fm_type->file_offset - fm_type->attr_offset);
					section->paddr = fm_type->attr_offset + baddr;
					section->srwx = R_BIN_SCN_READABLE;
					section->add = true;
					r_list_append (sections, section);
				}
			}
		}
	}
	if (bin->methods_count > 0) {
		section = R_NEW0 (RBinSection);
		if (section) {
			strcpy (section->name, "methods");
			section->size = bin->methods_size;
			section->paddr = bin->methods_offset + baddr;
			section->srwx = R_BIN_SCN_READABLE | R_BIN_SCN_EXECUTABLE;
			section->add = true;
			r_list_append (sections, section);
			section = NULL;
			r_list_foreach (bin->methods_list, iter, fm_type) {
				if (fm_type->attr_offset == 0) {
					continue;
				}
				section = R_NEW0 (RBinSection);
				if (section) {
					snprintf (section->name, R_BIN_SIZEOF_STRINGS, "attrs.%s", fm_type->name);
					section->size = fm_type->size - (fm_type->file_offset - fm_type->attr_offset);
					section->paddr = fm_type->attr_offset + baddr;
					section->srwx = R_BIN_SCN_READABLE;
					section->add = true;
					r_list_append (sections, section);
				}
			}
		}
	}
	if (bin->interfaces_count > 0) {
		section = R_NEW0 (RBinSection);
		if (section) {
			strcpy (section->name, "interfaces");
			section->size = bin->interfaces_size;
			section->paddr = bin->interfaces_offset + baddr;
			section->srwx = R_BIN_SCN_READABLE;
			section->add = true;
			r_list_append (sections, section);
		}
		section = NULL;
	}
	if (bin->attrs_count > 0) {
		section = R_NEW0 (RBinSection);
		if (section) {
			strcpy (section->name, "attributes");
			section->size = bin->attrs_size;
			section->paddr = bin->attrs_offset + baddr;
			section->srwx = R_BIN_SCN_READABLE;
			section->add = true;
			r_list_append (sections, section);
		}
		section = NULL;
	}
	return sections;
}