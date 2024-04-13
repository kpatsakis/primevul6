static ut32 arithmetic (ArmOp *op, int k) {
	ut32 data = UT32_MAX;
	if (op->operands_count < 3) {
		return data;
	}

	if (!(op->operands[0].type & ARM_GPR &&
	      op->operands[1].type & ARM_GPR)) {
		return data;
	}
	if (op->operands[2].type & ARM_GPR) {
		k -= 6;
	}

	data = k;
	data += op->operands[0].reg << 24;
	data += (op->operands[1].reg & 7) << (24 + 5);
	data += (op->operands[1].reg >> 3) << 16;
	if (op->operands[2].reg_type & ARM_REG64) {
		data += op->operands[2].reg << 8;
	} else {
		data += (op->operands[2].reg & 0x3f) << 18;
		data += (op->operands[2].reg >> 6) << 8;
	}
	return data;
}