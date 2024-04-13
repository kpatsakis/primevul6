static ut32 adrp(ArmOp *op, ut64 addr, ut32 k) { //, int reg, ut64 dst) {
	ut64 at = 0LL;
	ut32 data = k;
	if (op->operands[0].type == ARM_GPR) {
		data += ((op->operands[0].reg & 0xff) << 24);
	} else {
		eprintf ("Usage: adrp x0, addr\n");
		return UT32_MAX;
	}
	if (op->operands[1].type == ARM_CONSTANT) {
		// XXX what about negative values?
		at = op->operands[1].immediate - addr;
		at /= 4;
	} else {
		eprintf ("Usage: adrp, x0, addr\n");
		return UT32_MAX;
	}
	ut8 b0 = at;
	ut8 b1 = (at >> 3) & 0xff;

#if 0
	ut8 b2 = (at >> (8 + 7)) & 0xff;
	data += b0 << 29;
	data += b1 << 16;
	data += b2 << 24;
#endif
	data += b0 << 16;
	data += b1 << 8;
	return data;
}