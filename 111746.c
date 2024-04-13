static RBinImport *import_from_name(RBin *rbin, const char *orig_name, HtPP *imports_by_name) {
	if (imports_by_name) {
		bool found = false;
		RBinImport *ptr = ht_pp_find (imports_by_name, orig_name, &found);
		if (found) {
			return ptr;
		}
	}

	RBinImport *ptr = NULL;
	if (!(ptr = R_NEW0 (RBinImport))) {
		return NULL;
	}

	char *name = (char*) orig_name;
	const char *_objc_class = "_OBJC_CLASS_$";
	const int _objc_class_len = strlen (_objc_class);
	const char *_objc_metaclass = "_OBJC_METACLASS_$";
	const int _objc_metaclass_len = strlen (_objc_metaclass);
	char *type = "FUNC";

	if (!strncmp (name, _objc_class, _objc_class_len)) {
		name += _objc_class_len;
		type = "OBJC_CLASS";
	} else if (!strncmp (name, _objc_metaclass, _objc_metaclass_len)) {
		name += _objc_metaclass_len;
		type = "OBJC_METACLASS";
	}

	// Remove the extra underscore that every import seems to have in Mach-O.
	if (*name == '_') {
		name++;
	}
	ptr->name = strdup (name);
	ptr->bind = "NONE";
	ptr->type = r_str_constpool_get (&rbin->constpool, type);

	if (imports_by_name) {
		ht_pp_insert (imports_by_name, orig_name, ptr);
	}

	return ptr;
}