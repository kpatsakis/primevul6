R_API void r_bin_java_default_free(void /* RBinJavaCPTypeObj*/ *o) {
	RBinJavaCPTypeObj *obj = o;
	if (obj) {
		free (obj->metas);
		free (obj->name);
		free (obj->value);
		free (obj);
	}
}