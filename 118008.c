static int countLeadingZeros(ut32 x) {
	int count = 0;
	while (x) {
		x >>= 1;
		--count;
	}
	return count;
}