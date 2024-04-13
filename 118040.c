static ut32 getthbimmed(st32 number) {
	ut32 res = 0;
	if (number < 0) {
		res |= 1 << 18;
	}
	number >>= 1;
	res |= (( number & 0xff) << 8);
	number >>= 8;
	res |= ( number & 0x07);
	number >>= 3;
	res |= (( number & 0xff) << 24);
	number >>= 8;
	res |= (( number & 0x3) << 16);
	number >>= 2;
	if (number < 0) {
		res |= (( number & 0x1) << 3);
		number >>= 1;
		res |= (( number & 0x1) << 5);
	} else {
		res |= ((!( number & 0x1)) << 3);
		number >>= 1;
		res |= ((!( number & 0x1)) << 5);
	}
	return res;
}