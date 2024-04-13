static ut32 stp(ArmOp *op, int k) {
	ut32 data = UT32_MAX;

	if (op->operands[3].immediate & 0x7) {
		return data;
	}
	if (k == 0x000040a9 && (op->operands[0].reg == op->operands[1].reg)) {
		return data;
	}

	data = k;
	data += op->operands[0].reg << 24;
	data += op->operands[1].reg << 18;
	data += (op->operands[2].reg & 0x7) << 29;
	data += (op->operands[2].reg >> 3) << 16;
	data += (op->operands[3].immediate & 0x8) << 20;
	data += (op->operands[3].immediate >> 4) << 8;
	return data;
}