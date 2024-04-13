R_API char *r_bin_java_get_item_desc_from_cp_item_list(RList *cp_list, RBinJavaCPTypeObj *obj, int depth) {
	/*
	Given a constant poool object FieldRef, MethodRef, or InterfaceMethodRef
	return the actual descriptor string.
	@rvalue ut8* (user frees) or NULL
	*/
	if (!obj || !cp_list || depth < 0) {
		return NULL;
	}
	switch (obj->tag) {
	case R_BIN_JAVA_CP_NAMEANDTYPE:
		return r_bin_java_get_utf8_from_cp_item_list (cp_list,
			obj->info.cp_name_and_type.descriptor_idx);
	// XXX - Probably not good form, but they are the same memory structure
	case R_BIN_JAVA_CP_FIELDREF:
	case R_BIN_JAVA_CP_INTERFACEMETHOD_REF:
	case R_BIN_JAVA_CP_METHODREF:
		obj = r_bin_java_get_item_from_cp_item_list (cp_list,
			obj->info.cp_method.name_and_type_idx);
		return r_bin_java_get_item_desc_from_cp_item_list (
			cp_list, obj, depth - 1);
	default:
		return NULL;
	}
	return NULL;
}