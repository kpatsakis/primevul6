R_API RList *r_bin_java_get_classes(RBinJavaObj *bin) {
	RList *classes = r_list_newf (r_bin_java_classes_free);
	RListIter *iter;
	RBinJavaCPTypeObj *cp_obj = NULL;
	RBinJavaCPTypeObj *this_class_cp_obj = r_bin_java_get_item_from_bin_cp_list (bin, bin->cf2.this_class);
	ut32 idx = 0;
	RBinClass *k = R_NEW0 (RBinClass);
	if (!k) {
		r_list_free (classes);
		return NULL;
	}
	k->visibility = bin->cf2.access_flags;
	if (bin->cf2.flags_str) {
		k->visibility_str = strdup (bin->cf2.flags_str);
	}
	k->methods = r_bin_java_enum_class_methods (bin, bin->cf2.this_class);
	k->fields = r_bin_java_enum_class_fields (bin, bin->cf2.this_class);
	k->name = r_bin_java_get_this_class_name (bin);
	k->super = r_bin_java_get_name_from_bin_cp_list (bin, bin->cf2.super_class);
	k->index = (idx++);
	r_list_append (classes, k);
	r_list_foreach (bin->cp_list, iter, cp_obj) {
		if (cp_obj && cp_obj->tag == R_BIN_JAVA_CP_CLASS
		&& (this_class_cp_obj != cp_obj && is_class_interface (bin, cp_obj))) {
			k = R_NEW0 (RBinClass);
			if (!k) {
				break;
			}
			k->methods = r_bin_java_enum_class_methods (bin, cp_obj->info.cp_class.name_idx);
			k->fields = r_bin_java_enum_class_fields (bin, cp_obj->info.cp_class.name_idx);
			k->index = idx;
			k->name = r_bin_java_get_item_name_from_bin_cp_list (bin, cp_obj);
			r_list_append (classes, k);
			idx++;
		}
	}
	return classes;
}