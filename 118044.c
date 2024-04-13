static ut32 mem_barrier (ArmOp *op, ut64 addr, int k) {
	ut32 data = UT32_MAX;
	data = k;
	if (!strncmp (op->mnemonic, "isb", 3)) {
		if (op->operands[0].mem_option == 15 || op->operands[0].type == ARM_NOTYPE) {
			return data;
		} else {
			return UT32_MAX;
		}
	}
	if (op->operands[0].type == ARM_MEM_OPT) {
		data |= op->operands[0].mem_option << 16;
	} else if (op->operands_count == 1 && op->operands[0].type == ARM_CONSTANT) {
		data |= (op->operands[0].immediate << 16);
	}
	return data;
}