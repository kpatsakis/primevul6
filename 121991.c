R_API void r_bin_java_interface_free(void /*RBinJavaInterfaceInfo*/ *o) {
	RBinJavaInterfaceInfo *obj = o;
	if (obj) {
		free (obj->name);
		free (obj);
	}
}