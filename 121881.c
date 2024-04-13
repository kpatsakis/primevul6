R_API ut8 r_bin_java_does_cp_idx_ref_method(RBinJavaObj *BIN_OBJ, int idx) {
	RBinJavaField *fm_type = NULL;
	RListIter *iter;
	ut8 res = 0;
	r_list_foreach (BIN_OBJ->methods_list, iter, fm_type) {
		if (fm_type->field_ref_cp_obj->metas->ord == idx) {
			res = 1;
			break;
		}
	}
	return res;
}