static int calcNegOffset(int n, int shift) {
	int a = n >> shift;
	if (a == 0) {
		return 0xff;
	}
	// find first set bit then invert it and all
	// bits below it
	int t = 0x400;
	while (!(t & a) && a != 0 && t != 0) {
		t = t >> 1;
	}
	t = t & (t - 1);
	a = a ^ t;
	// If bits below 32 are set
	if (countTrailingZeros(n) > shift) {
		a--;
	}
	return 0xff & (0xff - a);
}