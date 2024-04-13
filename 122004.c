R_API ut64 r_bin_java_stack_map_frame_calc_size(RBinJavaStackMapFrame *sf) {
	ut64 size = 0;
	RListIter *iter, *iter_tmp;
	RBinJavaVerificationObj *se;
	if (sf) {
		// sf->tag = buffer[offset];
		size += 1;
		switch (sf->type) {
		case R_BIN_JAVA_STACK_FRAME_SAME:
			// Nothing to read
			break;
		case R_BIN_JAVA_STACK_FRAME_SAME_LOCALS_1:
			r_list_foreach_safe (sf->stack_items, iter, iter_tmp, se) {
				size += rbin_java_verification_info_calc_size (se);
			}
			break;
		case R_BIN_JAVA_STACK_FRAME_CHOP:
			// sf->offset_delta = R_BIN_JAVA_USHORT (buffer, offset);
			size += 2;
			break;
		case R_BIN_JAVA_STACK_FRAME_SAME_FRAME_EXTENDED:
			// sf->offset_delta = R_BIN_JAVA_USHORT (buffer, offset);
			size += 2;
			r_list_foreach_safe (sf->stack_items, iter, iter_tmp, se) {
				size += rbin_java_verification_info_calc_size (se);
			}
			break;
		case R_BIN_JAVA_STACK_FRAME_APPEND:
			// sf->offset_delta = R_BIN_JAVA_USHORT (buffer, offset);
			size += 2;
			r_list_foreach_safe (sf->stack_items, iter, iter_tmp, se) {
				size += rbin_java_verification_info_calc_size (se);
			}
			break;
		case R_BIN_JAVA_STACK_FRAME_FULL_FRAME:
			// sf->offset_delta = R_BIN_JAVA_USHORT (buffer, offset);
			size += 2;
			// sf->number_of_locals = R_BIN_JAVA_USHORT (buffer, offset);
			size += 2;
			r_list_foreach_safe (sf->local_items, iter, iter_tmp, se) {
				size += rbin_java_verification_info_calc_size (se);
			}
			// sf->number_of_stack_items = R_BIN_JAVA_USHORT (buffer, offset);
			size += 2;
			r_list_foreach_safe (sf->stack_items, iter, iter_tmp, se) {
				size += rbin_java_verification_info_calc_size (se);
			}
			break;
		default:
			eprintf ("Unknown type\n");
			break;
		}
	}
	return size;
}