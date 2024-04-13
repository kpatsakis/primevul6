static int _(r_cmd_java_check_op_idx) (const ut8 *op_bytes, ut16 idx) {
	return R_BIN_JAVA_USHORT (op_bytes, 0) == idx;
}