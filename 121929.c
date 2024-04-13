R_API void r_bin_java_annotation_default_attr_free(void /*RBinJavaAttrInfo*/ *a) {
	RBinJavaAttrInfo *attr = a;
	RBinJavaElementValue *element_value = NULL, *ev_element = NULL;
	RBinJavaCPTypeObj *obj = NULL;
	RListIter *iter = NULL, *iter_tmp = NULL;
	if (attr == NULL || attr->type != R_BIN_JAVA_ATTR_TYPE_ANNOTATION_DEFAULT_ATTR) {
		return;
	}
	element_value = (attr->info.annotation_default_attr.default_value);
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
			r_bin_java_element_value_free (ev_element);
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
	if (attr) {
		free (attr->name);
		free (attr->metas);
		free (attr);
	}
}