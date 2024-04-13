R_API void r_bin_java_print_element_value_summary(RBinJavaElementValue *element_value) {
	RBinJavaCPTypeObj *obj;
	RBinJavaElementValue *ev_element = NULL;
	RListIter *iter = NULL, *iter_tmp = NULL;
	char *name;
	if (element_value == NULL) {
		eprintf ("Attempting to print an invalid RBinJavaElementValuePair *pair.\n");
		return;
	}
	name = ((RBinJavaElementValueMetas *) element_value->metas->type_info)->name;
	eprintf ("Element Value information:\n");
	eprintf ("   EV Pair File Offset: 0x%08"PFMT64x "\n", element_value->file_offset);
	eprintf ("   EV Value Type (%d): %s\n", element_value->tag, name);
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
		eprintf ("   EV Value Constant Value index: 0x%02x\n", element_value->value.const_value.const_value_idx);
		eprintf ("   EV Value Constant Value Information:\n");
		obj = element_value->value.const_value.const_value_cp_obj;
		((RBinJavaCPTypeMetas *) obj->metas->type_info)->allocs->print_summary (obj);
		break;
	case R_BIN_JAVA_EV_TAG_ENUM:
		eprintf ("   EV Value Enum Constant Value Const Name Index: 0x%02x\n", element_value->value.enum_const_value.const_name_idx);
		eprintf ("   EV Value Enum Constant Value Type Name Index: 0x%02x\n", element_value->value.enum_const_value.type_name_idx);
		eprintf ("   EV Value Enum Constant Value Const CP Information:\n");
		obj = element_value->value.enum_const_value.const_name_cp_obj;
		((RBinJavaCPTypeMetas *) obj->metas->type_info)->allocs->print_summary (obj);
		eprintf ("   EV Value Enum Constant Value Type CP Information:\n");
		obj = element_value->value.enum_const_value.type_name_cp_obj;
		((RBinJavaCPTypeMetas *) obj->metas->type_info)->allocs->print_summary (obj);
		break;
	case R_BIN_JAVA_EV_TAG_CLASS:
		eprintf ("   EV Value Class Info Index: 0x%02x\n", element_value->value.class_value.class_info_idx);
		eprintf ("   EV Value Class Info CP Information:\n");
		obj = element_value->value.class_value.class_info_cp_obj;
		((RBinJavaCPTypeMetas *) obj->metas->type_info)->allocs->print_summary (obj);
		break;
	case R_BIN_JAVA_EV_TAG_ARRAY:
		eprintf ("   EV Value Array Value Number of Values: 0x%04x\n", element_value->value.array_value.num_values);
		eprintf ("   EV Value Array Values\n");
		r_list_foreach_safe (element_value->value.array_value.values, iter, iter_tmp, ev_element) {
			r_bin_java_print_element_value_summary (ev_element);
		}
		break;
	case R_BIN_JAVA_EV_TAG_ANNOTATION:
		eprintf ("   EV Annotation Information:\n");
		r_bin_java_print_annotation_summary (&element_value->value.annotation_value);
		break;
	default:
		// eprintf unable to handle tag
		break;
	}
}