static ut32 regsluop(ArmOp *op, int k) {
	ut32 data = UT32_MAX;

	if (op->operands[1].reg_type & ARM_REG32) {
		return data;
	}
	if (op->operands[0].reg_type & ARM_REG32) {
		k -= 0x40;
	}
	if (op->operands[2].type & ARM_GPR) {
		return data;
	}

	int n = op->operands[2].immediate;
	if (n > 0xff || n < -0x100) {
		return data;
	}

	data = k | op->operands[0].reg << 24 | op->operands[1].reg << 29 | (op->operands[1].reg & 56) << 13;

	if (n < 0) {
		n *= -1;
		data |= ( 0xf & (0xf - (n - 1)) ) << 20;

		if (countTrailingZeros(n) > 3) {
			data |= (0x1f - ((n >> 4) - 1)) << 8;
		} else {
			data |= (0x1f - (n >> 4)) << 8;
		}
	} else {
		data |= (0xf & (n & 63)) << 20;
		if (countTrailingZeros(n) < 4) {
			data |= (n >> 4) << 8;
		} else {
			data |= (0xff & n) << 4;
		}
		data |= (n >> 8) << 8;
	}

	return data;
}