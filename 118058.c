static st8 std_16bit_2reg(ArmOpcode *ao, ut64 m) {
	ut8 rd = getreg (ao->a[0]);
	ut8 rn = getreg (ao->a[1]);
	if ( (rd < 8) && (rn < 8) && !(m & DOTW_BIT)) {
		ao->o |= rd << 8;
		ao->o |= rn << 11;
		return 2;
	}
	return 0;
}