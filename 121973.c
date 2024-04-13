R_API DsoJsonObj *r_bin_java_get_class_info_json(RBinJavaObj *bin) {
	RList *classes = r_bin_java_get_classes (bin);
	DsoJsonObj *interfaces_list = dso_json_list_new ();
	DsoJsonObj *class_info_dict = dso_json_dict_new ();
	RBinClass *class_ = r_list_get_n (classes, 0);

	if (class_) {
		int dummy = 0;
		RListIter *iter;
		RBinClass *class_v = NULL;
		// add access flags like in methods
		bool is_public = ((class_->visibility & R_BIN_JAVA_CLASS_ACC_PUBLIC) != 0);
		bool is_final = ((class_->visibility & R_BIN_JAVA_CLASS_ACC_FINAL) != 0);
		bool is_super = ((class_->visibility & R_BIN_JAVA_CLASS_ACC_SUPER) != 0);
		bool is_interface = ((class_->visibility & R_BIN_JAVA_CLASS_ACC_INTERFACE) != 0);
		bool is_abstract = ((class_->visibility & R_BIN_JAVA_CLASS_ACC_ABSTRACT) != 0);
		bool is_synthetic = ((class_->visibility & R_BIN_JAVA_CLASS_ACC_SYNTHETIC) != 0);
		bool is_annotation = ((class_->visibility & R_BIN_JAVA_CLASS_ACC_ANNOTATION) != 0);
		bool is_enum = ((class_->visibility & R_BIN_JAVA_CLASS_ACC_ENUM) != 0);

		dso_json_dict_insert_str_key_num (class_info_dict, "access_flags", class_->visibility);
		dso_json_dict_insert_str_key_num (class_info_dict, "is_public", is_public);
		dso_json_dict_insert_str_key_num (class_info_dict, "is_final", is_final);
		dso_json_dict_insert_str_key_num (class_info_dict, "is_super", is_super);
		dso_json_dict_insert_str_key_num (class_info_dict, "is_interface", is_interface);
		dso_json_dict_insert_str_key_num (class_info_dict, "is_abstract", is_abstract);
		dso_json_dict_insert_str_key_num (class_info_dict, "is_synthetic", is_synthetic);
		dso_json_dict_insert_str_key_num (class_info_dict, "is_annotation", is_annotation);
		dso_json_dict_insert_str_key_num (class_info_dict, "is_enum", is_enum);
		dso_json_dict_insert_str_key_str (class_info_dict, "name", class_->name);

		if (!class_->super) {
			DsoJsonObj *str = dso_json_str_new ();
			dso_json_dict_insert_str_key_obj (class_info_dict, "super", str);
			dso_json_str_free (str);
		} else {
			dso_json_dict_insert_str_key_str (class_info_dict, "super", class_->super);
		}

		r_list_foreach (classes, iter, class_v) {
			if (!dummy) {
				dummy++;
				continue;
			}
			// enumerate all interface classes and append them to the interfaces
			if ((class_v->visibility & R_BIN_JAVA_CLASS_ACC_INTERFACE) != 0) {
				dso_json_list_append_str (interfaces_list, class_v->name);
			}
		}
	}
	dso_json_dict_insert_str_key_obj (class_info_dict, "interfaces", interfaces_list);
	r_list_free (classes);
	// dso_json_list_free (interfaces_list);
	dso_json_obj_del (interfaces_list);
	return class_info_dict;
}