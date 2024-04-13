static st8 std_32bit_3reg(ArmOpcode *ao, ut64 m, bool shift) {
	ut8 rd = getreg (ao->a[0]);
	ut8 rn = getreg (ao->a[1]);
	ut8 rm = getreg (ao->a[2]);
	if ((rd > 15) || (rn > 15) || (rm > 15) || (m & DOTN_BIT)) {
		return -1;
	}
	ao->o |= rd;
	ao->o |= rn << 24;
	ao->o |= rm << 8;
	if (shift) {
		err = false;
		ut32 shiftnum = thumb_getshift (ao->a[3]);
		if (err) {
			return -1;
		}
		ao->o |= shiftnum;
	}
	if (m & S_BIT) {
		ao->o |= 1 << 28;
	}
	return 4;
}