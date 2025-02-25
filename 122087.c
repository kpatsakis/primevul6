R_API RBinJavaAttrInfo *r_bin_java_exceptions_attr_new(ut8 *buffer, ut64 sz, ut64 buf_offset) {
	ut32 i = 0, offset = 0;
	ut64 size;
	RBinJavaAttrInfo *attr = NULL;
	attr = r_bin_java_default_attr_new (buffer, sz, buf_offset);
	offset += 6;
	if (attr == NULL) {
		// TODO eprintf
		return attr;
	}
	attr->type = R_BIN_JAVA_ATTR_TYPE_LINE_NUMBER_TABLE_ATTR;
	attr->info.exceptions_attr.number_of_exceptions = R_BIN_JAVA_USHORT (buffer, offset);
	offset += 2;
	size = sizeof (ut16) * attr->info.exceptions_attr.number_of_exceptions;
	if (size < attr->info.exceptions_attr.number_of_exceptions) {
		free (attr);
		return NULL;
	}
	attr->info.exceptions_attr.exception_idx_table = (ut16 *) malloc (size);
	if (!attr->info.exceptions_attr.exception_idx_table) {
		free (attr);
		return NULL;
	}
	for (i = 0; i < attr->info.exceptions_attr.number_of_exceptions; i++) {
		attr->info.exceptions_attr.exception_idx_table[i] = R_BIN_JAVA_USHORT (buffer, offset);
		offset += 2;
	}
	attr->size = offset;
	// IFDBG r_bin_java_print_exceptions_attr_summary(attr);
	return attr;
}