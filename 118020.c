static ut32 branch(ArmOp *op, ut64 addr, int k) {
	ut32 data = UT32_MAX;
	int n = 0;
	if (op->operands[0].type & ARM_CONSTANT) {
		n = op->operands[0].immediate;
		if (!(n & 0x3 || n > 0x7ffffff)) {
			n -= addr;
			n = n >> 2;
			int t = n >> 24;
			int h = n >> 16;
			int m = (n & 0xff00) >> 8;
			n &= 0xff;
			data = k;
			data |= n << 24;
			data |= m << 16;
			data |= h << 8;
			data |= t;
		}
	} else {
		n = op->operands[0].reg;
		if (n < 0 || n > 31) {
			return -1;
		}
		n = n << 5;
		int h = n >> 8;
		n &= 0xff;
		data = k;
		data |= n << 24;
		data |= h << 16;
	}
	return data;
}