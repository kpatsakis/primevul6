static int countTrailingOnes(ut32 x) {
	return countTrailingZeros (~x);
}