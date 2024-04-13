R_API int is_class_interface(RBinJavaObj *bin, RBinJavaCPTypeObj *cp_obj) {
	RBinJavaInterfaceInfo *ifobj;
	RListIter *iter;
	int res = false;
	r_list_foreach (bin->interfaces_list, iter, ifobj) {
		if (ifobj) {
			res = cp_obj == ifobj->cp_class;
			if (res) {
				break;
			}
		}
	}
	return res;
}