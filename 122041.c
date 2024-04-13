R_API void *r_bin_java_free(RBinJavaObj *bin) {
	char *bin_obj_key = NULL;
	if (!bin) {
		return NULL;
	}
	// Delete the bin object from the data base.
	bin_obj_key = r_bin_java_build_obj_key (bin);
	// if (bin->AllJavaBinObjs && sdb_exists (bin->AllJavaBinObjs, bin_obj_key)) {
	// sdb_unset (bin->AllJavaBinObjs, bin_obj_key, 0);
	// }
	free (bin_obj_key);
	r_list_free (bin->imports_list);
	// XXX - Need to remove all keys belonging to this class from
	// the share meta information sdb.
	// TODO e.g. iterate over bin->kv and delete all obj, func, etc. keys
	// sdb_free (bin->kv);
	// free up the constant pool list
	r_list_free (bin->cp_list);
	// free up the fields list
	r_list_free (bin->fields_list);
	// free up methods list
	r_list_free (bin->methods_list);
	// free up interfaces list
	r_list_free (bin->interfaces_list);
	r_list_free (bin->attrs_list);
	// TODO: XXX if a class list of all inner classes
	// are formed then this will need to be updated
	free (bin->cf2.flags_str);
	free (bin->cf2.this_class_name);
	if (bin == R_BIN_JAVA_GLOBAL_BIN) {
		R_BIN_JAVA_GLOBAL_BIN = NULL;
	}
	free (bin->file);
	free (bin);
	return NULL;
}