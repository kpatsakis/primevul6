R_API void r_bin_java_attribute_free(void /*RBinJavaAttrInfo*/ *a) {
	RBinJavaAttrInfo *attr = a;
	if (attr) {
		IFDBG eprintf("Deleting attr %s, %p\n", attr->name, attr);
		((RBinJavaAttrMetas *) attr->metas->type_info)->allocs->delete_obj (attr);
		// free (attr->metas);
		// free (attr);
	}
}