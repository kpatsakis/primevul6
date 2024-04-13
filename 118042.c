static ut32 orr(ArmOp *op, int addr) {
	ut32 data = UT32_MAX;

	if (op->operands[2].type & ARM_GPR) {
		// All operands need to be the same
		if (!(op->operands[0].reg_type == op->operands[1].reg_type &&
	 	    op->operands[1].reg_type == op->operands[2].reg_type)) {
		 	   return data;
		}
		if (op->operands[0].reg_type & ARM_REG64) {
			data = 0x000000aa;
		} else {
			data = 0x0000002a;
		}
		data += op->operands[0].reg << 24;
		data += op->operands[1].reg << 29;
		data += (op->operands[1].reg >> 3)  << 16;
		data += op->operands[2].reg << 8;
	} else if (op->operands[2].type & ARM_CONSTANT) {
		// Reg types need to match
		if (!(op->operands[0].reg_type == op->operands[1].reg_type)) {
			return data;
		}
		if (op->operands[0].reg_type & ARM_REG64) {
			data = 0x000040b2;
		} else {
			data = 0x00000032;
		}

		data += op->operands[0].reg << 24;
		data += op->operands[1].reg << 29;
		data += (op->operands[1].reg >> 3)  << 16;

		ut32 imm = decodeBitMasks (op->operands[2].immediate);
		if (imm == -1) {
			return imm;
		}
		int low = imm & 0xF;
		if (op->operands[0].reg_type & ARM_REG64) {
			imm = ((imm >> 6) | 0x78);
			if (imm > 120) {
				data |= imm << 8;
			}
		} else {
			imm = ((imm >> 2));
			if (imm > 120) {
				data |= imm << 4;
			}
		}
		data |= (4 * low) << 16;
	}
	return data;
}