static ut32 bdot(ArmOp *op, ut64 addr, int k) {
	ut32 data = UT32_MAX;
	int n = 0;
	int a = 0;
	n = op->operands[0].immediate;
	// I am sure there's a logical way to do negative offsets,
	// but I was unable to find any sensible docs so I did my best
	if (!(n & 0x3 || n > 0x7ffffff)) {
		n -= addr;
		data = k;
		if (n < 0) {
			n *= -1;
			a = (n << 3) - 1;
			data |= (0xff - a) << 24;

			a = calcNegOffset(n, 5);
			data |= a << 16;

			a = calcNegOffset(n, 13);
			data |= a << 8;
		} else {
			data |= (n & 31) << 27;
			data |= (0xff & (n >> 5)) << 16;
			data |= (0xff & (n >> 13)) << 8;
		}
	}

	return data;
}