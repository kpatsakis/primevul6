R_API RBinJavaAttrInfo *r_bin_java_get_attr_from_field(RBinJavaField *field, R_BIN_JAVA_ATTR_TYPE attr_type, ut32 pos) {
	/*
	Search through the Attribute list for the given type starting at position pos.
	rvalue: NULL or the first occurrence of attr_type after pos
	*/
	RBinJavaAttrInfo *attr = NULL, *item;
	RListIter *iter;
	ut32 i = 0;
	if (field) {
		r_list_foreach (field->attributes, iter, item) {
			// Note the increment happens after the comparison
			if ((i++) >= pos) {
				if (item && (item->type == attr_type)) {
					attr = item;
					break;
				}
			}
		}
	}
	return attr;
}