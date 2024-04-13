static ut32 getthzeroimmed16(ut32 number) {
	ut32 res = 0;
	res |= (number & 0xf000) << 12;
	res |= (number & 0x0800) << 7;
	res |= (number & 0x0700) >> 4;
	res |= (number & 0x00ff) << 8;
	return res;
}