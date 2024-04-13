R_API RList *r_bin_java_get_symbols(RBinJavaObj *bin) {
	RListIter *iter = NULL, *iter_tmp = NULL;
	RList *imports, *symbols = r_list_newf (free);
	RBinSymbol *sym = NULL;
	RBinImport *imp;
	RBinJavaField *fm_type;
	r_list_foreach_safe (bin->methods_list, iter, iter_tmp, fm_type) {
		sym = r_bin_java_create_new_symbol_from_field (fm_type, bin->loadaddr);
		if (sym) {
			r_list_append (symbols, (void *) sym);
		}
		sym = r_bin_java_create_new_symbol_from_fm_type_meta (fm_type, bin->loadaddr);
		if (sym) {
			r_list_append (symbols, (void *) sym);
		}
	}
	r_list_foreach_safe (bin->fields_list, iter, iter_tmp, fm_type) {
		sym = r_bin_java_create_new_symbol_from_field (fm_type, bin->loadaddr);
		if (sym) {
			r_list_append (symbols, (void *) sym);
		}
		sym = r_bin_java_create_new_symbol_from_fm_type_meta (fm_type, bin->loadaddr);
		if (sym) {
			r_list_append (symbols, (void *) sym);
		}
	}
	bin->lang = "java";
	imports = r_bin_java_get_imports (bin);
	r_list_foreach (imports, iter, imp) {
		sym = R_NEW0 (RBinSymbol);
		if (!sym) {
			break;
		}
		if (imp->classname && !strncmp (imp->classname, "kotlin/jvm", 10)) {
			bin->lang = "kotlin";
		}
		sym->name = r_str_newf ("imp.%s", imp->name);
		if (!sym->name) {
			free (sym);
			break;
		}
		sym->type = r_str_const ("import");
		if (!sym->type) {
			free (sym);
			break;
		}
		sym->vaddr = sym->paddr = imp->ordinal;
		sym->ordinal = imp->ordinal;
		r_list_append (symbols, (void *) sym);
	}
	r_list_free (imports);
	return symbols;
}