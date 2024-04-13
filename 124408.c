i64 nloops(i64 seconds, uchar *b1, uchar *b2)
{
	i64 nloops;
	int nbits;

	nloops = ARBITRARY_AT_EPOCH * pow(MOORE_TIMES_PER_SECOND, seconds);
	if (nloops < ARBITRARY)
		nloops = ARBITRARY;
	for (nbits = 0; nloops > 255; nbits ++)
		nloops = nloops >> 1;
	*b1 = nbits;
	*b2 = nloops;
	return nloops << nbits;
}