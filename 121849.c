R_API void r_bin_java_element_value_free(void /*RBinJavaElementValue*/ *e) {
	RBinJavaElementValue *element_value = e;
	RListIter *iter = NULL, *iter_tmp = NULL;
	RBinJavaCPTypeObj *obj = NULL;
	RBinJavaElementValue *ev_element = NULL;
	if (element_value) {
		free (element_value->metas);
		element_value->metas = NULL;
		switch (element_value->tag) {
		case R_BIN_JAVA_EV_TAG_BYTE:
		case R_BIN_JAVA_EV_TAG_CHAR:
		case R_BIN_JAVA_EV_TAG_DOUBLE:
		case R_BIN_JAVA_EV_TAG_FLOAT:
		case R_BIN_JAVA_EV_TAG_INT:
		case R_BIN_JAVA_EV_TAG_LONG:
		case R_BIN_JAVA_EV_TAG_SHORT:
		case R_BIN_JAVA_EV_TAG_BOOLEAN:
		case R_BIN_JAVA_EV_TAG_STRING:
			// Delete the CP Type Object
			obj = element_value->value.const_value.const_value_cp_obj;
			((RBinJavaCPTypeMetas *) obj->metas->type_info)->allocs->delete_obj (obj);
			break;
		case R_BIN_JAVA_EV_TAG_ENUM:
			// Delete the CP Type Objects
			obj = element_value->value.enum_const_value.const_name_cp_obj;
			((RBinJavaCPTypeMetas *) obj->metas->type_info)->allocs->delete_obj (obj);
			obj = element_value->value.enum_const_value.type_name_cp_obj;
			((RBinJavaCPTypeMetas *) obj->metas->type_info)->allocs->delete_obj (obj);
			break;
		case R_BIN_JAVA_EV_TAG_CLASS:
			// Delete the CP Type Object
			obj = element_value->value.class_value.class_info_cp_obj;
			((RBinJavaCPTypeMetas *) obj->metas->type_info)->allocs->delete_obj (obj);
			break;
		case R_BIN_JAVA_EV_TAG_ARRAY:
			// Delete the Element Value array List
			r_list_foreach_safe (element_value->value.array_value.values, iter, iter_tmp, ev_element) {
				if (ev_element) {
					r_bin_java_element_value_free (ev_element);
				} else {
					// TODO eprintf ev_pairs value was NULL
				}
				// r_list_delete (element_value->value.array_value.values, iter);
				ev_element = NULL;
			}
			r_list_free (element_value->value.array_value.values);
			break;
		case R_BIN_JAVA_EV_TAG_ANNOTATION:
			// Delete the Annotations List
			r_list_free (element_value->value.annotation_value.element_value_pairs);
			break;
		default:
			// eprintf unable to free the tag
			break;
		}
		free (element_value);
	}
}