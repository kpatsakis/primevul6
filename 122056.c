R_API void r_bin_java_constant_pool(void /*RBinJavaCPTypeObj*/ *o) {
	RBinJavaCPTypeObj *obj = o;
	if (obj != &R_BIN_JAVA_NULL_TYPE) {
		((RBinJavaCPTypeMetas *) obj->metas->type_info)->allocs->delete_obj (obj);
	}
}