static ut32 exception(ArmOp *op, ut32 k) {
	ut32 data = UT32_MAX;

	if (op->operands[0].type == ARM_CONSTANT) {
		int n = op->operands[0].immediate;
		data = k;
		data += (((n / 8) & 0xff) << 16);
		data += n << 29;//((n >> 8) << 8);
	}
	return data;
}