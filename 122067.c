R_API RList *r_bin_java_get_interface_names(RBinJavaObj *bin) {
	RList *interfaces_names = r_list_new ();
	RListIter *iter;
	RBinJavaInterfaceInfo *ifobj;
	r_list_foreach (bin->interfaces_list, iter, ifobj) {
		if (ifobj && ifobj->name) {
			r_list_append (interfaces_names, strdup (ifobj->name));
		}
	}
	return interfaces_names;
}