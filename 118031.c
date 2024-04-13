static st8 std_32bit_2reg(ArmOpcode *ao, ut64 m, bool shift) {
	ut8 rd = getreg (ao->a[0]);
	ut8 rn = getreg (ao->a[1]);
	if ((rd > 15) || (rn > 15) || (m & DOTN_BIT)) {
		return -1;
	}
	if (m & S_BIT) {
		ao->o |= 1 << 28;
	}
	if (shift) {
		err = false;
		ut32 shiftnum = thumb_getshift (ao->a[2]);
		if (err) {
			return -1;
		}
		ao->o |= shiftnum;
		ao->o |= rd << 24;
		ao->o |= rn << 8;
	} else {
		ao->o |= rd;
		ao->o |= rn << 24;
	}
	return 4;
}