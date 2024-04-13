static st8 mem_32bit_2reg(ArmOpcode *ao, ut64 m) {
	ut8 rd = getreg (ao->a[0]);
	ut8 rn = getregmemstart (ao->a[1]);
	if ((rd > 15) || (rn > 15) || (m & DOTN_BIT)) {
		return -1;
	}
	ao->o |= rd << 4;
	ao->o |= rn << 24;
	return 4;
}