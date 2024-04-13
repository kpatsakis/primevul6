R_API RBinJavaStackMapFrameMetas *r_bin_java_determine_stack_frame_type(ut8 tag) {
	ut8 type_value = 0;
	if (tag < 64) {
		type_value = R_BIN_JAVA_STACK_FRAME_SAME;
	} else if (63 < tag && tag < 128) {
		type_value = R_BIN_JAVA_STACK_FRAME_SAME_LOCALS_1;
	} else if (247 < tag && tag < 251) {
		type_value = R_BIN_JAVA_STACK_FRAME_CHOP;
	} else if (tag == 251) {
		type_value = R_BIN_JAVA_STACK_FRAME_SAME_FRAME_EXTENDED;
	} else if (251 < tag && tag < 255) {
		type_value = R_BIN_JAVA_STACK_FRAME_APPEND;
	} else if (tag == 255) {
		type_value = R_BIN_JAVA_STACK_FRAME_FULL_FRAME;
	} else {
		type_value = R_BIN_JAVA_STACK_FRAME_RESERVED;
	}
	return &R_BIN_JAVA_STACK_MAP_FRAME_METAS[type_value];
}