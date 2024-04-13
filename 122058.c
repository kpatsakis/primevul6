R_API RList *r_bin_java_get_bin_obj_list_thru_obj(RBinJavaObj *bin_obj) {
	RList *the_list;
	Sdb *sdb;
	if (!bin_obj) {
		return NULL;
	}
	sdb = bin_obj->AllJavaBinObjs;
	if (!sdb) {
		return NULL;
	}
	the_list = r_list_new ();
	if (!the_list) {
		return NULL;
	}
	sdb_foreach (sdb, sdb_iterate_build_list, (void *) the_list);
	return the_list;
}