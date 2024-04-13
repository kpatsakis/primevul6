R_API void U(copy_type_info_to_stack_frame_list_up_to_idx)(RList * type_list, RList * sf_list, ut64 idx) {
	RListIter *iter, *iter_tmp;
	RBinJavaVerificationObj *ver_obj, *new_ver_obj;
	ut32 pos = 0;
	if (type_list == NULL) {
		return;
	}
	if (sf_list == NULL) {
		return;
	}
	r_list_foreach_safe (type_list, iter, iter_tmp, ver_obj) {
		new_ver_obj = (RBinJavaVerificationObj *) malloc (sizeof (RBinJavaVerificationObj));
		// FIXME: how to handle failed memory allocation?
		if (new_ver_obj && ver_obj) {
			memcpy (new_ver_obj, ver_obj, sizeof (RBinJavaVerificationObj));
			if (!r_list_append (sf_list, (void *) new_ver_obj)) {
				R_FREE (new_ver_obj);
			}
		} else {
			R_FREE (new_ver_obj);
		}
		pos++;
		if (pos == idx) {
			break;
		}
	}
}