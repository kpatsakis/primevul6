static int countTrailingZeros(ut32 x) {
	int count = 0;
	while (x > 0) {
		if ((x & 1) == 1) {
			break;
		} else {
			count ++;
			x = x >> 1;
		}
	}
	return count;
}