R_API void r_bin_java_obj_free(void /*RBinJavaCPTypeObj*/ *o) {
	RBinJavaCPTypeObj *obj = o;
	((RBinJavaCPTypeMetas *) obj->metas->type_info)->allocs->delete_obj (obj);
}