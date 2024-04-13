R_API ut64 rbin_java_verification_info_calc_size(RBinJavaVerificationObj *se) {
	ut64 sz = 1;
	if (!se) {
		return 0;
	}
	// r_buf_read_at (bin->b, offset, (ut8*)(&se->tag), 1)
	switch (se->tag) {
	case R_BIN_JAVA_STACKMAP_OBJECT:
		// r_buf_read_at (bin->b, offset+1, (ut8*)buf, 2)
		sz += 2;
		break;
	case R_BIN_JAVA_STACKMAP_UNINIT:
		// r_buf_read_at (bin->b, offset+1, (ut8*)buf, 2)
		sz += 2;
		break;
	}
	return sz;
}