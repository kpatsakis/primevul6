static int countLeadingOnes(ut32 x) {
	return countLeadingZeros (~x);
}