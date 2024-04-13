R_API char *r_bin_java_get_name_from_cp_item_list(RList *cp_list, ut64 idx) {
	/*
	Given a constant poool object Class, FieldRef, MethodRef, or InterfaceMethodRef
	return the actual descriptor string.
	@param cp_list: RList of RBinJavaCPTypeObj *
	@param obj object to look up the name for
	@rvalue ut8* (user frees) or NULL
	*/
	RBinJavaCPTypeObj *obj = r_bin_java_get_item_from_cp_item_list (
		cp_list, idx);
	if (obj && cp_list) {
		return r_bin_java_get_item_name_from_cp_item_list (
			cp_list, obj, MAX_CPITEMS);
	}
	return NULL;
}