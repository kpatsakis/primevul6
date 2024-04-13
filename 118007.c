static ut32 msr(ArmOp *op, int w) {
	ut32 data = UT32_MAX;
	int i;
	ut32 r, b;
	/* handle swapped args */
	if (w) {
		if (op->operands[1].reg_type != (ARM_REG64 | ARM_SP)) {
			if (op->operands[1].type == ARM_CONSTANT) {
				for (i = 0; msr_const[i].name; i++) {
					if (op->operands[1].immediate == msr_const[i].val) {
						op->operands[1].sp_val = msr_const[i].val;
						op->operands[1].reg = op->operands[1].immediate;
						break;
					}
				}
			} else {
				return data;
			}
		}
		r = op->operands[0].reg;
		b = msrk (op->operands[0].sp_val);
	} else {
		if (op->operands[0].reg_type != (ARM_REG64 | ARM_SP)) {
			if (op->operands[0].type == ARM_CONSTANT) {
				for (i = 0; msr_const[i].name; i++) {
					if (op->operands[0].immediate == msr_const[i].val) {
						op->operands[0].sp_val = msr_const[i].val;
						op->operands[0].reg = op->operands[0].immediate;
						break;
					}
				}
			} else {
				return data;
			}
		}
		r = op->operands[0].reg;
		b = msrk (op->operands[0].sp_val);
	}
	data = (r << 24) | b | 0xd5;
	if (w) {
		/* mrs */
		data |= 0x413000;
	}
	if (op->operands[1].reg_type == ARM_REG64) {
		data |= op->operands[1].reg << 24;
	}

	return data;
}